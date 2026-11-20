#!/usr/bin/env python3
"""Guest triangle batching, optional real validator and A9 instruction execution.
No hardware connection. --arm requires Unicorn and arm-none-eabi tools.
"""
import argparse,hashlib,json,re,subprocess
from pathlib import Path
from glide_firmware import validator_source
ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'amiga/dosbox/glide'
OUT=ROOT/'.context/amiga/glide/draw'
TEST=ROOT/'tests/amiga/dosbox/glide/draw_stream.c'
SOURCES=[SRC/x for x in ('draw_stream.c','guest_vertex.c','clip.c','translator.c','descriptor.c')]

def run(args):
    result=subprocess.run([str(a) for a in args],cwd=ROOT,capture_output=True,text=True)
    if result.returncode:
        print(result.stdout+result.stderr,flush=True)
        result.check_returncode()
    return result.stdout

def firmware_adapter(path):
    defines,body=validator_source(path)
    source=OUT/'validator.c'
    source.write_text('#include <stdint.h>\n#include <string.h>\n#include "translator.h"\n'+'''
static uint32_t be32(uint32_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
 return __builtin_bswap32(x);
#else
 return x;
#endif
}
'''+defines+'\n'+body+'''
int grx_check_firmware(const grx_packet *p) {
 uint32_t wire[GRX_PACKET_WORDS],bad,detail; XACPGPU_SafeState state={0};
 memcpy(wire,p->bytes,p->words*4);
 return xacpgpu_validate_submit(wire,p->words,&state,&bad,&detail)==0;
}
''')
    return source

def arm_model(includes,native_digest):
    from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
    from unicorn.arm_const import (UC_CPU_ARM_CORTEX_A9,UC_ARM_REG_C1_C0_2,
        UC_ARM_REG_FPEXC,UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC,UC_ARM_REG_R0)
    stubs=OUT/'model_stubs.c'
    stubs.write_text('''#include <stddef.h>
volatile unsigned assertion_line;
void __assert_func(const char *f,int line,const char *fn,const char *e)
{(void)f;(void)fn;(void)e;assertion_line=line;for(;;){}}
int puts(const char *p){(void)p;return 0;}
int printf(const char *p,...){(void)p;return 0;}
void *memcpy(void *d,const void *s,size_t n){unsigned char *a=d;const unsigned char *b=s;while(n--)*a++=*b++;return d;}
void *memset(void *d,int v,size_t n){unsigned char *a=d;while(n--)*a++=v;return d;}
int memcmp(const void *a,const void *b,size_t n){const unsigned char *x=a,*y=b;while(n--){if(*x!=*y)return *x-*y;x++;y++;}return 0;}
''')
    results={}
    for abi in ('soft','softfp'):
        elf=OUT/(abi+'.elf');binary=OUT/(abi+'.bin')
        run(['arm-none-eabi-gcc','-std=c11','-O2','-ffp-contract=off','-fno-builtin','-nostdlib','-mcpu=cortex-a9','-marm',
             '-mfpu=vfpv3','-mfloat-abi='+abi,*includes,TEST,*SOURCES,stubs,
             '-Wl,-Ttext=0x100000','-Wl,-e,main','-lgcc','-o',elf])
        run(['arm-none-eabi-objcopy','-O','binary',elf,binary])
        symbols={p[2]:int(p[0],16) for line in run(['arm-none-eabi-nm',elf]).splitlines() if len(p:=line.split())==3}
        u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
        u.mem_map(0x100000,0x700000);u.mem_write(0x100000,binary.read_bytes())
        u.reg_write(UC_ARM_REG_C1_C0_2,0xf00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
        u.reg_write(UC_ARM_REG_SP,0x7ff000);u.reg_write(UC_ARM_REG_LR,0x7fe000)
        u.emu_start(symbols['main'],0x7fe000,count=1000000000)
        line=int.from_bytes(u.mem_read(symbols['assertion_line'],4),'little')
        assert not line,(abi,'assertion line',line)
        assert u.reg_read(UC_ARM_REG_PC)==0x7fe000,(abi,'instruction budget exhausted')
        assert u.reg_read(UC_ARM_REG_R0)==0
        digest=int.from_bytes(u.mem_read(symbols['draw_digest'],4),'little')
        assert digest==native_digest,(abi,hex(digest),hex(native_digest))
        results[abi]=f'{digest:08x}'
        print('PASS A9 instruction execution',abi,'packet digest',results[abi],flush=True)
    return results

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--firmware',type=Path);parser.add_argument('--arm',action='store_true')
    args=parser.parse_args();OUT.mkdir(parents=True,exist_ok=True)
    (OUT/'result.json').unlink(missing_ok=True)
    run(['python3','scripts/build_dosbox_glide_ovl.py','--generate-only'])
    includes=['-I'+str(SRC),'-I'+str(ROOT/'.context/amiga/glide/ovl')]
    flags=['clang','-std=c11','-Wall','-Wextra','-Werror','-O1','-ffp-contract=off','-g','-fsanitize=address,undefined',*includes]
    extra=['-DGRX_CHECK_FIRMWARE',firmware_adapter(args.firmware)] if args.firmware else []
    binary=OUT/'native'
    run([*flags,TEST,*SOURCES,*extra,'-o',binary])
    log=run([binary]);print(log,end='',flush=True);(OUT/'native.log').write_text(log)
    # A transport-rejection mutant must fail rather than falsely completing.
    mutant=OUT/'draw_mutant.c'
    mutant.write_text(SOURCES[0].read_text().replace('if(!s->submit(s->context,&s->packet))return GRX_BAD_STATE;',
                                                  '(void)s->submit(s->context,&s->packet);'))
    mutated=OUT/'mutant'
    run([*flags,TEST,mutant,*SOURCES[1:],'-o',mutated])
    result=subprocess.run([str(mutated)],capture_output=True,text=True)
    assert result.returncode!=0 and 'grx_draw_flush(&s)==GRX_BAD_STATE' in result.stderr,result.stderr
    (OUT/'mutant.log').write_text(result.stderr)
    print('PASS ignored transport-rejection mutation detected',flush=True)
    digest=int(re.search(r'DRAW_DIGEST=([0-9a-f]+)',log)[1],16)
    record={'scope':'Native and optional A9 model; not physical GPU, DOSBox runtime or GTA',
            'native_digest':f'{digest:08x}','retirement_rejection_mutant_detected':True,'fp_contraction':'off for native/A9 comparison',
            'firmware_validator_checked':bool(args.firmware)}
    if args.firmware:record['firmware_sha256']=hashlib.sha256(args.firmware.read_bytes()).hexdigest()
    if args.arm:
        record['arm_compiler']=run(['arm-none-eabi-gcc','--version']).splitlines()[0]
        record['arm_model']=arm_model(includes,digest)
    paths=[Path(__file__),TEST,*SOURCES,*sorted(SRC.glob('*.h')),
           ROOT/'.context/amiga/glide/ovl/guest_calls.h']
    record['sources']={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths if p.exists()}
    (OUT/'result.json').write_text(json.dumps(record,indent=2)+'\n')
if __name__=='__main__':main()
