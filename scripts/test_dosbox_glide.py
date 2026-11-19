#!/usr/bin/env python3
"""Exercise the experimental encoder; never connects to the Amiga.

--firmware checks streams against the actual XACP main.c validator.
--arm also compiles Cortex-A9 soft and softfp/VFP objects (not execution).
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'amiga/dosbox/glide'
OUT = ROOT / '.context/amiga/glide'


def run(*args):
    subprocess.run([str(x) for x in args], cwd=ROOT, check=True)


def firmware_test(path, flags, objects):
    text = path.read_text()
    start = text.index('typedef struct {\n    uint32_t color_base;')
    end = text.index('static int xacpgpu_fifo_send_be', start)
    # Compile upstream's unchanged validator, not a reimplementation of it.
    validator = text[start:end]
    defines = '\n'.join(re.findall(r'^#define XACPGPU_.*$', text, re.M))
    harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "amiga/dosbox/glide/translator.h"
#include "amiga/dosbox/glide/probe_scene.h"
static uint32_t be32(uint32_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
 return __builtin_bswap32(x);
#else
 return x;
#endif
}
'''
    harness += defines + '\n' + validator + r'''
int main(void) {
 unsigned runs=0;
 for(unsigned w=1;w<=256;w*=2)for(unsigned h=1;h<=256;h*=2) {
  if(w>8*h||h>8*w)continue;
  for(unsigned blend=0;blend<3;blend++)for(unsigned origin=0;origin<2;origin++) {
   grx_config c={640,480,0x06600000,w,h,0x066a0000,(grx_origin)origin,(grx_blend)blend,0};
   grx_packet p;assert(grx_begin(&p,&c)==GRX_OK);
   grx_vertex v[3]={{0,0,255,0,0,255,0,0,1},
    {639,0,0,255,0,255,128,0,.5f},{0,479,0,0,255,255,0,64,.25f}};
   while(grx_triangle(&p,v)==GRX_OK){}
   assert(grx_end(&p)==GRX_OK);
   uint32_t wire[GRX_PACKET_WORDS];memcpy(wire,p.bytes,p.words*4);
   XACPGPU_SafeState st={0};uint32_t bad=0,detail=0;
   int rc=xacpgpu_validate_submit(wire,p.words,&st,&bad,&detail);
   if(rc)fprintf(stderr,"validator rc=%x word=%u detail=%x\n",rc,bad,detail);
   assert(rc==0);assert(st.color_base==c.color_address&&st.res_x==640&&st.res_y==480);
   /* Verify this is really the restricted ABI: depth enable must fail. */
   wire[3]=be32(be32(wire[3])|4);
   assert(xacpgpu_validate_submit(wire,p.words,&st,&bad,&detail)==0x4932);
   runs++;
  }
 }
 printf("PASS upstream XACP validator: %u texture/aspect/origin/blend batches and depth rejection\n",runs);
 for(unsigned mode=0;mode<2;mode++)for(unsigned scene=0;scene<GRX_PROBE_SCENES;scene++) {
  grx_packet p;assert(mode?grx_probe_triangle_marker(&p,scene):grx_probe_scene(&p,scene,0x4120+scene));
  uint32_t wire[GRX_PACKET_WORDS];memcpy(wire,p.bytes,p.words*4);
  XACPGPU_SafeState st={0};uint32_t bad=0,detail=0;
  assert(xacpgpu_validate_submit(wire,p.words,&st,&bad,&detail)==0);
 }
 puts("PASS upstream XACP validator: ten offscreen fixtures with completion markers");
}
'''
    source = OUT / 'firmware_validator.c'
    source.write_text(harness)
    binary = OUT / 'test-firmware'
    run('clang', *flags, '-I.', source, *objects, '-o', binary)
    run(binary)
    return {'main_c_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
            'validator_sha256': hashlib.sha256(validator.encode()).hexdigest()}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--arm', action='store_true')
    parser.add_argument('--firmware', type=Path)
    args = parser.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-O1', '-g', '-fsanitize=address,undefined']
    objects = []
    sources = [SRC/'translator.c', SRC/'descriptor.c', SRC/'probe_scene.c']
    for source in sources:
        obj = OUT / (source.stem+'.o')
        run('clang', *flags, '-c', source, '-o', obj)
        objects.append(obj)
    binary = OUT/'test'
    run('clang++', '-std=c++17', *flags[1:], '-I.',
        'tests/amiga/dosbox/glide/translator.cpp', *objects, '-o', binary)
    run(binary)
    readback = OUT/'test-readback'
    for height in (65, 72):
        run('clang', *flags, '-I.', '-DGRX_PROBE_HEIGHT='+str(height),
            'tests/amiga/dosbox/glide/readback.c', '-o', readback)
        run(readback)
    record = {'scope': 'Native encoder tests; no physical GPU or game execution',
              'native_sanitizers': ['address', 'undefined']}
    if args.firmware:
        record['upstream_validator'] = firmware_test(args.firmware, flags, objects)
    if args.arm:
        for abi in ('soft', 'softfp'):
            for source in sources:
                run('arm-none-eabi-gcc', '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror',
                    '-mcpu=cortex-a9', '-marm', '-mfpu=vfpv3', '-mfloat-abi='+abi,
                    '-ffreestanding', '-fno-builtin', '-c', source,
                    '-o', OUT/(source.stem+'.'+abi+'.o'))
        record['arm_compile_only'] = ['cortex-a9 ARM32 soft', 'cortex-a9 ARM32 softfp VFPv3']
    inputs = [*sources, SRC/'descriptor.h', SRC/'translator.h', SRC/'vertex.h', SRC/'probe_scene.h', SRC/'pipeline.h', SRC/'probe_wire.h',
              ROOT/'tests/amiga/dosbox/glide/translator.cpp', Path(__file__)]
    record['source_sha256'] = {str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}
    (OUT/'validation.json').write_text(json.dumps(record, indent=2)+'\n')
    print('Record:', OUT/'validation.json')


if __name__ == '__main__':
    main()
