// Upstream parser + bypass Coalescer + ValueTrack; simulated AXI memory.
// Does not cover coalesced writes, texture caches or the complete GPU.
`timescale 1ns/1ps
module fence_axi_test;
reg clk=0,resetn=0,valid=0;
reg [31:0] data=0;
wire writes;
reg texture=0;
reg awvalid=0,wvalid=0,awready=0,wready=0,bvalid=0,bready=0;
wire ready,idle;
always #5 clk=~clk;
CommandParser dut(
 .aclk(clk),.resetn(resetn),.s_cmd_axis_tvalid(valid),.s_cmd_axis_tready(ready),
 .s_cmd_axis_tlast(1'b0),.s_cmd_axis_tdata(data),
 .m_cmd_fog_axis_tready(1'b1),.m_cmd_rasterizer_axis_tready(1'b1),
 .m_cmd_tmu0_axis_tready(1'b1),.m_cmd_tmu1_axis_tready(1'b1),.m_cmd_config_axis_tready(1'b1),
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
Coalescer #(.MAX_BEATS_TO_COALESCE(0)) memory_writes(
 .aclk(clk),
 .resetn(resetn),
 .s_mem_axi_awid('0),
 .s_mem_axi_awaddr(32'h1000),
 .s_mem_axi_awlen('0),
 .s_mem_axi_awsize(3'd2),
 .s_mem_axi_awburst(2'b01),
 .s_mem_axi_awlock('0),
 .s_mem_axi_awcache('0),
 .s_mem_axi_awprot('0),
 .s_mem_axi_awvalid(awvalid),
 .s_mem_axi_wdata(32'hcafe1234),
 .s_mem_axi_wstrb(4'b1111),
 .s_mem_axi_wlast(1'b1),
 .s_mem_axi_wvalid(wvalid),
 .s_mem_axi_bready(bready),
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
 .active(writes)
);
task scenario(input integer count, input integer order);
 integer i;
 begin
  @(negedge clk);resetn=0;valid=0;awvalid=0;wvalid=0;
  bvalid=0;bready=0;awready=0;wready=0;
  repeat(3)@(negedge clk);resetn=1;
  // Exercise independent address/data stalls, then leave B responses pending.
  for(i=0;i<count;i=i+1)begin
   awvalid=1;wvalid=1;
   repeat(3)begin
    @(negedge clk);
    if(writes!==1'b1)$fatal(1,"Lost pending stalled write");
   end
   awready=(order!=2);wready=(order!=1);
   @(negedge clk);
   if(awready)awvalid=0;
   if(wready)wvalid=0;
   repeat(3)@(negedge clk);
   awready=1;wready=1;
   @(negedge clk);awvalid=0;wvalid=0;awready=0;wready=0;
  end
  valid=1;data=32'h60000000;accepted();
  valid=1;data=0;
  blocked(); // Address/data accepted is not write completion.
  @(negedge clk);bvalid=1;bready=0;
  blocked(); // A response held under backpressure is not completion either.
  for(i=0;i<count;i=i+1)begin
   @(negedge clk);bready=1;
   @(negedge clk);bready=0;
   if(i+1<count)blocked(); // Partial retirement must not release the fence.
  end
  bvalid=0;
  accepted();
  repeat(4)@(negedge clk);
  if(writes!==1'b0||idle!==1'b1)$fatal(1,"Did not retire after final response");
  $display("PASS AXI fence: writes=%0d channel_order=%0d",count,order);
 end
endtask
initial begin
 scenario(1,0);scenario(3,1);scenario(8,2);
 scenario(32,0);scenario(128,1);
 $display("PASS fence waits for every accepted AXI write response");
 $finish;
end
initial begin #1000000;$fatal(1,"Simulation deadline");end
endmodule
