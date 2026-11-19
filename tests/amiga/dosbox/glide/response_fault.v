// GPU response observer connected to the completion mailbox; synthetic AXI.
`timescale 1ns/1ps
module response_fault_test;
 reg clk=0,resetn=0;
 reg bvalid=0,rvalid=0;
 reg [1:0] bresp=0,rresp=0;
 reg token_valid=0,drained=0,cancel=0,ack_ready=0;
 wire bus_fault,token_ready,ack_valid,fault,busy;
 integer channel,code;
 always #5 clk=~clk;
 grx_axi_response_fault observer(
  .clk(clk),.resetn(resetn),.bvalid(bvalid),.bresp(bresp),
  .rvalid(rvalid),.rresp(rresp),.fault(bus_fault)
 );
 grx_completion mailbox(
  .clk(clk),.resetn(resetn),.token_valid(token_valid),.token_ready(token_ready),
  .token_epoch(32'h1234),.token_sequence(32'h5678),.drained(drained),
  .cancel(cancel),.bus_error(bus_fault),.ack_valid(ack_valid),
  .ack_ready(ack_ready),.ack_epoch(),.ack_sequence(),.ack_cancelled(),
  .fault(fault),.busy(busy)
 );
 task reset;
  begin
   @(negedge clk);resetn=0;bvalid=0;rvalid=0;bresp=0;rresp=0;
   token_valid=0;drained=0;cancel=0;ack_ready=0;
   repeat(3)@(negedge clk);resetn=1;
   @(negedge clk);
   if(bus_fault||fault||busy||!token_ready)$fatal(1,"Quiesced reset failed");
  end
 endtask
 task submit;
  begin
   if(!token_ready)$fatal(1,"Token not ready");
   token_valid=1;@(negedge clk);token_valid=0;
  end
 endtask
 task expect_fault;
  begin
   #1;
   if(!bus_fault||ack_valid||token_ready||!busy)
    $fatal(1,"Response error did not immediately quarantine completion");
   @(negedge clk);bvalid=0;rvalid=0;bresp=0;rresp=0;
   cancel=1;ack_ready=1;drained=1;
   repeat(5)begin
    @(negedge clk);
    if(!bus_fault||!fault||ack_valid||token_ready||!busy)
     $fatal(1,"Response fault lost quarantine");
   end
  end
 endtask
 initial begin
  reset();bresp=3;rresp=2; // Invalid channels may carry arbitrary response bits.
  repeat(5)@(negedge clk);
  if(bus_fault||fault)$fatal(1,"Invalid response caused a fault");
  $display("PASS response fault: invalid response bits ignored");

  reset();bvalid=1;rvalid=1;
  repeat(3)@(negedge clk);
  bresp=1;rresp=1; // AXI EXOKAY is not SLVERR or DECERR.
  repeat(3)@(negedge clk);
  if(bus_fault||fault)$fatal(1,"Successful response caused a fault");
  $display("PASS response fault: OKAY and EXOKAY do not fault");

  // Every error code on either channel must remain sticky, even before a job.
  for(channel=0;channel<2;channel=channel+1)begin
   for(code=2;code<4;code=code+1)begin
    reset();
    if(channel==0)begin bvalid=1;bresp=code;end
    else begin rvalid=1;rresp=code;end
    expect_fault();
    $display("PASS response fault: channel %0d error %0d sticky",channel,code);
   end
  end

  reset();submit();
  // No READY/RLAST qualification: a valid stalled/nonfinal read error counts.
  rvalid=1;rresp=2;drained=1;
  #1;if(!bus_fault||ack_valid)$fatal(1,"Stalled read error escaped");
  repeat(8)@(negedge clk);
  expect_fault();
  $display("PASS response fault: stalled read error overrides apparent drain");

  reset();submit();drained=1;
  repeat(3)@(negedge clk);
  if(!ack_valid)$fatal(1,"Expected retained acknowledgement");
  bvalid=1;bresp=3;ack_ready=1;
  // The error must mask ACK before the next edge, not one cycle afterwards.
  expect_fault();
  $display("PASS response fault: write error blocks same-cycle ACK consumption");

  reset();submit();bvalid=1;bresp=2;rvalid=1;rresp=3;
  expect_fault();
  reset();submit();drained=1;
  repeat(3)@(negedge clk);
  if(!ack_valid||bus_fault)$fatal(1,"Fresh session failed after quiesced reset");
  ack_ready=1;@(negedge clk);
  if(ack_valid||busy)$fatal(1,"Fresh result failed to consume");
  $display("PASS response fault: simultaneous errors and quiesced recovery");
  $finish;
 end
 initial begin #20000;$fatal(1,"Response fault simulation deadline");end
endmodule
