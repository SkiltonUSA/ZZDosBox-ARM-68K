"""Connect the board-generated AXI4-to-AXI3 converter to the test RAM."""
from pathlib import Path


def one(paths):
    paths=list(paths)
    if len(paths)!=1:
        raise ValueError('Expected one generated converter dependency: '+str(paths))
    return paths[0]


def sources(generated):
    wrapper=one(generated.glob('ip/zz9000_ps_axi_interconnect_4_imp_auto_pc_0/sim/*.v'))
    dependencies=[one(generated.glob('ipshared/*/hdl/'+name)) for name in (
        'axi_protocol_converter*_vl_rfs.v', 'axi_register_slice*_vl_rfs.v',
        'axi_data_fifo*_vl_rfs.v',
        'axi_infrastructure*_vl_rfs.v')]
    header=one(generated.glob('ipshared/*/hdl/axi_infrastructure*.vh'))
    fifo=one(generated.glob('ipshared/*/hdl/fifo_generator*_rfs.v'))
    behavior=one(generated.glob('ipshared/*/simulation/fifo_generator_vlog_beh.v'))
    return [wrapper,*dependencies,fifo,behavior],header


def wires(widths):
    hp=dict(widths,awlen=4,arlen=4,awlock=2,arlock=2,wid=6,awqos=4,arqos=4)
    text='\n'.join(f'wire [{n-1}:0] hp_{k};' for k,n in hp.items())+'\n'
    text+='assign hp_bvalid=ram_bvalid && b_allow;assign ram_bready=hp_bready && b_allow;\n'
    text+='assign hp_rvalid=ram_rvalid && r_allow;assign ram_rready=hp_rready && r_allow;\n'
    text+="assign hp_bresp=inject_error==1 ? 2'b10 : ram_bresp;\n"
    text+="assign hp_rresp=inject_error==2 ? 2'b10 : ram_rresp;\n"
    ports=['.aclk(clk)','.aresetn(resetn)']
    ports += [f'.s_axi_{k}({k})' for k in widths]
    ports += [f'.s_axi_{k}(4\'b0)' for k in ('awregion','arregion','awqos','arqos')]
    ports += [f'.m_axi_{k}(hp_{k})' for k in hp]
    text+='zz9000_ps_axi_interconnect_4_imp_auto_pc_0 hp3_converter(\n'+',\n'.join(ports)+');\n'
    remap=dict(awaddr='hp_awaddr[17:0]',araddr='hp_araddr[17:0]',
        awlen="{4'b0,hp_awlen}",arlen="{4'b0,hp_arlen}",
        awlock='hp_awlock[0]',arlock='hp_arlock[0]',bvalid='ram_bvalid',bready='ram_bready',
        bresp='ram_bresp',rresp='ram_rresp',rvalid='ram_rvalid',rready='ram_rready')
    text+='axi_ram #(.DATA_WIDTH(64),.ID_WIDTH(6),.ADDR_WIDTH(18)) ram(.clk(clk),.rst(!resetn),\n'
    text+=',\n'.join(f'.s_axi_{k}({remap.get(k,"hp_"+k)})' for k in widths)+');\n'
    # The RAM model serializes writes; assert the AXI3 ID/last/length contract.
    text+='''
 integer hp_aw_count=0,hp_b_count=0,hp_ar_count=0,hp_rlast_count=0;
 integer hp_write_beat=0,hp_write_length=0,hp_max_write_length=0;
 reg [5:0] hp_write_id=0;
 always @(posedge clk)if(resetn)begin
  if(hp_awvalid&&hp_awready)begin
   hp_aw_count<=hp_aw_count+1;hp_write_id<=hp_awid;
   hp_write_length<=hp_awlen+1;hp_write_beat<=0;
   if(hp_awlen+1>hp_max_write_length)hp_max_write_length<=hp_awlen+1;
   if(hp_awaddr[31:18]!==32'h06600000>>18)$fatal(1,"HP3 write escaped aperture or has unknown address");
  end
  if(hp_wvalid&&hp_wready)begin
   if(hp_wid!==hp_write_id)$fatal(1,"AXI3 WID mismatch");
   if(hp_wlast!==(hp_write_beat==hp_write_length-1))$fatal(1,"AXI3 WLAST mismatch");
   hp_write_beat<=hp_write_beat+1;
  end
  if(hp_bvalid&&hp_bready)hp_b_count<=hp_b_count+1;
  if(hp_arvalid&&hp_arready)begin
   hp_ar_count<=hp_ar_count+1;
   if(hp_araddr[31:18]!==32'h06600000>>18)$fatal(1,"HP3 read escaped aperture or has unknown address");
  end
  if(hp_rvalid&&hp_rready&&hp_rlast)hp_rlast_count<=hp_rlast_count+1;
  if(ack_valid&&(hp_aw_count!=hp_b_count||hp_ar_count!=hp_rlast_count))
   $fatal(1,"Acknowledged outstanding memory transactions at HP3");
 end
 final $display("HP3 traffic: writes=%0d GPU_writes=%0d reads=%0d GPU_reads=%0d max_write_beats=%0d",
   hp_aw_count,aw_count,hp_ar_count,ar_count,hp_max_write_length);
'''
    return text
