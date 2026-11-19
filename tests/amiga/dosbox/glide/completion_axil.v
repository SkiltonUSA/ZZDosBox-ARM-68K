// Single-clock AXI-Lite wrapper proposal. No hardware addresses or CDC.
`timescale 1ns/1ps
module completion_axil_test;
reg clk=0,resetn=0;
reg token_valid=0,drained=0,bus_error=0;
reg [31:0] epoch=1,seq=1;
wire token_ready;
reg [7:0] awaddr=0,araddr=0;
reg awvalid=0,wvalid=0,bready=0,arvalid=0,rready=0;
reg [31:0] wdata=0;
reg [3:0] wstrb=15;
wire awready,wready,bvalid,arready,rvalid;
wire [1:0] bresp,rresp;
wire [31:0] rdata;
always #5 clk=~clk;
reg address_seen=0,data_seen=0;
always @(posedge clk)begin
 if(!resetn)begin address_seen=0;data_seen=0;end
 else begin
  if(awvalid&&awready)address_seen=1;
  if(wvalid&&wready)data_seen=1;
  if(bvalid&&(!address_seen||!data_seen))$fatal(1,"Response before complete AXI write");
  if(bvalid&&bready)begin address_seen=0;data_seen=0;end
 end
end
grx_completion_axil dut(
 .clk(clk),.resetn(resetn),.token_valid(token_valid),.token_ready(token_ready),
 .token_epoch(epoch),.token_sequence(seq),.drained(drained),.bus_error(bus_error),
 .s_awaddr(awaddr),.s_awvalid(awvalid),.s_awready(awready),
 .s_wdata(wdata),.s_wstrb(wstrb),.s_wvalid(wvalid),.s_wready(wready),
 .s_bresp(bresp),.s_bvalid(bvalid),.s_bready(bready),
 .s_araddr(araddr),.s_arvalid(arvalid),.s_arready(arready),
 .s_rdata(rdata),.s_rresp(rresp),.s_rvalid(rvalid),.s_rready(rready)
);
task reset;
 begin
  @(negedge clk);resetn=0;token_valid=0;drained=0;bus_error=0;
  awvalid=0;wvalid=0;arvalid=0;bready=0;rready=0;
  repeat(3)@(negedge clk);resetn=1;
 end
endtask
task token(input reg[31:0] e,input reg[31:0] s);
 begin
  @(negedge clk);epoch=e;seq=s;token_valid=1;
  @(posedge clk);while(!token_ready)@(posedge clk);
  @(negedge clk);token_valid=0;
 end
endtask
task send_address(input reg[7:0] a,input integer delay_cycles);
 begin
  repeat(delay_cycles)@(negedge clk);
  @(negedge clk);awaddr=a;awvalid=1;
  @(posedge clk);while(!awready)@(posedge clk);
  @(negedge clk);awvalid=0;
 end
endtask
task send_data(input reg[31:0] d,input reg[3:0] strobe,input integer delay_cycles);
 begin
  repeat(delay_cycles)@(negedge clk);
  @(negedge clk);wdata=d;wstrb=strobe;wvalid=1;
  @(posedge clk);while(!wready)@(posedge clk);
  @(negedge clk);wvalid=0;
 end
endtask
task write_reg(input reg[7:0] a,input reg[31:0] d,input reg[3:0] strobe,
               input integer order,input reg[1:0] response);
 begin
  fork
   send_address(a,order==1?7:0);
   send_data(d,strobe,order==2?7:0);
  join
  while(!bvalid)@(negedge clk);
  repeat(8)begin
   if(!bvalid||bresp!==response)$fatal(1,"Wrong or unstable write response addr=%h got=%h want=%h",a,bresp,response);
   @(negedge clk);
  end
  bready=1;@(negedge clk);bready=0;
 end
endtask
task read_reg(input reg[7:0] a,input reg[31:0] expected,input reg[1:0] response);
 begin
  @(negedge clk);araddr=a;arvalid=1;
  @(posedge clk);while(!arready)@(posedge clk);
  @(negedge clk);arvalid=0;
  while(!rvalid)@(negedge clk);
  repeat(8)begin
   if(!rvalid||rdata!==expected||rresp!==response)$fatal(1,"Wrong or unstable read addr=%h got=%h want=%h",a,rdata,expected);
   @(negedge clk);
  end
  rready=1;@(negedge clk);rready=0;
 end
endtask
task identity(input reg[31:0] e,input reg[31:0] s);
 begin write_reg(8'h0c,e,15,1,0);write_reg(8'h10,s,15,2,0);end
endtask
task retire;
 begin @(negedge clk);drained=1;repeat(4)@(negedge clk);end
endtask
initial begin
 reset();read_reg(0,0,0);
 write_reg(8'h14,1,15,0,2);
 $display("PASS AXI mailbox: empty consume rejected");
 token(32'h10203040,32'h50607080);read_reg(0,2,0);
 identity(32'h10203040,32'h50607080);write_reg(8'h14,1,15,1,2);
 read_reg(0,2,0);retire();read_reg(0,3,0);
 read_reg(4,32'h10203040,0);read_reg(8,32'h50607080,0);
 $display("PASS AXI mailbox: pending work cannot be consumed");
 identity(32'h10203040,32'h50607081);write_reg(8'h14,1,15,2,2);
 read_reg(0,3,0);
 identity(32'h10203040,32'h50607080);write_reg(8'h14,1,15,1,0);
 read_reg(0,0,0);
 $display("PASS AXI mailbox: split AW/W and exact identity consume");
 token(2,2);repeat(4)@(negedge clk);read_reg(0,3,0);
 write_reg(8'h14,1,15,0,2); // Old staging was invalidated.
 write_reg(8'h0c,2,15,0,0);write_reg(8'h10,2,3,0,2);
 write_reg(8'h14,1,15,0,2);read_reg(0,3,0);
 identity(2,2);write_reg(8'h14,1,15,0,0);
 $display("PASS AXI mailbox: stale and partial identity writes rejected");
 token(3,2);repeat(4)@(negedge clk);
 write_reg(8'h0c,3,15,0,0); // Sequence matches old staging but was not freshly written.
 write_reg(8'h14,1,15,0,2);read_reg(0,3,0);
 identity(3,2);write_reg(8'h14,1,15,0,0);
 $display("PASS AXI mailbox: both identity fields must be freshly staged");
 @(negedge clk);drained=0;token(3,3);
 identity(3,3);write_reg(8'h18,1,15,0,0);read_reg(0,2,0);
 retire();read_reg(0,11,0);
 identity(3,3);write_reg(8'h14,1,15,0,0);read_reg(0,0,0);
 $display("PASS AXI mailbox: cancellation waits for drain");
 token(4,4);repeat(4)@(negedge clk);
 write_reg(0,1,15,0,2);write_reg(8'h0d,1,15,0,2);
 write_reg(8'h14,2,15,0,2);read_reg(8'hfc,0,2);read_reg(1,0,2);
 read_reg(0,3,0);identity(4,4);write_reg(8'h14,1,15,0,0);
 $display("PASS AXI mailbox: bad offsets and controls do not consume");
 token(5,5);repeat(4)@(negedge clk);identity(5,5);
 // Hold a previously captured read response while a new fault arrives.
 @(negedge clk);araddr=0;arvalid=1;
 @(posedge clk);while(!arready)@(posedge clk);
 @(negedge clk);arvalid=0;
 while(!rvalid)@(negedge clk);
 @(negedge clk);bus_error=1;
 repeat(6)begin
  @(negedge clk);if(!rvalid||rdata!==3||rresp!==0)$fatal(1,"AXI read snapshot changed while stalled");
 end
 bus_error=0;rready=1;@(negedge clk);rready=0;
 read_reg(0,6,0);write_reg(8'h14,1,15,0,2);
 read_reg(0,6,0);
 $display("PASS AXI mailbox: stale read snapshot cannot consume faulted work");
 reset();token(6,6);retire();identity(6,6);
 fork send_address(8'h14,0);send_data(1,15,0);join
 while(!bvalid)@(negedge clk);
 @(negedge clk);bus_error=1;
 repeat(6)begin
  @(negedge clk);if(!bvalid||bresp!==0)$fatal(1,"Committed write response changed while stalled");
 end
 bus_error=0;bready=1;@(negedge clk);bready=0;
 read_reg(0,6,0);
 $display("PASS AXI mailbox: post-commit fault reported by fresh status");
 $finish;
end
initial begin #500000;$fatal(1,"AXI mailbox simulation deadline");end
endmodule
