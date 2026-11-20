#!/usr/bin/env python3
"""Full RasterIX rendering/completion simulation; never accesses hardware."""
import argparse,hashlib,json,shutil,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/full-renderer'

def wires(board=False,hp3=False):
    widths=dict(awid=6,awaddr=32,awlen=8,awsize=3,awburst=2,awlock=1,awcache=4,awprot=3,
        awvalid=1,awready=1,wdata=64,wstrb=8,wlast=1,wvalid=1,wready=1,bid=6,bresp=2,bvalid=1,bready=1,
        arid=6,araddr=32,arlen=8,arsize=3,arburst=2,arlock=1,arcache=4,arprot=3,arvalid=1,arready=1,
        rid=6,rdata=64,rresp=2,rlast=1,rvalid=1,rready=1)
    text='\n'.join(f'wire [{n-1}:0] {k};' for k,n in widths.items())
    text+='\nwire ram_bvalid,ram_bready,ram_rvalid,ram_rready;wire [1:0] ram_bresp,ram_rresp;\n'
    if not hp3:
        text+='assign bvalid=ram_bvalid && b_allow;assign ram_bready=bready && b_allow;\n'
        text+='assign rvalid=ram_rvalid && r_allow;assign ram_rready=rready && r_allow;\n'
    ports=['.aclk(clk)','.resetn(resetn)','.s_cmd_axis_tvalid(cmd_valid)',
        '.s_cmd_axis_tready(cmd_ready)','.s_cmd_axis_tlast(cmd_last)', '.s_cmd_axis_tdata(cmd_data)',
        '.m_cmd_resp_axis_tvalid(resp_valid)','.m_cmd_resp_axis_tready(resp_ready)',
        '.m_cmd_resp_axis_tlast(resp_last)','.m_cmd_resp_axis_tdata(resp_data)',
        '.fb_swapped(1\'b1)','.idle(idle)']
    ports += [f'.m_axi_{k}({"observed_bresp" if k=="bresp" else "observed_rresp" if k=="rresp" else k})' for k in widths]
    text+='RasterIX_EF #(.DATA_WIDTH(64),.ID_WIDTH(6),.TMU_COUNT(1),.ENABLE_TEXTURE_CACHE(0),\n'
    text+=' .RASTERIZER_FIXPOINT_PRECISION(18),.RASTERIZER_FLOAT_PRECISION(24)) gpu(\n'+',\n'.join(ports)+');\n'
    if hp3:
        from glide_hp3_fixture import wires as hp3_wires
        return text.replace(',.ENABLE_TEXTURE_CACHE(0)','')+hp3_wires(widths)
    remap=dict(awaddr='awaddr[17:0]',araddr='araddr[17:0]',bvalid='ram_bvalid',bready='ram_bready',
        bresp='ram_bresp',rresp='ram_rresp',rvalid='ram_rvalid',rready='ram_rready')
    text+='axi_ram #(.DATA_WIDTH(64),.ID_WIDTH(6),.ADDR_WIDTH(18)) ram(.clk(clk),.rst(!resetn),\n'
    text+=',\n'.join(f'.s_axi_{k}({remap.get(k,k)})' for k in widths)+');\n'
    if board:text=text.replace(',.ENABLE_TEXTURE_CACHE(0)','')
    return text

