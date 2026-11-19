// Passive GPU-side AXI response observer. No transaction or drain tracking.
// Connect at the GPU's m_axi interface in its clock/reset domain.
module grx_axi_response_fault (
 input wire clk,
 input wire resetn,
 input wire bvalid,
 input wire [1:0] bresp,
 input wire rvalid,
 input wire [1:0] rresp,
 output wire fault
);
 reg latched_fault;
 // SLVERR=2, DECERR=3. OKAY=0 and EXOKAY=1 are not bus errors.
 // VALID already makes RESP meaningful, even if READY is low. Read errors
 // count on any beat, not only RLAST. No ready/last gating is required.
 wire response_error = (bvalid && bresp[1]) || (rvalid && rresp[1]);
 // Combinational visibility closes the same-edge acknowledgement race.
 assign fault = resetn && (latched_fault || response_error);
 always @(posedge clk) begin
  if (!resetn) latched_fault <= 0;
  else if (response_error) latched_fault <= 1;
 end
endmodule
