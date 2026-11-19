// Experimental receiver for a dedicated two-word DMA response stream.
// Epoch then sequence, TLAST only on sequence. Not an assigned XACP ABI.
module grx_response_token (
 input wire clk, input wire resetn,
 input wire s_valid, output wire s_ready,
 input wire [31:0] s_data, input wire s_last,
 input wire external_fault,
 output wire token_valid, input wire token_ready,
 output reg [31:0] token_epoch, output reg [31:0] token_sequence,
 output wire fault
);
 localparam EPOCH=0, SEQUENCE=1, TOKEN=2;
 reg [1:0] state;
 reg latched_fault;
 wire malformed = s_valid && s_ready &&
                  ((state==EPOCH && s_last) || (state==SEQUENCE && !s_last));
 assign s_ready = resetn && !latched_fault && !external_fault && state!=TOKEN;
 assign fault = resetn && (latched_fault || external_fault || malformed);
 assign token_valid = resetn && state==TOKEN && !fault;
 always @(posedge clk) begin
  if(!resetn)begin
   state<=EPOCH;latched_fault<=0;token_epoch<=0;token_sequence<=0;
  end else if(external_fault || malformed)begin
   latched_fault<=1;
  end else if(!latched_fault)begin
   case(state)
    EPOCH:if(s_valid && s_ready)begin token_epoch<=s_data;state<=SEQUENCE;end
    SEQUENCE:if(s_valid && s_ready)begin token_sequence<=s_data;state<=TOKEN;end
    TOKEN:if(token_valid && token_ready)state<=EPOCH;
   endcase
  end
 end
endmodule
