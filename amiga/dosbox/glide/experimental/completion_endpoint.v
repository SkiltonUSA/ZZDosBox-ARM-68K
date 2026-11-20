// Experimental board endpoint. Candidate address is not a released XACP ABI.
// All logic and GPU response signals use the 100 MHz GPU clock domain.
module grx_completion_endpoint #(parameter [31:0] BASE_ADDR=32'h83c80000) (
 (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 ACLK CLK", X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ACLK, ASSOCIATED_BUSIF S_AXI:S_RESP_AXIS, ASSOCIATED_RESET ARESETN, FREQ_HZ 100000000" *)
 input wire aclk,
 (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 ARESETN RST", X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
 input wire aresetn,
 input wire gpu_idle,
 input wire gpu_fault,
 (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_RESP_AXIS TVALID" *)
 input wire s_resp_axis_tvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_RESP_AXIS TREADY" *)
 output wire s_resp_axis_tready,
 (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_RESP_AXIS TDATA" *)
 input wire [31:0] s_resp_axis_tdata,
 (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_RESP_AXIS TLAST" *)
 input wire s_resp_axis_tlast,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWADDR" *)
 (* X_INTERFACE_PARAMETER = "PROTOCOL AXI4LITE, DATA_WIDTH 32, ADDR_WIDTH 32, FREQ_HZ 100000000" *)
 input wire [31:0] s_axi_awaddr,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWPROT" *)
 input wire [2:0] s_axi_awprot,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWVALID" *)
 input wire s_axi_awvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREADY" *)
 output wire s_axi_awready,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WDATA" *)
 input wire [31:0] s_axi_wdata,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WSTRB" *)
 input wire [3:0] s_axi_wstrb,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WVALID" *)
 input wire s_axi_wvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WREADY" *)
 output wire s_axi_wready,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BRESP" *)
 output wire [1:0] s_axi_bresp,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BVALID" *)
 output wire s_axi_bvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BREADY" *)
 input wire s_axi_bready,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARADDR" *)
 input wire [31:0] s_axi_araddr,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARPROT" *)
 input wire [2:0] s_axi_arprot,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARVALID" *)
 input wire s_axi_arvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARREADY" *)
 output wire s_axi_arready,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RDATA" *)
 output wire [31:0] s_axi_rdata,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RRESP" *)
 output wire [1:0] s_axi_rresp,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RVALID" *)
 output wire s_axi_rvalid,
 (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RREADY" *)
 input wire s_axi_rready
 );
 wire token_valid,token_ready,token_fault;
 wire [31:0] token_epoch,token_sequence;
 grx_response_token receiver(
  .clk(aclk),.resetn(aresetn),.s_valid(s_resp_axis_tvalid),.s_ready(s_resp_axis_tready),
  .s_data(s_resp_axis_tdata),.s_last(s_resp_axis_tlast),.external_fault(gpu_fault),
  .token_valid(token_valid),.token_ready(token_ready),.token_epoch(token_epoch),
  .token_sequence(token_sequence),.fault(token_fault)
 );
 // Decode all address bits. High offsets and other banks must never alias.
 grx_completion_axil #(.ADDR_WIDTH(32)) registers(
  .clk(aclk),.resetn(aresetn),.token_valid(token_valid),.token_ready(token_ready),
  .token_epoch(token_epoch),.token_sequence(token_sequence),.drained(gpu_idle),
  .bus_error(gpu_fault | token_fault),
  .s_awaddr(s_axi_awaddr ^ BASE_ADDR),
  .s_awvalid(s_axi_awvalid),
  .s_awready(s_axi_awready),
  .s_wdata(s_axi_wdata),
  .s_wstrb(s_axi_wstrb),
  .s_wvalid(s_axi_wvalid),
  .s_wready(s_axi_wready),
  .s_bresp(s_axi_bresp),
  .s_bvalid(s_axi_bvalid),
  .s_bready(s_axi_bready),
  .s_araddr(s_axi_araddr ^ BASE_ADDR),
  .s_arvalid(s_axi_arvalid),
  .s_arready(s_axi_arready),
  .s_rdata(s_axi_rdata),
  .s_rresp(s_axi_rresp),
  .s_rvalid(s_axi_rvalid),
  .s_rready(s_axi_rready)
 );
endmodule
