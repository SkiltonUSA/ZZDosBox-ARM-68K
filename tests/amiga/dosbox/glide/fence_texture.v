// Upstream TextureReader + parser. Synthetic AXI reads, no board execution.
`timescale 1ns/1ps
module texture_fence_test;
parameter DATA_WIDTH=32;
localparam BYTES=DATA_WIDTH/8;
reg clk=0,resetn=0,valid=0;
reg [31:0] data=0;
wire ready,idle,texture_pending,page_valid,page_last,page_ready,config_valid;
wire [31:0] command_data;
wire [4:0] config_register;
reg tr_valid=0,tr_ready=0,inflight=0;
wire input_ready,result_valid;
reg [16:0] a00=0,a01=0,a10=0,a11=0;
wire [15:0] q00,q01,q10,q11;
reg arready=0,rvalid=0,rlast=0;
reg [DATA_WIDTH-1:0] rdata=0;
reg [3:0] rid=0;
wire arvalid,rready;
wire [31:0] araddr;
wire [7:0] arlen;
wire [2:0] arsize;
wire [1:0] arburst;
wire [3:0] arid;
always #5 clk=~clk;
CommandParser parser(
 .aclk(clk),.resetn(resetn),.s_cmd_axis_tvalid(valid),.s_cmd_axis_tready(ready),
 .s_cmd_axis_tlast(1'b0),.s_cmd_axis_tdata(data),
 .m_cmd_fog_axis_tready(1'b1),.m_cmd_rasterizer_axis_tready(1'b1),
 .m_cmd_tmu0_axis_tready(page_ready),.m_cmd_tmu1_axis_tready(1'b1),
 .m_cmd_config_axis_tready(1'b1),.m_cmd_config_axis_tvalid(config_valid),
 .m_cmd_xxx_axis_tdata(command_data),.m_cmd_xxx_axis_tuser(config_register),
 .m_cmd_xxx_axis_tlast(page_last),.m_cmd_tmu0_axis_tvalid(page_valid),
 .rasterizerRunning(1'b0),.pixelInPipeline(inflight),.dataInTriangleInterpolator(1'b0),
 .framebufferWritesPending(1'b0),.textureMemoryTransactionsPending(texture_pending),.idle(idle),
 .colorBufferApplied(1'b1),.depthBufferApplied(1'b1),.stencilBufferApplied(1'b1),
 .framebufferClearApplied(1'b1)
);
TextureReader #(.DATA_WIDTH(DATA_WIDTH)) texture_reader(
 .aclk(clk),.resetn(resetn),.enable(1'b1),.nearest(1'b0),
 .s_tr_valid(tr_valid),.s_tr_ready(input_ready),
 .s_tr_addr_00(a00),.s_tr_addr_01(a01),.s_tr_addr_10(a10),.s_tr_addr_11(a11),
 .m_tr_valid(result_valid),.m_tr_ready(tr_ready),
 .m_tr_texel_00(q00),.m_tr_texel_01(q01),.m_tr_texel_10(q10),.m_tr_texel_11(q11),
 .s_axis_tvalid(page_valid),.s_axis_tready(page_ready),
 .s_axis_tlast(page_last),.s_axis_tdata(command_data),
 .m_axi_arid(arid),.m_axi_araddr(araddr),.m_axi_arlen(arlen),
 .m_axi_arsize(arsize),.m_axi_arburst(arburst),.m_axi_arvalid(arvalid),.m_axi_arready(arready),
 .m_axi_rid(rid),.m_axi_rdata(rdata),.m_axi_rresp(2'b00),
 .m_axi_rlast(rlast),.m_axi_rvalid(rvalid),.m_axi_rready(rready),
 .memoryTransactionsPending(texture_pending)
);
integer tick=0,requests=0,completed=0,beat=0,last_ar=-100;
integer due=0,tokens=0,outputs=0,early_outputs=0;
reg [31:0] bases[0:255];
integer lengths[0:255];
reg [3:0] ids[0:255];
reg [31:0] page0=0,page1=0,expected_token=0;
reg previous_transfer=0;
function [15:0] memory_texel(input reg [31:0] addr);
 begin memory_texel=16'((addr>>1)^(addr>>16)^32'h5a31);end
endfunction
function [15:0] expected(input reg [16:0] address);
 reg [31:0] physical;
 begin
  physical=(address<1024? page0:page1)+(address*2)%2048;
  expected=memory_texel(physical);
 end
endfunction
always @(posedge clk)begin
 if(!resetn)begin
  tick=0;requests=0;completed=0;beat=0;tokens=0;outputs=0;early_outputs=0;previous_transfer=0;
 end else begin
  tick=tick+1;previous_transfer=rvalid&&rready;
  if(arvalid&&arready)begin
   if(requests>=256)$fatal(1,"Read scoreboard overflow");
   if(arsize!==3'($clog2(BYTES))||arburst!==2'b01||arlen!==8'(32/BYTES-1))$fatal(1,"Invalid cache-line burst");
   if(araddr<page0||!((araddr>=page0&&araddr<page0+2048)||(araddr>=page1&&araddr<page1+2048)))$fatal(1,"Read outside mapped pages");
   bases[requests]=araddr;lengths[requests]=arlen+1;ids[requests]=arid;
   requests=requests+1;last_ar=tick;
  end
  if(previous_transfer)begin
   if(rlast)begin completed=completed+1;beat=0;end
   else beat=beat+1;
  end
  if(result_valid&&tr_ready)begin
   if(q00!==expected(a00)||q01!==expected(a01)||q10!==expected(a10)||q11!==expected(a11))
    $fatal(1,"Stale or corrupt texture: %h %h %h %h expected %h %h %h %h",q00,q01,q10,q11,expected(a00),expected(a01),expected(a10),expected(a11));
   outputs=outputs+1;
   if(requests>completed)early_outputs=early_outputs+1;
  end
  if(config_valid)begin
   if(completed!=requests||inflight)$fatal(1,"Completion token overtook texture reads");
   if(config_register!==5'd1||command_data!==expected_token)$fatal(1,"Wrong texture token");
   tokens=tokens+1;
  end
 end
end
integer lane;
always @(negedge clk)begin
 if(!resetn)begin arready=0;rvalid=0;rlast=0;due=0;end
 else begin
  arready=tick%11>=4;
  if(requests>completed&&tick>last_ar+1&&texture_pending!==1'b1)$fatal(1,"Lost outstanding texture read");
  if(previous_transfer)begin
   rvalid=0;rlast=0;
   due=tick+3;
   if(completed<requests&&beat==lengths[completed]-1)due=tick+31;
  end
  if(!rvalid&&completed<requests&&tick>=due&&tick>last_ar+4)begin
   rdata=0;
   for(lane=0;lane<BYTES/2;lane=lane+1)
    rdata[lane*16+:16]=memory_texel(bases[completed]+beat*BYTES+lane*2);
   rid=ids[completed];rlast=(beat==lengths[completed]-1);rvalid=1;
  end
 end
end
task send_word(input reg [31:0] word);
 integer timeout;
 begin
  @(negedge clk);valid=1;data=word;timeout=0;
  @(posedge clk);
  while(ready!==1'b1)begin
   @(posedge clk);timeout=timeout+1;
   if(timeout>10000)$fatal(1,"Texture command timeout");
  end
  @(negedge clk);valid=0;
 end
endtask
task map_pages(input reg [31:0] base);
 begin
  page0=base;page1=base+32'h10000;
  send_word(32'h50000002);send_word(page0);send_word(page1);
  repeat(80)@(negedge clk); // Allow cache invalidation to finish.
 end
endtask
task sample(input integer n,input integer kind,input integer hit);
 integer old_requests,old_outputs,old_tokens;
 begin
  old_requests=requests;old_outputs=outputs;old_tokens=tokens;
  @(negedge clk);
  case(kind)
   1:begin a00=15;a01=16;a10=31;a11=32;end
   2:begin a00=1023;a01=1024;a10=1025;a11=1026;end
   default:begin a00=16;a01=17;a10=18;a11=19;end
  endcase
  expected_token=32'hbcde0000+n;inflight=1;tr_valid=1;
  @(posedge clk);while(input_ready!==1'b1)@(posedge clk);
  @(negedge clk);tr_valid=0;
  // Texel requests come from a simulated pixel stage, not the parser's
  // triangle stream. Establish activity before submitting its trailing fence.
  while(texture_pending!==1'b1&&result_valid!==1'b1)@(negedge clk);
  fork
   begin
    send_word(32'h60000000);send_word(32'h10000001);send_word(expected_token);
   end
   begin
    while(result_valid!==1'b1)@(negedge clk);
    repeat(23)begin
     @(negedge clk);
     if(tokens!=old_tokens)$fatal(1,"Token passed blocked texel consumer");
    end
    tr_ready=1;@(posedge clk);@(negedge clk);tr_ready=0;inflight=0;
   end
  join
  repeat(10)@(negedge clk);
  if(outputs!=old_outputs+1||tokens!=old_tokens+1||idle!==1'b1||texture_pending!==1'b0)$fatal(1,"Texture batch did not retire cleanly");
  if(hit&&requests!=old_requests)$fatal(1,"Expected cache hit fetched memory");
  if(!hit&&requests==old_requests)$fatal(1,"Expected cache miss did not fetch");
  $display("PASS texture fence: width=%0d case=%0d reads=%0d",DATA_WIDTH,n,requests-old_requests);
 end
endtask
initial begin
 repeat(4)@(negedge clk);resetn=1;
 map_pages(32'h10000000);
 sample(1,0,0);sample(2,0,1);
 map_pages(32'h20000000); // Same virtual texels, different physical texture.
 sample(3,0,0);sample(4,0,1);
 sample(5,1,0);sample(6,2,0);sample(7,2,1);
 map_pages(32'h10000000);sample(8,0,0);
 if(early_outputs==0)$fatal(1,"Early texture delivery was not exercised");
 $display("Texture outputs consumed before final read response: %0d",early_outputs);
 $finish;
end
initial begin #2000000;$fatal(1,"Texture simulation deadline");end
endmodule
