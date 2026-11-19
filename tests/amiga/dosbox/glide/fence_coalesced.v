// Upstream parser and complete Coalescer, with simulated AXI memory.
// Check burst addresses, data, strobes, WLAST and delayed response retirement.
`timescale 1ns/1ps
module coalesced_fence_test;
parameter DATA_WIDTH=32;
localparam BYTES=DATA_WIDTH/8;
reg clk=0,resetn=0,valid=0;
reg [31:0] data=0;
wire writes;
reg texture=0;
reg awvalid=0,wvalid=0,awready=0,wready=0,bvalid=0;
wire input_awready,input_wready,output_bready,output_awvalid,output_wvalid,output_wlast;
reg [31:0] input_addr=0;
reg [DATA_WIDTH-1:0] input_data=0;
wire [31:0] output_addr;
wire [7:0] output_len;
wire [DATA_WIDTH-1:0] output_data;
wire [BYTES-1:0] output_strb;
wire ready,idle;
wire config_valid;
wire [31:0] config_data;
wire [4:0] config_register;
always #5 clk=~clk;
CommandParser dut(
 .aclk(clk),.resetn(resetn),.s_cmd_axis_tvalid(valid),.s_cmd_axis_tready(ready),
 .s_cmd_axis_tlast(1'b0),.s_cmd_axis_tdata(data),
 .m_cmd_fog_axis_tready(1'b1),.m_cmd_rasterizer_axis_tready(1'b1),
 .m_cmd_tmu0_axis_tready(1'b1),.m_cmd_tmu1_axis_tready(1'b1),.m_cmd_config_axis_tready(1'b1),
 .m_cmd_config_axis_tvalid(config_valid),.m_cmd_xxx_axis_tdata(config_data),
 .m_cmd_xxx_axis_tuser(config_register),
 .rasterizerRunning(1'b0),.pixelInPipeline(1'b0),.dataInTriangleInterpolator(1'b0),
 .framebufferWritesPending(writes),.textureMemoryTransactionsPending(texture),.idle(idle),
 .colorBufferApplied(1'b1),.depthBufferApplied(1'b1),.stencilBufferApplied(1'b1),
 .framebufferClearApplied(1'b1)
);
task accepted;
 integer cycles;
 begin
  cycles=0;
  @(posedge clk);
  while(ready!==1'b1)begin
   cycles=cycles+1;if(cycles>50)$fatal(1,"Command acceptance timeout");
   @(posedge clk);
  end
  @(negedge clk);valid=0;
 end
endtask
task blocked;
 integer i;
 begin
  for(i=0;i<20;i=i+1)begin
   @(posedge clk);
   if(ready!==1'b0||idle!==1'b0)$fatal(1,"Passed fence with transactions pending");
  end
 end
endtask
Coalescer #(.DATA_WIDTH(DATA_WIDTH),.MAX_BEATS_TO_COALESCE(8)) memory_writes(
 .aclk(clk),
 .resetn(resetn),
 .s_mem_axi_awid('0),
 .s_mem_axi_awaddr(input_addr),
 .s_mem_axi_awlen('0),
 .s_mem_axi_awsize(3'($clog2(BYTES))),
 .s_mem_axi_awburst(2'b01),
 .s_mem_axi_awlock('0),
 .s_mem_axi_awcache('0),
 .s_mem_axi_awprot('0),
 .s_mem_axi_awvalid(awvalid),
 .s_mem_axi_wdata(input_data),
 .s_mem_axi_wstrb({BYTES{1'b1}}),
 .s_mem_axi_wlast(1'b1),
 .s_mem_axi_wvalid(wvalid),
 .s_mem_axi_bready(1'b1),
 .s_mem_axi_arid('0),
 .s_mem_axi_araddr('0),
 .s_mem_axi_arlen('0),
 .s_mem_axi_arsize('0),
 .s_mem_axi_arburst('0),
 .s_mem_axi_arlock('0),
 .s_mem_axi_arcache('0),
 .s_mem_axi_arprot('0),
 .s_mem_axi_arvalid('0),
 .s_mem_axi_rready('0),
 .m_mem_axi_awready(awready),
 .m_mem_axi_wready(wready),
 .m_mem_axi_bid('0),
 .m_mem_axi_bresp('0),
 .m_mem_axi_bvalid(bvalid),
 .m_mem_axi_arready('0),
 .m_mem_axi_rid('0),
 .m_mem_axi_rdata('0),
 .m_mem_axi_rresp('0),
 .m_mem_axi_rlast('0),
 .m_mem_axi_rvalid('0),
 .s_mem_axi_awready(input_awready),.s_mem_axi_wready(input_wready),
 .m_mem_axi_awvalid(output_awvalid),.m_mem_axi_awaddr(output_addr),.m_mem_axi_awlen(output_len),
 .m_mem_axi_wvalid(output_wvalid),.m_mem_axi_wdata(output_data),
 .m_mem_axi_wstrb(output_strb),.m_mem_axi_wlast(output_wlast),
 .m_mem_axi_bready(output_bready),.active(writes)
);

integer tick=0,aw_count=0,w_count=0,w_bursts=0,responses=0;
integer tokens=0;
reg [31:0] expected_token=0;
reg allow_responses=0,clear_scores=0;
reg [31:0] observed_addr[0:255];
integer observed_len[0:255];
reg [DATA_WIDTH-1:0] observed_data[0:255];
reg [BYTES-1:0] observed_strb[0:255];
reg observed_last[0:255];
// Independent AW/W channels may arrive in either order. Record both before
// matching them; return B only when the corresponding WLAST has arrived.
always @(posedge clk)begin
 if(!resetn||clear_scores)begin
  tick=0;aw_count=0;w_count=0;w_bursts=0;responses=0;tokens=0;
 end else begin
  tick=tick+1;
  if(output_awvalid&&awready)begin
   if(aw_count>=256)$fatal(1,"Address scoreboard overflow");
   observed_addr[aw_count]=output_addr;observed_len[aw_count]=output_len+1;
   aw_count=aw_count+1;
  end
  if(output_wvalid&&wready)begin
   if(w_count>=256)$fatal(1,"Data scoreboard overflow");
   observed_data[w_count]=output_data;observed_strb[w_count]=output_strb;
   observed_last[w_count]=output_wlast;w_count=w_count+1;
   if(output_wlast)w_bursts=w_bursts+1;
  end
  if(bvalid&&output_bready)responses=responses+1;
  if(config_valid)begin
   if(responses!=aw_count||w_count==0)$fatal(1,"Completion token overtook writes");
   if(config_register!==5'd1||config_data!==expected_token)$fatal(1,"Wrong completion token");
   tokens=tokens+1;
  end
 end
end
always @(negedge clk)begin
 if(!resetn)begin awready=0;wready=0;bvalid=0;end
 else begin
  // Different bounded stalls force both FIFO and address backpressure.
  awready=(tick%13>=5);wready=(tick%17>=7);
  if(bvalid&&output_bready)bvalid=0;
  else if(allow_responses&&responses<aw_count&&responses<w_bursts&&tick%11==0)bvalid=1;
 end
end
function [31:0] address_for(input integer index,input integer layout);
 begin
  case(layout)
   1:address_for=32'h1ff0+index*BYTES; // Cross a 4-KiB boundary.
   2:address_for=32'h1000+index*BYTES*3; // Non-contiguous writes.
   default:address_for=32'h1000+index*BYTES;
  endcase
 end
endfunction
task addresses(input integer count,input integer layout);
 integer i;
 begin
  for(i=0;i<count;i=i+1)begin
   @(negedge clk);input_addr=address_for(i,layout);awvalid=1;
   @(posedge clk);while(!input_awready)@(posedge clk);
   @(negedge clk);awvalid=0;
  end
 end
endtask
task pixels(input integer count);
 integer i;
 begin
  for(i=0;i<count;i=i+1)begin
   @(negedge clk);input_data=DATA_WIDTH'(64'h12345678cafe0000+i);wvalid=1;
   @(posedge clk);while(!input_wready)@(posedge clk);
   @(negedge clk);wvalid=0;
  end
 end
endtask
task scenario(input integer count,input integer layout,input integer cold);
 integer i,j,k,timeout;
 begin
  @(negedge clk);resetn=!cold;clear_scores=1;
  valid=0;awvalid=0;wvalid=0;allow_responses=0;
  repeat(3)@(negedge clk);resetn=1;clear_scores=0;
  fork addresses(count,layout);pixels(count);join
  expected_token=32'habcd0000+count+layout*256;
  valid=1;data=32'h60000000;accepted();
  valid=1;data=32'h10000001; // Ordered clear-colour register token, not a DDR marker.
  blocked();
  timeout=0;
  while(w_count<count||aw_count!=w_bursts)begin
   @(negedge clk);timeout=timeout+1;
   if(timeout>2000)$fatal(1,"Coalesced output drain timeout");
  end
  blocked(); // Even drained AW/W and FIFO must wait for B responses.
  k=0;
  for(i=0;i<aw_count;i=i+1)begin
   if(observed_len[i]<1||observed_len[i]>8)$fatal(1,"Invalid burst length");
   if((observed_addr[i]&4095)+observed_len[i]*BYTES>4096)$fatal(1,"Burst crossed 4-KiB boundary");
   for(j=0;j<observed_len[i];j=j+1)begin
    if(k>=count||observed_addr[i]+j*BYTES!==address_for(k,layout))$fatal(1,"Wrong burst address at pixel %0d",k);
    if(observed_data[k]!==DATA_WIDTH'(64'h12345678cafe0000+k))$fatal(1,"Corrupt data at pixel %0d",k);
    if(observed_strb[k]!=={BYTES{1'b1}})$fatal(1,"Corrupt strobes");
    if(observed_last[k]!== (j==observed_len[i]-1))$fatal(1,"Wrong WLAST");
    k=k+1;
   end
  end
  if(k!=count||w_count!=count)$fatal(1,"Lost or extra pixels");
  if(layout==0&&count>=8&&aw_count>=count)$fatal(1,"Test failed to exercise merging");
  @(negedge clk);allow_responses=1;
  while(responses<aw_count)begin
   @(posedge clk);
   if(ready!==1'b0||idle!==1'b0)$fatal(1,"Passed fence before all burst responses");
  end
  accepted();valid=1;data=expected_token;accepted();
  repeat(10)@(negedge clk);
  if(tokens!=1)$fatal(1,"Lost or duplicated completion token");
  if(writes!==1'b0||idle!==1'b1)$fatal(1,"Did not retire coalesced writes");
  $display("PASS coalesced fence: width=%0d pixels=%0d layout=%0d bursts=%0d cold=%0d",DATA_WIDTH,count,layout,aw_count,cold);
 end
endtask
initial begin
 scenario(1,0,1);scenario(7,0,1);scenario(8,0,1);scenario(17,0,1);
 scenario(64,0,1);scenario(17,1,1);scenario(17,2,1);
 scenario(64,0,0);scenario(7,1,0); // Keep hardware state across submissions.
 $finish;
end
initial begin #1000000;$fatal(1,"Simulation deadline");end
endmodule
