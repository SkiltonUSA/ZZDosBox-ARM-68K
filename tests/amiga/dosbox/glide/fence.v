// Diagnostic against upstream CommandParser. No installed bitstream claim.
`timescale 1ns/1ps
module fence_test;
reg clk=0,resetn=0,valid=0;
reg [31:0] data=0;
reg writes=0,texture=0;
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
task scenario(input reg fb,input reg tex);
 begin
  @(negedge clk);resetn=0;valid=0;writes=fb;texture=tex;
  repeat(3)@(negedge clk);
  resetn=1;valid=1;data=32'h60000000;
  accepted(); // Fence word itself can be accepted before memory retires.
  valid=1;data=0; // The following NOP must remain blocked.
  if(fb||tex)blocked();
  if(fb&&tex)begin
   @(negedge clk);writes=0;
   blocked(); // Texture traffic alone must still hold the fence.
  end
  @(negedge clk);writes=0;texture=0;
  accepted();
  repeat(4)@(negedge clk);
  if(idle!==1'b1)$fatal(1,"Did not return to idle after retirement");
  $display("PASS parser fence: framebuffer=%0d texture=%0d",fb,tex);
 end
endtask
initial begin
 scenario(1,0);scenario(0,1);scenario(1,1);scenario(0,0);
 $display("PASS fence acceptance is not retirement; following command waits for both domains");
 $finish;
end
initial begin #100000;$fatal(1,"Simulation deadline");end
endmodule
