#!/usr/bin/env python3
"""Run prepared renderer/HP3 fixtures with AMD XSim under the existing Wine install."""
import argparse
import hashlib
import json
import os
import re
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]


def windows(path):
    return 'Z:'+str(path.resolve()).replace('/', '\\')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--prepared',required=True,type=Path)
    p.add_argument('--wine-prefix',required=True,type=Path)
    p.add_argument('--vivado',default='C:\\AMDDesignTools\\2025.2\\Vivado')
    p.add_argument('--wine',default='/opt/homebrew/bin/wine')
    a=p.parse_args();out=a.prepared.resolve()
    (out/'hp3-results.json').unlink(missing_ok=True)
    inputs=json.loads((out/'simulation-inputs.json').read_text())
    env=dict(os.environ,WINEPREFIX=str(a.wine_prefix.resolve()),WINEDEBUG='-all',MVK_CONFIG_LOG_LEVEL='0')

    def run(tool,args,label,timeout=300,allow_failure=False):
        command=[a.wine,'cmd','/c',a.vivado+'\\bin\\'+tool+'.bat',*args]
        result=subprocess.run(command,cwd=out,env=env,capture_output=True,text=True,timeout=timeout)
        text=result.stdout+result.stderr
        (out/(label+'.log')).write_text(text.replace(str(ROOT)+'/', '').replace(windows(ROOT)+'\\',''))
        if result.returncode and not allow_failure:raise SystemExit(label+' failed: '+text[-3500:])
        return text

    # XSim rejects declarations following an implicit-net use. Reorder only
    # the declaration in a simulation copy; retain both hashes for comparison.
    normalization={}
    fixes=[('FloatAdd.v','    wire [MANTISSA_ONE_POS_SIZE', ['two_mantissaSum']),
           ('IntToFloat.v','    LeadingOneFinder #(', ['one_number']),
           ('RasterIXRenderCore.v','    localparam TEX_ADDR_WIDTH',
            ['alrp_tready','alrp_tvalid','alrp_tpixel','fragment_stream_out_tvalid',
             'fragment_stream_out_tready','framebuffer_pfp_wvalid',
             'framebuffer_pfp_wready','framebuffer_bc_wvalid']),
           ('RasterIX_EF.v','    wire [DATA_WIDTH - 1 : 0] cmd_rcredit_rdata;', ['commandReadsPending'])]
    for name,anchor,names in fixes:
        original=next(Path(f) for f in inputs['sources'] if Path(f).name==name)
        text=original.read_text();declarations=[]
        for variable in names:
            matches=[line for line in text.splitlines() if re.match(r'^    (?:wire|reg)\b',line) and re.search(r'\b'+variable+r'\b',line)]
            assert len(matches)==1,(name,variable)
            declarations.append(matches[0]);text=text.replace(matches[0]+'\n','',1)
        assert text.count(anchor)==1
        normalized=out/('xsim-'+name)
        normalized.write_text(text.replace(anchor,'\n'.join(declarations)+'\n'+anchor,1))
        inputs['sources'][inputs['sources'].index(str(original))]=str(normalized)
        normalization[name]={'before':hashlib.sha256(original.read_bytes()).hexdigest(),
                             'after':hashlib.sha256(normalized.read_bytes()).hexdigest(),
                             'moved_declarations':names}
    glbl=a.wine_prefix/'drive_c'/a.vivado[3:].replace('\\','/')/'data/verilog/src/glbl.v'
    files=[Path(s) for s in inputs['sources']]+[glbl]
    (out/'hp3.prj').write_text('\n'.join(('sv' if f.name=='full_renderer.v' else 'verilog')+' work "'+windows(f)+'"' for f in files)+'\n')
    args=['--prj','hp3.prj']
    for include in inputs['includes']:args+=['-i',windows(Path(include))]
    for define in inputs['defines']:args+=['-d',define]
    run('xvlog',args,'xvlog')
    run('xelab',['work.full_renderer_test','work.glbl','-s','hp3','--debug','typical','--timescale','1ns/1ps'],'xelab')
    cases=[('normal',[]),('delayed',['b_delay=37','r_delay=3']),('cancel',['cancel=1']),
           ('ddr',['ddr=1']),('ddr-delayed',['ddr=1','b_delay=37','r_delay=3']),
           ('ddr-cancel',['ddr=1','cancel=1']),('write-error',['error=1']),
           ('read-error',['error=2']),('ddr-unaligned',['ddr=1','unaligned=1']),
           ('unsafe-drain',['unsafe=1','b_delay=37','r_delay=3'])]
    records=[]
    for label,plusargs in cases:
        # cmd.exe treats '=' as a batch-argument separator. Pass simulator
        # options through a file so delay/error controls reach the test intact.
        options=out/(label+'.opts')
        options.write_text('--runall\n'+''.join('--testplusarg '+arg+'\n' for arg in plusargs))
        log=run('xsim',['hp3','--file',options.name],label,allow_failure=label=='unsafe-drain')
        if label=='unsafe-drain':
            if 'Acknowledged outstanding memory transactions' not in log:
                raise SystemExit('Unsafe completion was not detected')
            records.append({'name':label,'mutation_detected':True});continue
        if 'Fatal:' in log or 'Error:' in log:raise SystemExit(label+' failed: '+log[-3000:])
        expected={'write-error':1,'read-error':3,'ddr-unaligned':1}.get(label,5)
        if log.count('PASS full renderer:')!=expected:
            raise SystemExit('Missing scene results: '+label+' '+log[-2000:])
        checks=[]
        if label not in ('write-error','ddr-unaligned'):
            for scene in range(2 if label=='read-error' else 5):
                check=subprocess.run([str(out/'fixtures'),str(scene),str(out/f'pixels-{scene}.hex')],capture_output=True,text=True)
                checks.append(check.stdout)
                (out/(label+'-pixels.log')).write_text(''.join(checks))
                if check.returncode:
                    raise SystemExit(f'{label} scene {scene} pixel check failed ({check.returncode}): '+check.stdout+check.stderr)
        (out/(label+'-pixels.log')).write_text(''.join(checks))
        records.append({'name':label,'framebuffers':len(checks),'traffic':[line for line in log.splitlines() if line.startswith('HP3 traffic:')]})
        print('PASS '+label,flush=True)
    sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
    report={'scope':'Generated AXI4-to-AXI3 converter with board renderer and behavioural RAM; no PS7/physical DDR/FPS',
      'cases':records,'ordered_state':inputs['ordered_state'],
      'source_sha256':inputs['source_sha256'],'normalization':normalization,
      'input_sha256':{str(f.relative_to(ROOT)) if f.is_relative_to(ROOT) else f.name:sha(f) for f in files+[Path(h) for h in inputs.get('headers',[])]},
      'harness_sha256':{name:sha(ROOT/name) for name in ['scripts/test_dosbox_glide_hp3.py','scripts/glide_hp3_fixture.py','scripts/test_dosbox_glide_full_renderer.py']},
      'generated_sha256':{name:sha(out/name) for name in ['gpu_wires.vh','scene_sizes.vh','commands.hex']}}
    (out/'hp3-results.json').write_text(json.dumps(report,indent=2)+'\n')


if __name__=='__main__':main()
