`timescale 1ns/1ps
module full_renderer_test;
 reg clk=0,resetn=0;
 always #5 clk=~clk;
 reg cmd_valid=0,cmd_last=0; reg [31:0] cmd_data=0;
 wire cmd_ready,resp_valid,resp_ready,resp_last,idle;
 wire [31:0] resp_data;
 reg [31:0] commands[0:2047];
 integer starts[0:4],sizes[0:4];
 integer gta_alpha=0;
 integer tick=0,scene=0,aw_count=0,b_count=0,ar_count=0,rlast_count=0;
 integer ddr_commands=0,unaligned=0,ddr_words;
 integer b_delay=0,r_delay=0,b_wait=0,r_wait=0;
 integer inject_error=0,do_cancel=0,unsafe_drain=0,ack_tick=0,token_tick=0;
 reg ack_ready=0,cancel=0;
 wire b_allow=b_wait>=b_delay;
 wire r_allow=r_wait>=r_delay;
 wire [1:0] observed_bresp,observed_rresp;
 wire token_valid,token_ready,token_fault,memory_fault,ack_valid,fault;
 wire [31:0] token_epoch,token_sequence,ack_epoch,ack_sequence;
 wire ack_cancelled;
 `include "gpu_wires.vh"
`ifdef GRX_HP3
 assign observed_bresp=bresp;
 assign observed_rresp=rresp;
`else
 assign observed_bresp=inject_error==1 ? 2'b10 : ram_bresp;
 assign observed_rresp=inject_error==2 ? 2'b10 : ram_rresp;
