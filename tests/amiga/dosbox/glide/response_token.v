// Real upstream DMA stream engine and parser; injected GPU retirement signals.
`timescale 1ns/1ps
module response_token_test;
 reg clk=0,resetn=0,valid=0,rx_enable=1;
 reg [31:0] data=0;
 reg writes=0,textures=0,ack_ready=0,cancel=0,external_fault=0;
 wire ready,dma_idle,parser_idle,cmd_valid,cmd_ready,cmd_last;
 wire [31:0] cmd_data,response_data,token_epoch,token_sequence,ack_epoch,ack_sequence;
 wire response_valid,response_ready,response_last,rx_ready,token_valid,token_ready;
 wire protocol_fault,ack_valid,ack_cancelled,fault,busy;
 reg full_idle=0;
 always #5 clk=~clk;
 // Match RasterIX_EF's registered combination, not a pre-submit idle latch.
 always @(posedge clk)if(!resetn)full_idle<=0;else full_idle<=dma_idle&&parser_idle;
 FrameStreamingCore dma(
  .aclk(clk),.resetn(resetn),.idle(dma_idle),
  .s_st0_axis_tvalid(valid),.s_st0_axis_tready(ready),.s_st0_axis_tlast(1'b0),.s_st0_axis_tdata(data),
  .m_st1_axis_tvalid(cmd_valid),.m_st1_axis_tready(cmd_ready),.m_st1_axis_tlast(cmd_last),.m_st1_axis_tdata(cmd_data),
  .s_st1_axis_tvalid(1'b0),.s_st1_axis_tlast(1'b0),.s_st1_axis_tdata(32'b0),
  .m_st0_axis_tvalid(response_valid),.m_st0_axis_tready(response_ready),
  .m_st0_axis_tlast(response_last),.m_st0_axis_tdata(response_data),
  .m_mem_axi_awready(1'b1),.m_mem_axi_wready(1'b1),.m_mem_axi_bvalid(1'b0),
  .m_mem_axi_bid(8'b0),.m_mem_axi_bresp(2'b0),.m_mem_axi_arready(1'b1),
  .m_mem_axi_rid(8'b0),.m_mem_axi_rdata(32'b0),.m_mem_axi_rresp(2'b0),
  .m_mem_axi_rlast(1'b0),.m_mem_axi_rvalid(1'b0)
 );
 CommandParser parser(
  .aclk(clk),.resetn(resetn),.s_cmd_axis_tvalid(cmd_valid),.s_cmd_axis_tready(cmd_ready),
  .s_cmd_axis_tlast(cmd_last),.s_cmd_axis_tdata(cmd_data),
  .m_cmd_fog_axis_tready(1'b1),.m_cmd_rasterizer_axis_tready(1'b1),
  .m_cmd_tmu0_axis_tready(1'b1),.m_cmd_tmu1_axis_tready(1'b1),.m_cmd_config_axis_tready(1'b1),
  .rasterizerRunning(1'b0),.pixelInPipeline(1'b0),.dataInTriangleInterpolator(1'b0),
  .framebufferWritesPending(writes),.textureMemoryTransactionsPending(textures),.idle(parser_idle),
  .colorBufferApplied(1'b1),.depthBufferApplied(1'b1),.stencilBufferApplied(1'b1),.framebufferClearApplied(1'b1)
 );
 assign response_ready=rx_ready&&rx_enable;
 grx_response_token receiver(
  .clk(clk),.resetn(resetn),.s_valid(response_valid&&rx_enable),.s_ready(rx_ready),
  .s_data(response_data),.s_last(response_last),.external_fault(external_fault),
  .token_valid(token_valid),.token_ready(token_ready),.token_epoch(token_epoch),
  .token_sequence(token_sequence),.fault(protocol_fault)
 );
 grx_completion mailbox(
  .clk(clk),.resetn(resetn),.token_valid(token_valid),.token_ready(token_ready),
  .token_epoch(token_epoch),.token_sequence(token_sequence),.drained(full_idle),
  .cancel(cancel),.bus_error(protocol_fault||external_fault),.ack_valid(ack_valid),
  .ack_ready(ack_ready),.ack_epoch(ack_epoch),.ack_sequence(ack_sequence),
  .ack_cancelled(ack_cancelled),.fault(fault),.busy(busy)
 );
 task reset;
  begin
   @(negedge clk);resetn=0;valid=0;rx_enable=1;writes=0;textures=0;
   ack_ready=0;cancel=0;external_fault=0;
   repeat(3)@(negedge clk);resetn=1;repeat(8)@(negedge clk);
  end
 endtask
 task word(input [31:0] value);
  integer timeout;
  begin
   @(negedge clk);data=value;valid=1;timeout=0;
   @(posedge clk);while(!ready)begin
    @(posedge clk);timeout=timeout+1;if(timeout>500)$fatal(1,"Stream word timeout");
   end
   @(negedge clk);valid=0;
  end
 endtask
 task batch(input [31:0] epoch,input [31:0] seq);
  begin
   // Existing FrameStreamingCore routes: ST0->ST1, then ST0->ST0.
   word(32'h90000004);word(0);word(32'h60000000);
   word(32'h50000008);word(0);word(epoch);word(seq);
  end
 endtask
 task quiet(input integer cycles);
  integer i;
  begin for(i=0;i<cycles;i=i+1)begin
   @(negedge clk);if(ack_valid)$fatal(1,"Premature stream completion");
  end end
 endtask
 task result(input [31:0] epoch,input [31:0] seq,input cancelled);
  integer timeout;
  begin
   timeout=0;while(!ack_valid)begin
    @(negedge clk);timeout=timeout+1;if(timeout>100)$fatal(1,"Stream completion timeout");
   end
   repeat(8)begin
    if(!ack_valid||ack_epoch!==epoch||ack_sequence!==seq||ack_cancelled!==cancelled)
     $fatal(1,"Wrong stream identity");
    @(negedge clk);
   end
   ack_ready=1;@(negedge clk);ack_ready=0;
  end
 endtask
 initial begin
  reset();quiet(10);batch(1,10);result(1,10,0);
  $display("PASS response token: stale idle and real DMA echo identity");

  reset();writes=1;textures=1;batch(2,20);quiet(12);
  if(!busy)$fatal(1,"Echo did not reach mailbox while fence pending");
  writes=0;quiet(12);textures=0;result(2,20,0);
  $display("PASS response token: echo may precede fence retirement but not completion");

  reset();rx_enable=0;
  fork
   batch(3,30);
   begin quiet(35);rx_enable=1;end
  join
  result(3,30,0);
  $display("PASS response token: response backpressure preserves identity");

  reset();batch(4,40);while(!ack_valid)@(negedge clk);
  batch(5,50);repeat(10)@(negedge clk);
  if(!token_valid||token_epoch!==5||token_sequence!==50)
   $fatal(1,"Buffered next token unstable");
  result(4,40,0);result(5,50,0);
  $display("PASS response token: unread result retains the next complete identity");

  reset();writes=1;batch(6,60);quiet(10);cancel=1;
  @(negedge clk);cancel=0;quiet(10);writes=0;result(6,60,1);
  $display("PASS response token: cancellation does not bypass the fence");

  reset();word(32'h50000004);word(0);word(7);quiet(5);
  if(!protocol_fault||!fault||rx_ready)$fatal(1,"Short response not quarantined");
  $display("PASS response token: short identity rejected");

  reset();word(32'h5000000c);word(0);word(8);word(80);quiet(5);
  if(!protocol_fault||!fault||rx_ready)$fatal(1,"Long response not quarantined");
  $display("PASS response token: overlong identity rejected before extra data");

  reset();writes=1;batch(9,90);quiet(5);external_fault=1;writes=0;quiet(5);
  if(token_valid||rx_ready||!fault)$fatal(1,"Memory error failed to block response transport");
  $display("PASS response token: external memory fault quarantines pending work");

  reset();word(32'h50000008);word(0);word(10);quiet(8);
  if(token_valid)$fatal(1,"Partial identity became a token");
  external_fault=1;@(negedge clk);external_fault=0;quiet(5);
  if(!protocol_fault||rx_ready||token_valid)$fatal(1,"Partial identity fault not retained");
  $display("PASS response token: partial identity waits and retains a fault pulse");
  $finish;
 end
 initial begin #200000;$fatal(1,"Response token simulation deadline");end
endmodule