def normalize(path):
    # Give discarded output bits wires instead of illegal constant destinations.
    # Applied only to the generated simulation copy; rendering logic is unchanged.
    changes={}
    p=next(path.rglob('TextureReader.v'));s=p.read_text();before=s
    old=".m_stream_tdata({\n            1'b0,\n            1'b0,\n            fifoTexelPos,"
    assert s.count(old)==1
    s=s.replace(old,'.m_stream_tdata({\n            unusedStreamBits,\n            fifoTexelPos,')
    anchor='    StreamConcatFifo #('
    assert anchor in s
    s=s.replace(anchor,'    wire [1:0] unusedStreamBits;\n'+anchor,1)
    p.write_text(s);changes[str(p.relative_to(path))]={'before':sha(before.encode()),'after':sha(s.encode())}
    return changes

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--upstream',type=Path,required=True,help='Directory containing RasterIX and Float checkouts')
    p.add_argument('--verilator',default='verilator')
    p.add_argument('--board',action='store_true',help='Use the minimally patched XX19d-RIX renderer')
    p.add_argument('--output',type=Path,default=OUT,help='Generated build and evidence directory')
    p.add_argument('--ordered-state',action='store_true');p.add_argument('--build-only',action='store_true')
    p.add_argument('--prepare-only',action='store_true',help='Write input list for an external simulator')
    p.add_argument('--hp3-generated',type=Path,help='Generated board zz9000_ps directory for AXI3 conversion')
    a=p.parse_args()
    if a.hp3_generated and not a.board:p.error('--hp3-generated requires --board')
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    src=out/'source';src.mkdir(exist_ok=True)
    original={}
    for component in ['RasterIX','Float']:
        source=a.upstream.resolve()/component/'rtl';target=src/component
        if target.exists():shutil.rmtree(target)
        shutil.copytree(source,target)
        for f in source.rglob('*'):
            if f.is_file():original[component+'/'+str(f.relative_to(source))]=sha(f.read_bytes())
    changes={} if a.board else normalize(src)
    native=out/'fixtures';base=ROOT/'amiga/dosbox/glide'
    subprocess.run(['clang','-std=c11','-O2','-Wall','-Wextra','-Werror','-I'+str(ROOT),
       ROOT/'tests/amiga/dosbox/glide/full_renderer_fixtures.c',*[base/x for x in ['probe_scene.c','translator.c','descriptor.c']],'-o',native],check=True)
    fixture=subprocess.run([native,*(["--ordered-state"] if a.ordered_state else [])],capture_output=True,text=True,check=True)
    (out/'commands.hex').write_text(fixture.stdout)
    (out/'scene_sizes.vh').write_text('\n'.join(f'starts[{i}]={row.split()[0]};sizes[{i}]={row.split()[1]};' for i,row in enumerate(fixture.stderr.splitlines())))
    (out/'gpu_wires.vh').write_text(wires(a.board,bool(a.hp3_generated)))
    deps=[base/'experimental'/x for x in ['completion.v','response_token.v','axi_response_fault.v']]
    rtl=sorted([* (src/'RasterIX/RasterIX').glob('*.v'), * (src/'Float/float').glob('*.v'), * (src/'RasterIX/3rdParty/verilog-axi').glob('*.v'), * (src/'RasterIX/3rdParty/verilog-axis').glob('*.v'), src/'RasterIX/3rdParty/sfifo.v', src/'RasterIX/3rdParty/skidbuffer.v'])
    includes=sorted({str(f.parent) for f in src.rglob('*.vh')})+[str(out)]
    vendor=[];vendor_header=None
    if a.hp3_generated:
        from glide_hp3_fixture import sources
        vendor,vendor_header=sources(a.hp3_generated.resolve())
        includes.append(str(vendor_header.parent))
    cmd=[a.verilator,'--binary','--timing','-j','4','-Wno-fatal','-DUNITTEST',
         '--top-module','full_renderer_test','--Mdir',str(out/'verilated'),
         *['-I'+d for d in includes],str(ROOT/'tests/amiga/dosbox/glide/full_renderer.v'),
         *(['-DGRX_HP3'] if a.hp3_generated else []),*map(str,deps+rtl+vendor)]
    if a.prepare_only:
        (out/'simulation-inputs.json').write_text(json.dumps({
            'sources':[str(ROOT/'tests/amiga/dosbox/glide/full_renderer.v'),*map(str,deps+rtl+vendor)],
            'includes':includes,'defines':['UNITTEST']+(['GRX_HP3'] if a.hp3_generated else []),
            'headers':list(map(str,sorted(src.rglob('*.vh'))))+([str(vendor_header)] if vendor_header else []),
            'source_sha256':original,'ordered_state':a.ordered_state},indent=2)+'\n')
        return
    build=subprocess.run(cmd,capture_output=True,text=True)
    (out/'build.log').write_text((build.stdout+build.stderr).replace(str(ROOT)+'/',''))
    record={'scope':('Patched XX19d-RIX renderer' if a.board else 'Upstream renderer')+' with synthetic AXI RAM, not full board/HP3/hardware timing',
      'hp3_converter':bool(a.hp3_generated),'vendor_sha256':{p.name:sha(p.read_bytes()) for p in vendor+([vendor_header] if vendor_header else [])},
      'board_renderer':a.board,'source_sha256':original,'ordered_state':a.ordered_state,'simulator':subprocess.check_output([a.verilator,'--version'],text=True).strip(),'normalization':changes,'build_exit':build.returncode,'cases':[],
      'harness_sha256':{str(f.relative_to(ROOT)):sha(f.read_bytes()) for f in [Path(__file__).resolve(),ROOT/'tests/amiga/dosbox/glide/full_renderer.v',ROOT/'tests/amiga/dosbox/glide/full_renderer_fixtures.c',*deps]},
      'generated_sha256':{name:sha((out/name).read_bytes()) for name in ['commands.hex','scene_sizes.vh','gpu_wires.vh']}}
    (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    if a.hp3_generated:
        record['scope']='Patched board renderer and generated AXI4-to-AXI3 converter with synthetic RAM; no PS7/DDR hardware timing'
        record['harness_sha256']['scripts/glide_hp3_fixture.py']=sha((ROOT/'scripts/glide_hp3_fixture.py').read_bytes())
        (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    if build.returncode:raise SystemExit('Full renderer compile failed; see '+str(out/'build.log'))
    if a.build_only:return
    cases=[('normal',[]),('delayed',['+b_delay=37','+r_delay=3']),('cancel',['+cancel=1']),('write-error',['+error=1']),('read-error',['+error=2']),('unsafe-drain',['+unsafe=1','+b_delay=37','+r_delay=3'])]
    if a.board:cases += [('ddr',['+ddr=1']),('ddr-delayed',['+ddr=1','+b_delay=37','+r_delay=3']),('ddr-cancel',['+ddr=1','+cancel=1']),('ddr-unaligned',['+ddr=1','+unaligned=1'])]
    for label,args in cases:
        r=subprocess.run([str(out/'verilated/Vfull_renderer_test'),*args],cwd=out,capture_output=True,text=True,timeout=180)
        text=(r.stdout+r.stderr).replace(str(ROOT)+'/', '');(out/(label+'.log')).write_text(text);print(text,end='')
        if label=='unsafe-drain':
            if not r.returncode or 'Acknowledged outstanding memory transactions' not in text:
                raise SystemExit('Unsafe drain was not detected')
            record['cases'].append(dict(name=label,args=args,detected=True,exit=r.returncode))
            (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
            continue
        if r.returncode:raise SystemExit(label+' failed')
        expected={'write-error':1,'read-error':3,'ddr-unaligned':1}.get(label,5)
        if text.count('PASS full renderer:')!=expected:raise SystemExit('Missing scene results')
        checks=[]
        if label not in ('write-error','ddr-unaligned'):
            for scene in range(2 if label=='read-error' else 5):
                v=subprocess.run([native,str(scene),out/f'pixels-{scene}.hex'],capture_output=True,text=True)
                checks.append(v.stdout);print(v.stdout,end='')
                if v.returncode:raise SystemExit(label+' pixels failed')
        (out/(label+'-pixels.log')).write_text(''.join(checks))
        record['cases'].append(dict(name=label,args=args,exit=r.returncode))
        (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    if a.board:
        # Prove the new upstream-of-width-adapter drain condition is necessary.
        top=src/'RasterIX/RasterIX/RasterIX_EF.v'
        original_top=top.read_text()
        guard=' && !commonReadsPending'
        assert original_top.count(guard)==1
        try:
            top.write_text(original_top.replace(guard,''))
            mutant_cmd=cmd.copy()
            mutant_cmd[mutant_cmd.index('--Mdir')+1]=str(out/'verilated-mutant')
            mutant_build=subprocess.run(mutant_cmd,capture_output=True,text=True)
            (out/'command-drain-mutation-build.log').write_text((mutant_build.stdout+mutant_build.stderr).replace(str(ROOT)+'/', ''))
            if mutant_build.returncode:raise SystemExit('Command drain mutant did not compile')
            mutant=subprocess.run([str(out/'verilated-mutant/Vfull_renderer_test'),'+ddr=1','+unaligned=1'],cwd=out,capture_output=True,text=True,timeout=180)
            log=(mutant.stdout+mutant.stderr).replace(str(ROOT)+'/', '')
            (out/'command-drain-mutation.log').write_text(log)
            if not mutant.returncode or 'Unread DDR padding acknowledged' not in log:
                raise SystemExit('Missing command drain was not detected')
            record['cases'].append(dict(name='command-drain-mutation',detected=True,exit=mutant.returncode))
            (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
            print('PASS command-read drain mutation detected')
        finally:
            top.write_text(original_top)
if __name__=='__main__':main()
