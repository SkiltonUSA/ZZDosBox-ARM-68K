#!/usr/bin/env python3
"""Test private command preparation with the actual firmware validator."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
from glide_firmware import validator_source

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/tagged-submission'
SRC=ROOT/'amiga/dosbox/glide'


def run(args,label):
    result=subprocess.run(list(map(str,args)),cwd=ROOT,capture_output=True,text=True,timeout=120)
    log=(result.stdout+result.stderr).replace(str(ROOT)+'/', '')
    (OUT/(label+'.log')).write_text(log)
    if result.returncode:raise SystemExit(label+' failed:\n'+log[-2500:])
    return result.stdout


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--firmware',required=True,type=Path)
    p.add_argument('--board',type=Path,help='Override the sibling XX19d-RIX board design')
    a=p.parse_args(); OUT.mkdir(parents=True,exist_ok=True)
    (OUT/'results.json').unlink(missing_ok=True)
    defines,validator=validator_source(a.firmware)
    board=a.board or (a.firmware.resolve().parents[4]/'fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.srcs/sources_1/bd/zz9000_ps/zz9000_ps.bd')
    depth=int(json.loads(board.read_text())['design']['components']['gpu_cmd_fifo']['parameters']['C_TX_FIFO_DEPTH']['value'])
    prelude='''#include <stdint.h>
static uint32_t be32(uint32_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
 return __builtin_bswap32(value);
#else
 return value;
#endif
}
'''+defines+'\n'+validator+'\n#define GRX_BOARD_FIFO_WORDS '+str(depth)+'\n'
    harness=OUT/'harness.c'
    harness.write_text(prelude+'\n#include "tests/amiga/dosbox/glide/submit_prepare.c"\n')
    common=[SRC/name for name in ['translator.c','descriptor.c','probe_scene.c']]
    flags=['-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-I.', '-fsanitize=address,undefined']
    run(['clang',*flags,harness,*common,'-o',OUT/'test'],'build')
    log=run([OUT/'test',OUT/'commands.hex'],'test')
    oracle=ROOT/'tests/amiga/dosbox/glide/full_renderer_fixtures.c'
    run(['clang',*flags,oracle,*common,'-o',OUT/'oracle'],'oracle-build')
    expected=run([OUT/'oracle'],'oracle')
    if expected!=(OUT/'commands.hex').read_text():
        raise SystemExit('Prepared streams differ from existing full-renderer fixtures')
    # Compile the same validator/preparer pair for the eventual firmware build.
    arm=OUT/'arm.c'
    arm.write_text(prelude+'\n#include "amiga/dosbox/glide/experimental/submit_prepare.inc"\n'
      +'int grx_prepare_arm(GRX_TaggedSubmission *p,const volatile uint32_t *s,uint32_t n,uint32_t e,uint32_t q,uint32_t *b,uint32_t *d) {return grx_prepare_tagged(p,s,n,e,q,b,d);}\n')
    for abi in ['soft','softfp']:
        run(['arm-none-eabi-gcc','-std=c11','-O2','-Wall','-Wextra','-Werror','-I.',
          '-mcpu=cortex-a9','-marm','-mfpu=vfpv3','-mfloat-abi='+abi,'-ffreestanding',
          '-c',arm,'-o',OUT/(abi+'.o')],abi)
    sources=[SRC/'experimental/submit_prepare.inc',ROOT/'tests/amiga/dosbox/glide/submit_prepare.c',
             ROOT/'scripts/glide_firmware.py',Path(__file__),oracle,*common]
    sources += [SRC/name for name in ['probe_readback.h','probe_scene.h','translator.h',
                                      'vertex.h','descriptor.h','pipeline.h']]
    sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
    record={'scope':'Offline firmware preparation; no service, FIFO/MMIO, GPU execution or FPS',
      'main_c_sha256':sha(a.firmware),'validator_sha256':hashlib.sha256(validator.encode()).hexdigest(),
      'board_sha256':sha(board),'board_fifo_words':depth,'maximum_client_words':depth-7,
      'native_sanitizers':['address','undefined'],'matching_renderer_scenes':5,
      'commands_sha256':sha(OUT/'commands.hex'),'arm_compile_only':['soft','softfp'],
      'source_sha256':{str(s.relative_to(ROOT)):sha(s) for s in sources}}
    (OUT/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    print(log.strip());print('PASS byte-identical full-renderer scene streams; Cortex-A9 compile checks')


if __name__=='__main__':main()
