`timescale 1ns/1ps
// Exercises the generated M09 AXI converter, not a substitute CDC model.
module completion_cdc_test;
 glbl glbl();
 reg cpu_clk=0, gpu_clk=0, cpu_resetn=0, gpu_resetn=0;
 always #20 cpu_clk=~cpu_clk;
 initial begin #3; forever #5 gpu_clk=~gpu_clk; end
 reg idle=1, fault=0, valid=0, last=0;
 reg [31:0] data=0;
 wire ready;
 reg [31:0] c_awaddr=0;
 wire [31:0] g_awaddr;
 reg [2:0] c_awprot=0;
 wire [2:0] g_awprot;
 reg c_awvalid=0;
 wire g_awvalid;
 wire c_awready;
 wire g_awready;
 reg [31:0] c_wdata=0;
 wire [31:0] g_wdata;
 reg [3:0] c_wstrb=0;
 wire [3:0] g_wstrb;
 reg c_wvalid=0;
 wire g_wvalid;
 wire c_wready;
 wire g_wready;
 wire [1:0] c_bresp;
 wire [1:0] g_bresp;
 wire c_bvalid;
 wire g_bvalid;
 reg c_bready=0;
 wire g_bready;
 reg [31:0] c_araddr=0;
 wire [31:0] g_araddr;
 reg [2:0] c_arprot=0;
 wire [2:0] g_arprot;
 reg c_arvalid=0;
 wire g_arvalid;
 wire c_arready;
 wire g_arready;
 wire [31:0] c_rdata;
 wire [31:0] g_rdata;
 wire [1:0] c_rresp;
 wire [1:0] g_rresp;
 wire c_rvalid;
 wire g_rvalid;
 reg c_rready=0;
 wire g_rready;
 zz9000_ps_ps7_0_axi_periph_imp_auto_cc_1 crossing(
  .s_axi_aclk(cpu_clk),
  .s_axi_aresetn(cpu_resetn),
  .m_axi_aclk(gpu_clk),
  .m_axi_aresetn(gpu_resetn),
  .s_axi_awaddr(c_awaddr),
  .s_axi_awprot(c_awprot),
  .s_axi_awvalid(c_awvalid),
  .s_axi_awready(c_awready),
  .s_axi_wdata(c_wdata),
  .s_axi_wstrb(c_wstrb),
  .s_axi_wvalid(c_wvalid),
  .s_axi_wready(c_wready),
  .s_axi_bresp(c_bresp),
  .s_axi_bvalid(c_bvalid),
  .s_axi_bready(c_bready),
  .s_axi_araddr(c_araddr),
  .s_axi_arprot(c_arprot),
  .s_axi_arvalid(c_arvalid),
  .s_axi_arready(c_arready),
  .s_axi_rdata(c_rdata),
  .s_axi_rresp(c_rresp),
  .s_axi_rvalid(c_rvalid),
  .s_axi_rready(c_rready),
  .m_axi_awaddr(g_awaddr),
  .m_axi_awprot(g_awprot),
  .m_axi_awvalid(g_awvalid),
  .m_axi_awready(g_awready),
  .m_axi_wdata(g_wdata),
  .m_axi_wstrb(g_wstrb),
  .m_axi_wvalid(g_wvalid),
  .m_axi_wready(g_wready),
  .m_axi_bresp(g_bresp),
  .m_axi_bvalid(g_bvalid),
  .m_axi_bready(g_bready),
  .m_axi_araddr(g_araddr),
  .m_axi_arprot(g_arprot),
  .m_axi_arvalid(g_arvalid),
  .m_axi_arready(g_arready),
  .m_axi_rdata(g_rdata),
  .m_axi_rresp(g_rresp),
  .m_axi_rvalid(g_rvalid),
  .m_axi_rready(g_rready)
 );
 grx_completion_endpoint endpoint(
  .aclk(gpu_clk),
  .aresetn(gpu_resetn),
  .gpu_idle(idle),
  .gpu_fault(fault),
  .s_resp_axis_tvalid(valid),
  .s_resp_axis_tready(ready),
  .s_resp_axis_tdata(data),
  .s_resp_axis_tlast(last),
  .s_axi_awaddr(g_awaddr),
  .s_axi_awprot(g_awprot),
  .s_axi_awvalid(g_awvalid),
  .s_axi_awready(g_awready),
  .s_axi_wdata(g_wdata),
  .s_axi_wstrb(g_wstrb),
  .s_axi_wvalid(g_wvalid),
  .s_axi_wready(g_wready),
  .s_axi_bresp(g_bresp),
  .s_axi_bvalid(g_bvalid),
  .s_axi_bready(g_bready),
  .s_axi_araddr(g_araddr),
  .s_axi_arprot(g_arprot),
  .s_axi_arvalid(g_arvalid),
  .s_axi_arready(g_arready),
  .s_axi_rdata(g_rdata),
  .s_axi_rresp(g_rresp),
  .s_axi_rvalid(g_rvalid),
  .s_axi_rready(g_rready)
 );
 task token(input [31:0] epoch, seq);
 begin
  @(negedge gpu_clk); data=epoch;valid=1;last=0;
  do @(posedge gpu_clk); while(!ready);
  @(negedge gpu_clk); data=seq;last=1;
  do @(posedge gpu_clk); while(!ready);
  @(negedge gpu_clk);valid=0;last=0;
 end endtask
 task read_reg(input [31:0] offset, expected, input [1:0] response);
 reg [31:0] snapshot;
 begin
  @(negedge cpu_clk);c_araddr=32'h83c80000+offset;c_arvalid=1;
  do @(posedge cpu_clk); while(!c_arready);
  @(negedge cpu_clk);c_arvalid=0;
  wait(c_rvalid);snapshot=c_rdata;
  repeat(7) begin
   @(negedge cpu_clk);
   if(!c_rvalid || c_rdata!==expected || c_rresp!==response || c_rdata!==snapshot)
    $fatal(1,"CDC read offset=%h got=%h resp=%h expected=%h",offset,c_rdata,c_rresp,expected);
  end
  c_rready=1;@(negedge cpu_clk);c_rready=0;
 end endtask
 task write_reg(input [31:0] offset, value, input bit data_first, input [1:0] response);
 begin
  @(negedge cpu_clk);c_awaddr=32'h83c80000+offset;c_wdata=value;c_wstrb=15;
  if(data_first) begin
   c_wvalid=1;do @(posedge cpu_clk);while(!c_wready);
   @(negedge cpu_clk);c_wvalid=0;repeat(3) @(negedge cpu_clk);
   c_awvalid=1;do @(posedge cpu_clk);while(!c_awready);
   @(negedge cpu_clk);c_awvalid=0;
  end else begin
   c_awvalid=1;do @(posedge cpu_clk);while(!c_awready);
   @(negedge cpu_clk);c_awvalid=0;repeat(3) @(negedge cpu_clk);
   c_wvalid=1;do @(posedge cpu_clk);while(!c_wready);
   @(negedge cpu_clk);c_wvalid=0;
  end
  wait(c_bvalid);
  repeat(5) begin
   @(negedge cpu_clk);
   if(!c_bvalid || c_bresp!==response) $fatal(1,"CDC write response offset=%h",offset);
  end
  c_bready=1;@(negedge cpu_clk);c_bready=0;
 end endtask
 task stage_identity(input [31:0] epoch, seq);
 begin write_reg('hc,epoch,0,0);write_reg('h10,seq,1,0);end endtask
 initial begin
  repeat(10) @(negedge gpu_clk);gpu_resetn=1;
  repeat(10) @(negedge cpu_clk);cpu_resetn=1;
  read_reg('h1c,32'h47525831,0);read_reg(0,0,0);
  read_reg('h1001c,0,2);
  $display("PASS CDC: identity, empty status and no high-address alias");
  idle=0;token(17,23);
  read_reg(0,2,0);stage_identity(17,23);write_reg('h14,1,1,2);
  $display("PASS CDC: pending work cannot be consumed");
  @(negedge gpu_clk);idle=1;
  read_reg(0,3,0);read_reg(4,17,0);read_reg(8,23,0);
  stage_identity(17,24);write_reg('h14,1,0,2);read_reg(0,3,0);
  stage_identity(17,23);write_reg('h14,1,1,0);read_reg(0,0,0);
  $display("PASS CDC: identity match, split channels and backpressure");
  @(negedge gpu_clk);idle=0;token(18,1);
  stage_identity(18,1);write_reg('h18,1,0,0);read_reg(0,2,0);
  @(negedge gpu_clk);idle=1;
  read_reg(0,11,0);stage_identity(18,1);write_reg('h14,1,1,0);
  read_reg(0,0,0);
  $display("PASS CDC: cancellation retains work until drain");
  token(19,1);read_reg(0,3,0);
  @(negedge gpu_clk);fault=1;
  read_reg(0,6,0);stage_identity(19,1);write_reg('h14,1,0,2);
  @(negedge gpu_clk);fault=0;read_reg(0,6,0);
  $display("PASS CDC: fault quarantines result and remains sticky");
  @(negedge cpu_clk);cpu_resetn=0;
  @(negedge gpu_clk);gpu_resetn=0;
  repeat(10) @(negedge cpu_clk);cpu_resetn=1;
  repeat(10) @(negedge gpu_clk);gpu_resetn=1;
  read_reg(0,0,0);read_reg('h1c,32'h47525831,0);
  $display("PASS CDC: clean restart with reversed reset release");
  $finish;
 end
 initial begin #2000000;$fatal(1,"CDC test timeout");end
endmodule
