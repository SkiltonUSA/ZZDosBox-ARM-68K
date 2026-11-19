// Proposal only: token delivery through the upstream parser, synthetic drains.
`timescale 1ns/1ps
module completion_test;
reg clk=0,resetn=0,valid=0;
reg [31:0] data=0,epoch=1;
reg writes=0,texture=0,dma_idle=1,cancel=0,bus_error=0,ack_ready=0;
wire ready,parser_idle,token_valid,token_ready,ack_valid,ack_cancelled,fault,busy;
wire [31:0] token_data,ack_epoch,ack_sequence;
wire [4:0] config_register;
always #5 clk=~clk;
CommandParser parser(
 .aclk(clk),.resetn(resetn),.s_cmd_axis_tvalid(valid),.s_cmd_axis_tready(ready),
 .s_cmd_axis_tlast(1'b0),.s_cmd_axis_tdata(data),
 .m_cmd_fog_axis_tready(1'b1),.m_cmd_rasterizer_axis_tready(1'b1),
 .m_cmd_tmu0_axis_tready(1'b1),.m_cmd_tmu1_axis_tready(1'b1),
 .m_cmd_config_axis_tready(token_ready),.m_cmd_config_axis_tvalid(token_valid),
 .m_cmd_xxx_axis_tdata(token_data),.m_cmd_xxx_axis_tuser(config_register),
 .rasterizerRunning(1'b0),.pixelInPipeline(1'b0),.dataInTriangleInterpolator(1'b0),
 .framebufferWritesPending(writes),.textureMemoryTransactionsPending(texture),.idle(parser_idle),
 .colorBufferApplied(1'b1),.depthBufferApplied(1'b1),.stencilBufferApplied(1'b1),
 .framebufferClearApplied(1'b1)
);
// A clear-colour register value is only a test carrier. It is not a new opcode
// or a proposal to commandeer this register in a production application.
grx_completion completion(
 .clk(clk),.resetn(resetn),.token_valid(token_valid),.token_ready(token_ready),
 .token_epoch(epoch),.token_sequence(token_data),
 .drained(parser_idle&&dma_idle&&!writes&&!texture),
 .cancel(cancel),.bus_error(bus_error),.ack_valid(ack_valid),.ack_ready(ack_ready),
 .ack_epoch(ack_epoch),.ack_sequence(ack_sequence),.ack_cancelled(ack_cancelled),
 .fault(fault),.busy(busy)
);
task reset;
 begin
  @(negedge clk);resetn=0;valid=0;writes=0;texture=0;dma_idle=1;
  cancel=0;bus_error=0;ack_ready=0;
  repeat(3)@(negedge clk);resetn=1;
  repeat(8)@(negedge clk);
 end
endtask
task quiet(input integer cycles);
 integer i;
 begin for(i=0;i<cycles;i=i+1)begin
  @(negedge clk);if(ack_valid!==1'b0)$fatal(1,"Premature acknowledgement");
 end end
endtask
task word(input reg[31:0] value);
 integer timeout;
 begin
  @(negedge clk);data=value;valid=1;timeout=0;
  @(posedge clk);while(ready!==1'b1)begin
   @(posedge clk);timeout=timeout+1;if(timeout>500)$fatal(1,"Command timeout");
  end
  @(negedge clk);valid=0;
 end
endtask
task batch(input reg[31:0] seq);
 begin word(32'h60000000);word(32'h10000001);word(seq);end
endtask
task expect_ack(input reg[31:0] expected_epoch,input reg[31:0] expected_sequence,input reg cancelled);
 integer i;
 begin
  i=0;while(ack_valid!==1'b1)begin
   @(negedge clk);i=i+1;if(i>100)$fatal(1,"Acknowledgement timeout");
  end
  // Results must remain stable until explicitly consumed.
  repeat(12)begin
   if(ack_epoch!==expected_epoch||ack_sequence!==expected_sequence||ack_cancelled!==cancelled)
    $fatal(1,"Wrong or unstable acknowledgement");
   if(ack_valid!==1'b1||token_ready!==1'b0||busy!==1'b1)$fatal(1,"Lost retained result");
   @(negedge clk);
  end
  ack_ready=1;@(negedge clk);ack_ready=0;
  if(ack_valid!==1'b0||busy!==1'b0)$fatal(1,"Result did not clear on consume");
 end
endtask
initial begin
 reset();quiet(20);
 if(parser_idle!==1'b1||busy!==1'b0)$fatal(1,"Initial idle test invalid");
 $display("PASS completion: stale idle cannot acknowledge a batch");
 // Fence blocks the carrier until both retirement domains have drained.
 @(negedge clk);writes=1;texture=1;
 fork
  batch(32'h11);
  begin quiet(20);writes=0;quiet(20);texture=0;end
 join
 expect_ack(1,32'h11,0);
 $display("PASS completion: ordered token waits for writes and textures");
 // Parser token arrives but command-DMA activity still forbids retirement.
 @(negedge clk);dma_idle=0;
 batch(32'h12);quiet(20);
 if(busy!==1'b1)$fatal(1,"Missing pending batch");
 @(negedge clk);cancel=1;@(negedge clk);cancel=0;
 quiet(20); // Cancellation never releases ownership by itself.
 if(busy!==1'b1)$fatal(1,"Cancellation released pending work");
 @(negedge clk);dma_idle=1;
 expect_ack(1,32'h12,1);
 $display("PASS completion: cancellation waits for retirement");
 // Reusing a sequence in a new epoch cannot match an old session result.
 @(negedge clk);epoch=2;
 batch(32'h11);expect_ack(2,32'h11,0);
 $display("PASS completion: epoch and sequence retained together");
 batch(32'h21);
 while(!ack_valid)@(negedge clk);
 fork
  batch(32'h22);
  expect_ack(2,32'h21,0);
 join
 expect_ack(2,32'h22,0);
 $display("PASS completion: unread result backpressures the next token");
 @(negedge clk);dma_idle=0;
 batch(32'h23);quiet(8);
 @(negedge clk);cancel=1;dma_idle=1;
 @(negedge clk);cancel=0;
 expect_ack(2,32'h23,1);
 $display("PASS completion: cancellation on the drain cycle is retained");
 // Error plus an apparent drain must never become a successful result.
 @(negedge clk);dma_idle=0;
 batch(32'h13);quiet(8);
 @(negedge clk);bus_error=1;dma_idle=1;
 quiet(2);bus_error=0;quiet(10);
 if(!fault||!busy||token_ready)$fatal(1,"Fault did not quarantine work");
 @(negedge clk);cancel=1;ack_ready=1;quiet(5);cancel=0;ack_ready=0;
 if(!fault||!busy)$fatal(1,"Software controls cleared quarantine");
 $display("PASS completion: error overrides drain and stays quarantined");
 // Reset models an already-quiesced hardware reset, not a recovery command.
 reset();epoch=3;batch(32'h14);
 while(!ack_valid)@(negedge clk);
 @(negedge clk);bus_error=1;ack_ready=1;
 #1;if(ack_valid)$fatal(1,"Error allowed stale result consumption");
 @(negedge clk);bus_error=0;ack_ready=0;quiet(8);
 if(!fault||!busy)$fatal(1,"Late error lost quarantine");
 $display("PASS completion: error suppresses an unconsumed result");
 reset();epoch=4;batch(32'h15);expect_ack(4,32'h15,0);
 $display("PASS completion: quiesced reset starts a fresh session");
 $finish;
end
initial begin #100000;$fatal(1,"Completion simulation deadline");end
endmodule