`endif
 grx_response_token token(.clk(clk),.resetn(resetn),.s_valid(resp_valid),.s_ready(resp_ready),
  .s_data(resp_data),.s_last(resp_last),.external_fault(memory_fault),
  .token_valid(token_valid),.token_ready(token_ready),.token_epoch(token_epoch),
  .token_sequence(token_sequence),.fault(token_fault));
 grx_axi_response_fault observer(.clk(clk),.resetn(resetn),.bvalid(bvalid),.bresp(observed_bresp),
  .rvalid(rvalid),.rresp(observed_rresp),.fault(memory_fault));
 grx_completion completion(.clk(clk),.resetn(resetn),.token_valid(token_valid),
  .token_ready(token_ready),.token_epoch(token_epoch),.token_sequence(token_sequence),
  .drained(unsafe_drain?1'b1:idle),.cancel(cancel),.bus_error(memory_fault|token_fault),
  .ack_valid(ack_valid),.ack_ready(ack_ready),.ack_epoch(ack_epoch),
  .ack_sequence(ack_sequence),.ack_cancelled(ack_cancelled),.fault(fault),.busy());
 integer command_seen=0;
 always @(posedge clk) if(resetn && gpu.cmd_axis_tvalid && gpu.cmd_axis_tready)begin
  if(ddr_commands)begin
   if(command_seen < sizes[scene]-9)begin
    if(gpu.cmd_axis_tdata !== commands[starts[scene]+2+command_seen])
     $fatal(1,"Command mismatch scene=%0d word=%0d got=%h wanted=%h",scene,command_seen,gpu.cmd_axis_tdata,commands[starts[scene]+2+command_seen]);
   end else if(command_seen<ddr_words)begin
    if(gpu.cmd_axis_tdata!==0)$fatal(1,"DDR padding is not NOP");
   end else if(command_seen!=ddr_words || gpu.cmd_axis_tdata!==32'h60000000)
    $fatal(1,"Unexpected trailing DDR command");
   command_seen<=command_seen+1;
  end
 end
 integer fifo_in=0,fifo_out=0,fifo_sent=0;
 always @(posedge clk) if(resetn)begin
  if(gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.s_mem_axi_wvalid && gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.s_mem_axi_wready)fifo_in<=fifo_in+1;
  if(gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.dataFiFo.w_rd)fifo_out<=fifo_out+1;
  if(gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.m_mem_axi_wvalid && gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.m_mem_axi_wready)fifo_sent<=fifo_sent+1;
 end
 always @(posedge clk) begin
  tick<=tick+1;
  if(resetn && fifo_out>fifo_sent+gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.m_mem_axi_wvalid+gpu.rixef.colorBuffer.coalescer.gen_coalesce.coalesce_fifo_write_inst.skid_valid)begin
   $fatal(1,"Coalesced FIFO consumed a word without retaining it");
  end
  if(tick>150000)$fatal(1,"Renderer timeout scene=%0d idle=%b aw=%0d b=%0d ar=%0d rlast=%0d token=%0d",scene,idle,aw_count,b_count,ar_count,rlast_count,token_tick);
  if(resetn)begin
   if(awvalid&&awready)begin
    aw_count<=aw_count+1;
    if(awaddr[31:18]!==32'h06600000>>18)$fatal(1,"Write escaped test aperture or has unknown address");
   end
   if(bvalid&&bready)b_count<=b_count+1;
   if(arvalid&&arready)begin
    ar_count<=ar_count+1;
    if(araddr[31:18]!==32'h06600000>>18)$fatal(1,"Read escaped test aperture or has unknown address");
   end
   if(rvalid&&rready&&rlast)rlast_count<=rlast_count+1;
   if(ram_bvalid&&!b_allow)b_wait<=b_wait+1;
   if(ram_bvalid&&ram_bready)b_wait<=0;
   if(ram_rvalid&&!r_allow)r_wait<=r_wait+1;
   if(ram_rvalid&&ram_rready)r_wait<=0;
   if(token_valid&&token_ready)begin token_tick<=tick;if(do_cancel)cancel<=1;end
   if(ack_valid)begin
    if(aw_count!=b_count||ar_count!=rlast_count)$fatal(1,"Acknowledged outstanding memory transactions");
    if(memory_fault||token_fault)$fatal(1,"Acknowledged memory error");
   end
  end
 end
 task send(input [31:0] word);
 begin
  @(negedge clk);cmd_data=word;cmd_valid=1;
  @(posedge clk);while(!cmd_ready)@(posedge clk);
  @(negedge clk);cmd_valid=0;
 end
 endtask
 integer i,k,fd,first_tick,before_aw,before_ar;reg [63:0] texword;reg [15:0] texel;
 string dump_name;
 initial begin
  if($value$plusargs("unaligned=%d",unaligned))begin end
  if($value$plusargs("ddr=%d",ddr_commands))begin end
  if($value$plusargs("b_delay=%d",b_delay))begin end
  if($value$plusargs("r_delay=%d",r_delay))begin end
  if($value$plusargs("error=%d",inject_error))begin end
  if($value$plusargs("cancel=%d",do_cancel))begin end
  if($value$plusargs("unsafe=%d",unsafe_drain))begin end
  `include "scene_sizes.vh"
  gta_alpha=$test$plusargs("gta_alpha");
  $readmemh("commands.hex",commands,0,starts[4]+sizes[4]-1);
  repeat(5)@(negedge clk);
  for(i=0;i<65536/8;i=i+1)ram.mem[32'h10000/8+i]=64'ha55aa55aa55aa55a;
  for(i=0;i<1024;i=i+4)begin
   for(k=0;k<4;k=k+1)begin texel=((i+k)%16<8)?16'hf800:(gta_alpha?16'h003f:16'h001f);texword[k*16+:16]=texel;end
   ram.mem[32'h20000/8+i/4]=texword;
  end
  ram.mem[32'h10000/8-1]=64'hfeedface1234abcd;
  ram.mem[(32'h10000+9216)/8]=64'hdeadcafe9876abcd;
  resetn=1;repeat(10)@(negedge clk);
  for(scene=0;scene<5;scene=scene+1)begin
   command_seen=0;first_tick=tick;before_aw=aw_count;before_ar=ar_count;token_tick=0;
   if(ddr_commands)begin
    // Feed the render payload through the board's real command DDR reader.
    // Exclude the two-word inline envelope and seven-word fence/response tail.
    // The existing reader issues complete 16-word bursts: pad with NOPs.
    ddr_words=unaligned ? ((sizes[scene]-9)|1) : ((sizes[scene]-9+15)/16)*16;
    for(i=0;i<ddr_words;i=i+1)
     ram.mem[(32'h30000+scene*4096)/8+i/2][(i%2)*32+:32]=
      i<sizes[scene]-9 ? commands[starts[scene]+2+i] : 0;
    send(32'hb0000000 | (ddr_words*4));
    send(32'h06630000+scene*4096);
    for(i=starts[scene]+sizes[scene]-7;i<starts[scene]+sizes[scene];i=i+1)send(commands[i]);
   end else begin
    for(i=starts[scene];i<starts[scene]+sizes[scene];i=i+1)send(commands[i]);
   end
   if(unaligned)begin
    repeat(500)@(negedge clk);
    if(ack_valid||idle)$fatal(1,"Unread DDR padding acknowledged");
    $display("PASS full renderer: unread DDR padding keeps completion pending");$finish;
   end
   if(inject_error==1 || (inject_error==2&&scene==2))begin
    repeat(500)@(negedge clk);
    if(!fault||ack_valid)$fatal(1,"Error not quarantined");
    $display("PASS full renderer: memory error quarantined");$finish;
   end
   wait(ack_valid);@(negedge clk);ack_tick=tick;
   if(ack_epoch!=32'habcddcba||ack_sequence!=scene+1)$fatal(1,"Wrong scene identity");
   if(ack_cancelled!=do_cancel)$fatal(1,"Wrong cancellation state");
   if(aw_count==before_aw)$fatal(1,"No framebuffer writes");
   if(ddr_commands&&command_seen!=ddr_words+1)$fatal(1,"Missing DDR commands");
   if(ddr_commands&&ar_count==before_ar)$fatal(1,"No command DDR reads");
   if(scene==3&&ar_count==before_ar)$fatal(1,"No texture reads");
   $display("PASS full renderer: scene=%0d cycles=%0d token_cycle=%0d ack_cycle=%0d writes=%0d reads=%0d cancel=%0d",scene,tick-first_tick,token_tick,ack_tick,aw_count-before_aw,ar_count-before_ar,ack_cancelled);
   if(ram.mem[32'h10000/8-1]!=64'hfeedface1234abcd || ram.mem[(32'h10000+9216)/8]!=64'hdeadcafe9876abcd)$fatal(1,"Framebuffer guard changed");
   for(i=0;i<1024;i=i+1)begin
    texel=(i%16<8)?16'hf800:(gta_alpha?16'h003f:16'h001f);
    if(((ram.mem[32'h20000/8+i/4]>>((i%4)*16))&16'hffff)!=texel)$fatal(1,"Texture modified");
   end
   dump_name=$sformatf("pixels-%0d.hex",scene);fd=$fopen(dump_name,"w");
   for(i=0;i<4608;i=i+1)$fdisplay(fd,"%04x",(ram.mem[32'h10000/8+i/4]>>((i%4)*16))&16'hffff);
   $fclose(fd);
   /* Result and identity must remain stable while the CPU is stalled. */
   repeat(25)begin @(negedge clk);if(!ack_valid||ack_sequence!=scene+1||ack_epoch!=32'habcddcba||ack_cancelled!=do_cancel)$fatal(1,"Lost retained result");end
   ack_ready=1;@(negedge clk);ack_ready=0;cancel=0;repeat(3)@(negedge clk);
  end
  $finish;
 end
endmodule
