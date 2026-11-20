#!/usr/bin/env python3
"""Validate the larger fixed workloads offline against the actual firmware parser."""
import argparse,hashlib,json,subprocess
from pathlib import Path
from test_dosbox_glide_draw import firmware_adapter,OUT as DRAW_OUT
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--firmware',type=Path,required=True)
 a=p.parse_args();out=ROOT/'.context/amiga/glide/workload';out.mkdir(parents=True,exist_ok=True)
 DRAW_OUT.mkdir(parents=True,exist_ok=True)
 subprocess.run(['python3',str(ROOT/'scripts/build_dosbox_glide_ovl.py'),'--generate-only'],check=True)
 src=ROOT/'amiga/dosbox/glide';test=ROOT/'tests/amiga/dosbox/glide/workload.c'
 sources=[test,*[src/n for n in ['experimental/workload.c','draw_stream.c','guest_vertex.c','clip.c','translator.c','descriptor.c']]]
 args=['clang','-std=c11','-O1','-ffp-contract=off','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
       '-I'+str(src),'-I'+str(src/'experimental'),'-I'+str(ROOT/'.context/amiga/glide/ovl'),
       *map(str,sources),str(firmware_adapter(a.firmware)),'-o',str(out/'workload-test')]
 subprocess.run(args,check=True)
 result=subprocess.run([str(out/'workload-test')],capture_output=True,text=True)
 (out/'test.log').write_text(result.stdout+result.stderr);result.check_returncode();print(result.stdout,end='')
 paths=[*sources,src/'experimental/workload.h',Path(__file__)]
 (out/'result.json').write_text(json.dumps({'scope':'Offline packet and verifier checks; not GPU pixels or FPS',
  'packets_validated':130,'profiles':['320x240','640x480'],'triangles_per_frame':320,
  'source_sha256':{str(n.relative_to(ROOT)):hashlib.sha256(n.read_bytes()).hexdigest() for n in paths},
  'firmware_sha256':hashlib.sha256(a.firmware.read_bytes()).hexdigest()},indent=2)+'\n')
if __name__=='__main__':main()
