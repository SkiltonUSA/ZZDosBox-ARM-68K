#!/usr/bin/env python3
"""Verify completion-driven counting; optionally build its standalone 68k UI."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
EXP=ROOT/'amiga/dosbox/glide/experimental'
OUT=ROOT/'.context/amiga/glide/measure'
def run(args,check=True):
    return subprocess.run(list(map(str,args)),cwd=ROOT,capture_output=True,text=True,check=check)
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--host',action='store_true')
    p.add_argument('--connection',default='podman-machine-default-root')
    a=p.parse_args();OUT.mkdir(parents=True,exist_ok=True)
    test=ROOT/'tests/amiga/dosbox/glide/measure.c'
    flags=['-std=c11','-O2','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I.', '-I'+str(EXP)]
    for name,source in [('reference',EXP/'measure.c'),('counts-pending',OUT/'mutant.c')]:
        if name!='reference':
            source.write_text((EXP/'measure.c').read_text().replace(
                'if (result==GRX_COMPLETION_PENDING) {','if (result==GRX_COMPLETION_PENDING) { ++m->frames;'))
        run(['clang',*flags,test,source,EXP/'completion_client.c','-o',OUT/name])
        result=run([OUT/name],check=False)
        (OUT/(name+'.log')).write_text(result.stdout+result.stderr)
        if (result.returncode==0)!=(name=='reference'):raise SystemExit('Unexpected counter test: '+name)
    print((OUT/'reference.log').read_text().strip())
    print('PASS mutation: counting pending submissions is rejected')
    run(['arm-none-eabi-gcc','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-ffreestanding','-O2',
         '-Wall','-Wextra','-Werror','-c',EXP/'measure.c','-o',OUT/'measure.o'])
    sources=[test,EXP/'measure.c',EXP/'measure.h',EXP/'measure_host.c',
             EXP/'completion_client.c',EXP/'completion_client.h',Path(__file__)]
    record={'scope':'Native counter tests and ARM compile; no physical FPS',
            'pending_count_mutant_rejected':True,'source_sha256':{
                str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest() for s in sources}}
    if a.host:
        image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
        result=run(['podman','--connection',a.connection,'run','--rm','--platform','linux/amd64',
            '-v',f'{ROOT}:/work','-w','/work',image,'m68k-amigaos-gcc','-std=c99','-noixemul',
            '-m68020','-O2','-Wall','-Wextra','-Werror',EXP.relative_to(ROOT)/'measure_host.c',
            '-lamiga','-o',OUT.relative_to(ROOT)/'ZZGlide-Measure'])
        (OUT/'host-build.log').write_text(result.stdout+result.stderr)
        binary=OUT/'ZZGlide-Measure'
        record['host']={'image':image,'bytes':binary.stat().st_size,
                        'sha256':hashlib.sha256(binary.read_bytes()).hexdigest()}
        print('Built ZZGlide-Measure (68020, no host FPU required)')
    (OUT/'results.json').write_text(json.dumps(record,indent=2)+'\n')
if __name__=='__main__':main()
