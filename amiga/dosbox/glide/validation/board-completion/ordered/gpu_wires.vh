wire [5:0] awid;
wire [31:0] awaddr;
wire [7:0] awlen;
wire [2:0] awsize;
wire [1:0] awburst;
wire [0:0] awlock;
wire [3:0] awcache;
wire [2:0] awprot;
wire [0:0] awvalid;
wire [0:0] awready;
wire [63:0] wdata;
wire [7:0] wstrb;
wire [0:0] wlast;
wire [0:0] wvalid;
wire [0:0] wready;
wire [5:0] bid;
wire [1:0] bresp;
wire [0:0] bvalid;
wire [0:0] bready;
wire [5:0] arid;
wire [31:0] araddr;
wire [7:0] arlen;
wire [2:0] arsize;
wire [1:0] arburst;
wire [0:0] arlock;
wire [3:0] arcache;
wire [2:0] arprot;
wire [0:0] arvalid;
wire [0:0] arready;
wire [5:0] rid;
wire [63:0] rdata;
wire [1:0] rresp;
wire [0:0] rlast;
wire [0:0] rvalid;
wire [0:0] rready;
wire ram_bvalid,ram_bready,ram_rvalid,ram_rready;wire [1:0] ram_bresp,ram_rresp;
assign bvalid=ram_bvalid && b_allow;assign ram_bready=bready && b_allow;
assign rvalid=ram_rvalid && r_allow;assign ram_rready=rready && r_allow;
RasterIX_EF #(.DATA_WIDTH(64),.ID_WIDTH(6),.TMU_COUNT(1),
 .RASTERIZER_FIXPOINT_PRECISION(18),.RASTERIZER_FLOAT_PRECISION(24)) gpu(
.aclk(clk),
.resetn(resetn),
.s_cmd_axis_tvalid(cmd_valid),
.s_cmd_axis_tready(cmd_ready),
.s_cmd_axis_tlast(cmd_last),
.s_cmd_axis_tdata(cmd_data),
.m_cmd_resp_axis_tvalid(resp_valid),
.m_cmd_resp_axis_tready(resp_ready),
.m_cmd_resp_axis_tlast(resp_last),
.m_cmd_resp_axis_tdata(resp_data),
.fb_swapped(1'b1),
.idle(idle),
.m_axi_awid(awid),
.m_axi_awaddr(awaddr),
.m_axi_awlen(awlen),
.m_axi_awsize(awsize),
.m_axi_awburst(awburst),
.m_axi_awlock(awlock),
.m_axi_awcache(awcache),
.m_axi_awprot(awprot),
.m_axi_awvalid(awvalid),
.m_axi_awready(awready),
.m_axi_wdata(wdata),
.m_axi_wstrb(wstrb),
.m_axi_wlast(wlast),
.m_axi_wvalid(wvalid),
.m_axi_wready(wready),
.m_axi_bid(bid),
.m_axi_bresp(observed_bresp),
.m_axi_bvalid(bvalid),
.m_axi_bready(bready),
.m_axi_arid(arid),
.m_axi_araddr(araddr),
.m_axi_arlen(arlen),
.m_axi_arsize(arsize),
.m_axi_arburst(arburst),
.m_axi_arlock(arlock),
.m_axi_arcache(arcache),
.m_axi_arprot(arprot),
.m_axi_arvalid(arvalid),
.m_axi_arready(arready),
.m_axi_rid(rid),
.m_axi_rdata(rdata),
.m_axi_rresp(observed_rresp),
.m_axi_rlast(rlast),
.m_axi_rvalid(rvalid),
.m_axi_rready(rready));
axi_ram #(.DATA_WIDTH(64),.ID_WIDTH(6),.ADDR_WIDTH(18)) ram(.clk(clk),.rst(!resetn),
.s_axi_awid(awid),
.s_axi_awaddr(awaddr[17:0]),
.s_axi_awlen(awlen),
.s_axi_awsize(awsize),
.s_axi_awburst(awburst),
.s_axi_awlock(awlock),
.s_axi_awcache(awcache),
.s_axi_awprot(awprot),
.s_axi_awvalid(awvalid),
.s_axi_awready(awready),
.s_axi_wdata(wdata),
.s_axi_wstrb(wstrb),
.s_axi_wlast(wlast),
.s_axi_wvalid(wvalid),
.s_axi_wready(wready),
.s_axi_bid(bid),
.s_axi_bresp(ram_bresp),
.s_axi_bvalid(ram_bvalid),
.s_axi_bready(ram_bready),
.s_axi_arid(arid),
.s_axi_araddr(araddr[17:0]),
.s_axi_arlen(arlen),
.s_axi_arsize(arsize),
.s_axi_arburst(arburst),
.s_axi_arlock(arlock),
.s_axi_arcache(arcache),
.s_axi_arprot(arprot),
.s_axi_arvalid(arvalid),
.s_axi_arready(arready),
.s_axi_rid(rid),
.s_axi_rdata(rdata),
.s_axi_rresp(ram_rresp),
.s_axi_rlast(rlast),
.s_axi_rvalid(ram_rvalid),
.s_axi_rready(ram_rready));
