// Experimental completion mailbox. No XACP opcode, register address or CDC.
// See completion.md for the required integration contract.
module grx_completion (
 input wire clk,
 input wire resetn,
 input wire token_valid,
 output wire token_ready,
 input wire [31:0] token_epoch,
 input wire [31:0] token_sequence,
 input wire drained,
 input wire cancel,
 input wire bus_error,
 output wire ack_valid,
 input wire ack_ready,
 output reg [31:0] ack_epoch,
 output reg [31:0] ack_sequence,
 output reg ack_cancelled,
 output reg fault,
 output wire busy
);
 reg pending;
 reg result_valid;
 assign token_ready = !pending && !result_valid && !fault && !bus_error;
 assign ack_valid = result_valid && !fault && !bus_error;
 assign busy = pending || result_valid || fault || bus_error;
 always @(posedge clk) begin
  if (!resetn) begin
   pending <= 0;
   result_valid <= 0;
   fault <= 0;
   ack_epoch <= 0;
   ack_sequence <= 0;
   ack_cancelled <= 0;
  end else if (bus_error) begin
   // A timeout/cancel/ack cannot establish quiescence after a bus failure.
   fault <= 1;
   result_valid <= 0;
  end else if (!fault) begin
   if (token_valid && token_ready) begin
    ack_epoch <= token_epoch;
    ack_sequence <= token_sequence;
    ack_cancelled <= cancel;
    pending <= 1;
   end
   if (pending) begin
    if (cancel) ack_cancelled <= 1;
    if (drained) begin
     pending <= 0;
     result_valid <= 1;
    end
   end
   if (ack_valid && ack_ready) result_valid <= 0;
  end
 end
endmodule
