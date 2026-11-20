#!/usr/bin/env python3
"""Compare a carry-free texture-address candidate against the actual sampler."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/texture-address'

def candidate(source):
    old='<= step0_offset[0 +: 17] + (('
    if source.count(old)!=4:raise ValueError('Unexpected texture-address expressions')
    return source.replace(old,'<= step0_offset[0 +: 17] | ((')

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--renderer',type=Path,required=True)
    a=p.parse_args();r=a.renderer.resolve();OUT.mkdir(parents=True,exist_ok=True)
    sampler=r/'rtl/RasterIX/TextureSampler.v';source=sampler.read_text()
    # Exhaust all dimension/LOD fields. The coordinate mask covers every possible
    # value of either clamped 8-bit coordinate, including unsupported dimensions.
    for tw in range(16):
      for th in range(16):
        wm=((1<<tw)-1)&511;hm=((1<<th)-1)&511
        mask=sum((((wm>>i)^(hm>>i))&1)<<(16-2*i) |
                 (((wm>>i)|(hm>>i))&1)<<(17-2*i) for i in range(9))
        shift=18-(tw+th+1);mf=mask>>shift if shift>=0 else 0
        for lod in range(16):
            w=max(tw-lod,0);h=max(th-lod,0)
            offset=mf&(~((1<<(w+h+1))-1)&0x3ffff)
            xy=(((255>>((8-h)&15))<<w)|(255>>((8-w)&15)))&0x1ffff
            assert not(offset&xy),(tw,th,lod)
    fast=candidate(source)
    (OUT/'TextureSampler.v').write_text(fast)
    (OUT/'Reference.v').write_text(source.replace('module TextureSampler #(', 'module TextureSamplerReference #('))
    tb='''module texture_address_tb;
reg clk=0;always #5 clk=~clk;
reg resetn=0,ready=0,valid=0,halfOffset=0,clampS=0,clampT=0;
reg [3:0] width=0,height=0,lod=0;
reg [31:0] s=0,t=0;
reg [7:0] user=0;
wire [67:0] refAddr,newAddr;
wire [169:0] refOut,newOut;
'''
    # Outputs include all returned texels, coordinates, user data and handshake.
    for module,prefix in [('TextureSamplerReference','ref'),('TextureSampler','new')]:
        tb+=f'''{module} #(.USER_WIDTH(8)) {prefix}Dut(
.aclk(clk),.resetn(resetn),.textureSizeWidth(width),.textureSizeHeight(height),
.enableHalfPixelOffset(halfOffset),.s_valid(valid),.m_ready(ready),.s_user(user),
.s_texelS(s),.s_texelT(t),.s_clampS(clampS),.s_clampT(clampT),.s_textureLod(lod),
.texelAddr00({prefix}Addr[16:0]),.texelAddr01({prefix}Addr[33:17]),
.texelAddr10({prefix}Addr[50:34]),.texelAddr11({prefix}Addr[67:51]),
.texelInput00(32'h12345678),.texelInput01(32'h98765432),
.texelInput10(32'hdeadbeef),.texelInput11(32'h0badf00d),
.m_texel00({prefix}Out[31:0]),.m_texel01({prefix}Out[63:32]),
.m_texel10({prefix}Out[95:64]),.m_texel11({prefix}Out[127:96]),
.m_texelSubCoordS({prefix}Out[143:128]),.m_texelSubCoordT({prefix}Out[159:144]),
.m_user({prefix}Out[167:160]),.m_valid({prefix}Out[168]),.s_ready({prefix}Out[169]));
'''
    tb+='''integer cfg,cycle;reg [31:0] rng=32'h1234abcd;
task nextRandom;begin rng=rng^(rng<<13);rng=rng^(rng>>17);rng=rng^(rng<<5);end endtask
initial begin
for(cfg=0;cfg<4096;cfg=cfg+1) begin
 for(cycle=0;cycle<64;cycle=cycle+1) begin
  @(negedge clk);
  width=cfg[3:0];height=cfg[7:4];lod=cfg[11:8];resetn=1;
  nextRandom();s=rng;nextRandom();t=rng;
  if(cycle<8)begin s=32'h8000*cycle-1;t=32'h8000*(7-cycle);end
  nextRandom();ready=rng[0]|rng[1];valid=rng[2];halfOffset=rng[3];
  clampS=rng[4];clampT=rng[5];user=rng[15:8];
  @(posedge clk);#1;
  if(refAddr!==newAddr || refOut!==newOut)
   $fatal(1,"Sampler mismatch config=%0d cycle=%0d reference=%h candidate=%h",cfg,cycle,refAddr,newAddr);
 end
end
$display("PASS texture address: 4096 configurations, 262144 cycles, unchanged latency and backpressure");
$finish;
end
endmodule
'''
    (OUT/'tb.sv').write_text(tb)
    results={}
    for label in ('reference','mutant'):
        if label=='mutant':
            (OUT/'TextureSampler.v').write_text(fast.replace('step0_offset[0 +: 17] |','(step0_offset[0 +: 17] ^ 17\'h1) |',1))
        cmd=['verilator','--binary','--timing','-Wno-fatal','--top-module','texture_address_tb',
            '-I'+str(r/'rtl/RasterIX'),'--Mdir',str(OUT/label),str(OUT/'tb.sv'),
            str(OUT/'Reference.v'),str(OUT/'TextureSampler.v'),str(r/'rtl/Float/rtl/float/ValueDelay.v')]
        build=subprocess.run(cmd,capture_output=True,text=True)
        (OUT/(label+'-build.log')).write_text(build.stdout+build.stderr)
        if build.returncode:raise SystemExit(build.stderr[-2000:])
        run=subprocess.run([OUT/label/'Vtexture_address_tb'],capture_output=True,text=True)
        (OUT/(label+'.log')).write_text(run.stdout+run.stderr)
        if (run.returncode==0)!=(label=='reference'):raise SystemExit('Unexpected result '+label)
        results[label]=run.returncode
    (OUT/'TextureSampler.v').write_text(fast)
    record={'scope':'RTL equivalence candidate; no timing gain or physical FPS yet',
      'configurations':4096,'cycles':262144,'results':results,
      'source_sha256':hashlib.sha256(source.encode()).hexdigest(),
      'candidate_sha256':hashlib.sha256(fast.encode()).hexdigest(),
      'test_sha256':hashlib.sha256(tb.encode()).hexdigest()}
    (OUT/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    print((OUT/'reference.log').read_text().strip());print('PASS changed-address mutant rejected')
if __name__=='__main__':main()
