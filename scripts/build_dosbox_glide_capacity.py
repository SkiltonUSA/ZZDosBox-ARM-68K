#!/usr/bin/env python3
"""Build and validate an isolated A9 encoder benchmark. Never deploys or runs it."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
from build_zz9000_debug import unpack_elf

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/capacity'
SRC=Path('amiga/dosbox/glide')
COMMON=Path('amiga/common/arm')
ZZ=Path('amiga/arm_debug/zz9000')

def run(args):
    subprocess.run(list(map(str,args)),cwd=ROOT,check=True)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--connection',default='podman-machine-default-root')
    p.add_argument('--arm-only',action='store_true')
    a=p.parse_args();OUT.mkdir(parents=True,exist_ok=True)
    kernel=[SRC/n for n in ['capacity.c','translator.c','descriptor.c']]
    test=Path('tests/amiga/dosbox/glide/capacity.c')
    run(['clang','-std=c11','-O2','-g','-Wall','-Wextra','-Werror',
         '-fsanitize=address,undefined','-ffp-contract=off','-I.',test,*kernel,'-o',OUT/'native'])
    native=subprocess.check_output([OUT/'native'],text=True)
    (OUT/'native.log').write_text(native);print(native,end='')
    match=re.fullmatch(r'CAPACITY triangles=320 bytes=57344 hash=([0-9a-f]{8})\n',native)
    assert match,native
    flags=['-O2','-g','-Wall','-Wextra','-Werror','-mcpu=cortex-a9','-marm',
           '-mfpu=vfpv3','-ffp-contract=off','-mno-unaligned-access',
           '-ffreestanding','-fno-builtin','-fPIC','-fvisibility=hidden','-fno-stack-protector',
           '-DZZ_BLOCK_SIZE=0x100000','-DZZ_CONTROL=0x80000','-DMB_MMU','-DMB_DCACHE',
           '-Iamiga/arm_debug','-I'+str(ZZ),'-I'+str(COMMON),'-I'+str(SRC)]
    sources=[*kernel,SRC/'capacity_worker.c',COMMON/'worker.cpp',COMMON/'mmu.c',
             COMMON/'mmu_asm.S',COMMON/'memory.c',Path('amiga/dosbox/pc/neon_state.S')]
    objects=[]
    for i,src in enumerate(sources):
        obj=OUT/(str(i)+'-'+src.stem+'.o');objects.append(obj)
        cpp=src.suffix=='.cpp'
        extra=['-std=c++14','-fno-exceptions','-fno-rtti','-fno-threadsafe-statics'] if cpp else []
        # Only the guarded kernel may use VFP. Entry/MMU/state wrappers stay soft.
        abi='softfp' if src in kernel else 'soft'
        run(['arm-none-eabi-g++' if cpp else 'arm-none-eabi-gcc',*flags,
             '-mfloat-abi='+abi,*extra,'-c',src,'-o',obj])
    elf=OUT/'capacity.elf'
    run(['arm-none-eabi-ld','-shared','-Bsymbolic','--no-undefined','-T',SRC/'capacity.ld',*objects,'-o',elf])
    payload,relocations,entry=unpack_elf(elf.read_bytes(),0x40000)
    run(['arm-none-eabi-gcc',*flags,'-mfloat-abi=soft','-c',ZZ/'mapping_probe.S','-o',OUT/'mapping.o'])
    run(['arm-none-eabi-ld','-shared','-Bsymbolic','--no-undefined','-T',ZZ/'payload.ld',OUT/'mapping.o','-o',OUT/'mapping.elf'])
    mapping,rel,point=unpack_elf((OUT/'mapping.elf').read_bytes())
    assert not rel and not point and len(mapping)<=1024
    def array(name,data):return 'static const unsigned char '+name+'[]={'+','.join(map(str,data))+'};\n'
    (OUT/'zz_payload.h').write_text(f'#define ZZ_ENTRY {entry}u\n'+array('zz_image',payload)+
        array('zz_mapping_probe',mapping)+'static const unsigned long zz_relocations[]={'+','.join(map(str,relocations))+'};\n')
    (OUT/'expected.h').write_text('#define MESEN_EXPECT_CYCLES 320u\n#define MESEN_EXPECT_HASH 0x'+match[1]+'u\n')
    host=(ROOT/COMMON/'host.c').read_text().replace('8991','320').replace('instructions=320','triangles=320')
    host=host.replace('MESEN CPU-ONLY','GLIDE ENCODER ONLY').replace('cycles=','triangles=')
    host=host.replace('MESEN_CPU PASS; CPU-only throughput, NOT emulated video FPS',
                      'GLIDE_CAPACITY PASS; encoding only; no GPU, guest emulation or game FPS')
    # Calibrate instead of assuming the stock A9 clock; accept held 800 MHz too.
    host=host.replace('rate<320000000u || rate>345000000u','rate<300000000u || rate>450000000u')
    (OUT/'host.c').write_text(host)
    image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
    if not a.arm_only:
        run(['podman','--connection',a.connection,'run','--rm','--platform','linux/amd64',
             '-v',f'{ROOT}:/work','-w','/work',image,'m68k-amigaos-gcc','-std=c99','-noixemul',
             '-m68020','-O2','-g','-Wall','-Wextra','-Werror','-DZZ_HEADLESS_BENCH','-DZZ_RELEASE',
             '-DZZ_BLOCK_SIZE=0x100000','-DZZ_CONTROL=0x80000','-DMB_MMU','-DMB_DCACHE',
             '-DZZ_HEADLESS_MMU','-DZZ_HEADLESS_ICACHE','-DZZ_APP_NAME="ZZGlide-Capacity"',
             '-DZZ_APP_VERSION="0.1"','-DZZ_MIN_STACK=65536','-Iamiga/arm_debug',
             '-I'+str(ZZ),'-I'+str(COMMON),'-I'+str(OUT.relative_to(ROOT)),
             ZZ/'launcher.c',(OUT/'host.c').relative_to(ROOT),'-lamiga','-o',
             (OUT/'ZZGlide-Capacity').relative_to(ROOT)])
    inputs=[*sources,*SRC.glob('*.h'),SRC/'capacity.ld',COMMON/'host.c',COMMON/'wire.h',
            COMMON/'mmu.h',COMMON/'mmu_plan.h',COMMON/'bench.h',ZZ/'launcher.c',ZZ/'layout.h',
            ZZ/'mapping_probe.S',Path('amiga/arm_debug/protocol.h'),test,Path(__file__).relative_to(ROOT)]
    record={'scope':'Physical-build candidate: encoder only, no GPU/guest/game FPS',
        'triangles':320,'batches':64,'packet_bytes':57344,'expected_hash':match[1],
        'image_bytes':len(payload),'relocations':len(relocations),'container_image':image,
        'arm_flags':flags,'compiler':subprocess.check_output(['arm-none-eabi-gcc','--version'],text=True),
        'cache_policy':'Private MMU/I/D on; outer and shared noncacheable; VFP state preserved',
        'source_sha256':{str(f):hashlib.sha256((ROOT/f).read_bytes()).hexdigest() for f in inputs},
        'outputs':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in [elf,OUT/'host.c',OUT/'native.log']}}
    if not a.arm_only:record['outputs']['ZZGlide-Capacity']=hashlib.sha256((OUT/'ZZGlide-Capacity').read_bytes()).hexdigest()
    (OUT/'build.json').write_text(json.dumps(record,indent=2)+'\n')
    print('Built encoder payload',len(payload),'bytes;',len(relocations),'relocations')

if __name__=='__main__':main()
