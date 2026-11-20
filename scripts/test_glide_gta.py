#!/usr/bin/env python3
"""GTA subset and Core0 channel tests; no game data or hardware required."""
import argparse, subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
src=root/'amiga/dosbox/glide';tests=root/'tests/amiga/dosbox/glide'
out=root/'.context/amiga/glide/gta-tests';out.mkdir(parents=True,exist_ok=True)
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path);p.add_argument('--firmware',type=Path);args=p.parse_args()
validator=[]
if args.firmware:
 from glide_firmware import validator_source
 defines,body=validator_source(args.firmware)
 v=out/'validator.c'
 v.write_text('#include <stdint.h>\n#include <string.h>\n#include "translator.h"\nstatic uint32_t be32(uint32_t x){return __builtin_bswap32(x); }\n'+defines+'\n'+body+'''
int grx_check_firmware(const grx_packet *p){
 uint32_t wire[GRX_PACKET_WORDS],bad,detail;XACPGPU_SafeState state;
 memcpy(wire,p->bytes,p->words*4);
 return xacpgpu_validate_submit(wire,p->words,&state,&bad,&detail)==0;
}
''')
 validator=['-DGRX_REAL_VALIDATOR',str(v)]
base=['gta.c','draw_stream.c','guest_vertex.c','clip.c','translator.c','descriptor.c','guest.c']
for name,sources in [('channel',[]),('stream',['experimental/stream.c']),('gta_texture',['translator.c','descriptor.c']),('gta',base)]+([('gta_replay',base)] if args.trace else []):
 exe=out/name
 subprocess.run(['clang','-std=c11','-O2','-ffp-contract=off','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
 '-I'+str(src),'-I'+str(src/'experimental'),'-I'+str(root/'.context/amiga/glide/ovl'),
 str(tests/(name+'.c')),*[str(src/s) for s in sources],*(validator if name in ('gta','gta_replay') else []),'-lm','-o',str(exe)],check=True)
 subprocess.run([str(exe),*([str(args.trace.resolve())] if name=='gta_replay' else [])],check=True)
