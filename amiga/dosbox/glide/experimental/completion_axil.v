// Experimental 32-bit AXI-Lite interface, single clock, one transaction/channel.
// Offsets are local to this unassigned peripheral, not ZZ9000/XACP addresses.
module grx_completion_axil #(parameter ADDR_WIDTH=8) (
 input wire clk, input wire resetn,
 input wire token_valid, output wire token_ready,
 input wire [31:0] token_epoch, input wire [31:0] token_sequence,
 input wire drained, input wire bus_error,
 input wire [ADDR_WIDTH-1:0] s_awaddr, input wire s_awvalid, output wire s_awready,
 input wire [31:0] s_wdata, input wire [3:0] s_wstrb,
 input wire s_wvalid, output wire s_wready,
 output reg [1:0] s_bresp, output reg s_bvalid, input wire s_bready,
 input wire [ADDR_WIDTH-1:0] s_araddr, input wire s_arvalid, output wire s_arready,
 output reg [31:0] s_rdata, output reg [1:0] s_rresp,
 output reg s_rvalid, input wire s_rready
);
 localparam STATUS=8'h00, EPOCH=8'h04, SEQUENCE=8'h08;
 localparam EXPECT_EPOCH=8'h0c, EXPECT_SEQUENCE=8'h10, CONSUME=8'h14, CANCEL=8'h18;
 reg aw_held,w_held;
 reg [ADDR_WIDTH-1:0] write_address;
 reg [31:0] write_data,expected_epoch,expected_sequence;
 reg [3:0] write_strobe;
 reg [1:0] expected_valid;
 wire ack_valid,ack_cancelled,fault,busy;
 wire [31:0] ack_epoch,ack_sequence;
 wire commit = aw_held && w_held && !s_bvalid;
 wire identity_matches = (&expected_valid) && expected_epoch==ack_epoch && expected_sequence==ack_sequence;
 reg write_ok;
 always @* begin
  write_ok=0;
  if(write_strobe==4'hf) begin
   case(write_address)
    EXPECT_EPOCH,EXPECT_SEQUENCE:write_ok=1;
    CONSUME:write_ok=(write_data==1) && identity_matches && ack_valid;
    CANCEL:write_ok=(write_data==1) && identity_matches && busy && !ack_valid && !fault && !bus_error;
    default:write_ok=0;
   endcase
  end
 end
 wire consume = commit && write_ok && write_address==CONSUME;
 wire cancel = commit && write_ok && write_address==CANCEL;
 grx_completion mailbox(
  .clk(clk),.resetn(resetn),.token_valid(token_valid),.token_ready(token_ready),
  .token_epoch(token_epoch),.token_sequence(token_sequence),.drained(drained),
  .cancel(cancel),.bus_error(bus_error),.ack_valid(ack_valid),.ack_ready(consume),
  .ack_epoch(ack_epoch),.ack_sequence(ack_sequence),.ack_cancelled(ack_cancelled),
  .fault(fault),.busy(busy)
 );
 assign s_awready=!aw_held && !s_bvalid;
 assign s_wready=!w_held && !s_bvalid;
 assign s_arready=!s_rvalid;
 // Reads have no side effects. Capture the response at AR acceptance and keep
 // it stable under R backpressure, even if a later GPU error changes status.
 reg [31:0] read_data;
 reg [1:0] read_response;
 always @* begin
  read_data=0;read_response=0;
  case(s_araddr)
   8'h1c:read_data=32'h47525831; // GRX1 experimental endpoint identity
   STATUS:read_data={28'b0,(ack_valid && ack_cancelled),(fault || bus_error),busy,ack_valid};
   EPOCH:read_data=ack_valid?ack_epoch:0;
   SEQUENCE:read_data=ack_valid?ack_sequence:0;
   EXPECT_EPOCH:read_data=expected_epoch;
   EXPECT_SEQUENCE:read_data=expected_sequence;
   default:read_response=2'b10;
  endcase
 end
 always @(posedge clk) begin
  if(!resetn) begin
   aw_held<=0;w_held<=0;s_bvalid<=0;s_bresp<=0;
   s_rvalid<=0;s_rdata<=0;s_rresp<=0;
   write_address<=0;write_data<=0;write_strobe<=0;
   expected_epoch<=0;expected_sequence<=0;expected_valid<=0;
  end else begin
   if(s_awvalid && s_awready) begin write_address<=s_awaddr;aw_held<=1;end
   if(s_wvalid && s_wready) begin write_data<=s_wdata;write_strobe<=s_wstrb;w_held<=1;end
   if(commit) begin
    aw_held<=0;w_held<=0;s_bvalid<=1;s_bresp<=write_ok?2'b00:2'b10;
    if(write_ok && write_address==EXPECT_EPOCH) begin
     expected_epoch<=write_data;expected_valid[0]<=1;
    end
    if(write_ok && write_address==EXPECT_SEQUENCE) begin
     expected_sequence<=write_data;expected_valid[1]<=1;
    end
    // A fully formed control attempt uses up the staged identity, including
    // an identity mismatch or premature consume. Partial writes do nothing.
    if(write_strobe==4'hf && write_data==1 && (write_address==CONSUME || write_address==CANCEL))
     expected_valid<=0;
   end
   if(s_bvalid && s_bready)s_bvalid<=0;
   if(s_arvalid && s_arready) begin s_rdata<=read_data;s_rresp<=read_response;s_rvalid<=1;end
   if(s_rvalid && s_rready)s_rvalid<=0;
  end
 end
endmodule
