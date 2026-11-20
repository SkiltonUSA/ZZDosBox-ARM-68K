#!/usr/bin/env python3
"""Validate the full-address completion endpoint and detect address aliasing."""
import hashlib
import json
import subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/endpoint'
EXP=ROOT/'amiga/dosbox/glide/experimental'
BENCH=ROOT/'tests/amiga/dosbox/glide/completion_axil.v'


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    (OUT/'results.json').unlink(missing_ok=True)
    sources=[EXP/name for name in ['completion.v','completion_axil.v','response_token.v','completion_endpoint.v']]
    records=[]
    for name,endpoint,mutation in [('registers',False,False),('endpoint',True,False),('address-alias',True,True)]:
        inputs=list(sources)
        if mutation:
            mutant=OUT/'alias.v'
            text=sources[-1].read_text()
            assert text.count('.ADDR_WIDTH(32)')==1
            mutant.write_text(text.replace('.ADDR_WIDTH(32)','.ADDR_WIDTH(8)'))
            inputs[-1]=mutant
        command=['verilator','--binary','--timing','-j','4','-Wno-fatal','--top-module','completion_axil_test','--Mdir',str(OUT/name)]
        if endpoint:command+=['-DGRX_ENDPOINT_TEST']
        build=subprocess.run([*command,str(BENCH),*map(str,inputs)],capture_output=True,text=True)
        (OUT/(name+'-build.log')).write_text((build.stdout+build.stderr).replace(str(ROOT)+'/', ''))
        if build.returncode:raise SystemExit(name+' did not compile')
        result=subprocess.run([str(OUT/name/'Vcompletion_axil_test')],capture_output=True,text=True,timeout=60)
        log=(result.stdout+result.stderr).replace(str(ROOT)+'/', '')
        (OUT/(name+'.log')).write_text(log)
        if mutation:
            if not result.returncode or 'Wrong or unstable read' not in log:
                raise SystemExit('Address alias mutation was not detected')
        elif result.returncode or log.count('PASS AXI mailbox:')!=(11 if endpoint else 9):
            raise SystemExit(name+' failed')
        records.append({'name':name,'exit':result.returncode,'mutation_detected':mutation})
        print('PASS '+name)
    manifest={'cases':records,'sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__).resolve(),BENCH,*sources]}}
    (OUT/'results.json').write_text(json.dumps(manifest,indent=2)+'\n')


if __name__=='__main__':main()
