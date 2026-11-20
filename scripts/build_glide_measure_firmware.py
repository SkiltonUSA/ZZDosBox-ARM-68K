#!/usr/bin/env python3
"""Build-only fixed-scene GPU counter, preserving the recorded Core0 services."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
from glide_abort import patch_vectors

ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'amiga/dosbox/glide'
PIN={
 'ZZ9000OS/src/main.c':'c9ab713bbe81ce1891811a7bc2794ef93f30fd57e6e72553431c8b8e00978e5c',
 'ZZ9000OS/Makefile':'7b0c20764d2deec89548ea3b28b04afbe1071fbde7c691225e6a6f8f6b0716e5',
 'zz9000_bsp_new/ps7_cortexa9_0/lib/libxil.a':'229572878687511fddadb2e52663027d8733c511408c014e31807b867d744948'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def once(text,old,new):
    if text.count(old)!=1:raise ValueError('Unexpected source anchor: '+old)
    return text.replace(old,new)
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--base',type=Path,required=True)
    p.add_argument('--toolchain',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();base=a.base.resolve();out=a.output.resolve()
    for name,digest in PIN.items():
        if sha(base/name)!=digest:raise SystemExit('Unrecorded base: '+name)
    if out.exists() or out.is_relative_to(base):raise SystemExit('Use a fresh separate output directory')
    out.mkdir(parents=True)
    for name in ('ZZ9000OS','zz9000_bsp_new'):
        shutil.copytree(base/name,out/name,ignore=shutil.ignore_patterns('build','build-next-ay'))
    fw=out/'ZZ9000OS';dest=fw/'src/grx-measure';dest.mkdir()
    inputs=[SRC/name for name in ['translator.c','descriptor.c','probe_scene.c','translator.h',
            'descriptor.h','vertex.h','pipeline.h','probe_scene.h','probe_readback.h',
            'draw_stream.c','draw_stream.h','guest_vertex.c','guest_vertex.h','guest.h','clip.c','clip.h']]
    inputs+= [SRC/'experimental'/name for name in ['measure.h','measure.c','measure_firmware.inc',
        'submit_prepare.inc','completion_client.h','completion_client.c','mmio_guard.h',
        'mmio_guard.c','mmio_guard.S','mmio_transport.h','mmio_transport.c','workload.h','workload.c','stream.h','stream.c','stream_firmware.inc']]
    subprocess.run(['python3',str(ROOT/'scripts/build_dosbox_glide_ovl.py'),'--generate-only'],check=True)
    inputs.append(ROOT/'.context/amiga/glide/ovl/guest_calls.h')
    for source in inputs:shutil.copyfile(source,dest/source.name)
    tool=lambda n:str(a.toolchain.resolve()/('arm-none-eabi-'+n))
    flags=['-mcpu=cortex-a9','-marm','-mfpu=neon','-mfloat-abi=hard','-O2',
           '-ffp-contract=off','-ffunction-sections','-fdata-sections','-fno-unwind-tables','-fno-asynchronous-unwind-tables']
    objs=[]
    for source in inputs:
        if source.suffix not in ('.c','.S'):continue
        obj=dest/(source.name+'.o');objs.append(obj)
        # The abort frame saves only core registers. Prohibit VFP/NEON in its C handler.
        extra=['-mgeneral-regs-only'] if source.name=='mmio_guard.c' else []
        subprocess.run([tool('gcc'),*flags,*extra,'-Wall','-Wextra','-Werror','-c',dest/source.name,'-o',obj],check=True)
    subprocess.run([tool('ar'),'rcs',dest/'libmeasure.a',*objs],check=True)
    bsp=out/'zz9000_bsp_new/ps7_cortexa9_0'
    vectors=bsp/'libsrc/standalone_v6_8/src/asm_vectors.S'
    original_vectors=sha(vectors);vectors.write_text(patch_vectors(vectors.read_text()))
    vector_obj=dest/'asm_vectors.o'
    subprocess.run([tool('gcc'),*flags,'-I'+str(bsp/'include'),'-c',vectors,'-o',vector_obj],check=True)
    subprocess.run([tool('ar'),'r',bsp/'lib/libxil.a',vector_obj],check=True)
    main=(fw/'src/main.c').read_text()
    main=once(main,'#include "zz-clock/firmware.h"','static int grx_owns_gpu(void);\n#include "zz-clock/firmware.h"')
    main=once(main,'static void xacpgpu_submit_idle_poll(void)',
              '#include "grx-measure/measure_firmware.inc"\n\nstatic void xacpgpu_submit_idle_poll(void)')
    main=once(main,'    if (cmd == OP_GPU_QUERY) {','''    if (cmd == GRX_STREAM_OPCODE) {
        grx_stream_command((uint8_t*)video_state->framebuffer + XACP_CMD_OFFSET);
        x_error=0; x_status=XACP_STATUS_DONE; break;
    }
    if (cmd == GRX_MEASURE_OPCODE) {
        grx_firmware_command((uint8_t*)video_state->framebuffer + XACP_CMD_OFFSET);
        x_error=0; x_status=XACP_STATUS_DONE; break;
    }
    if (grx_owns_gpu() && (cmd==OP_GPU_TEST2 || cmd==OP_GPU_SUBMIT)) {
        x_error=0x49f0; x_status=XACP_STATUS_ERROR; break;
    }
    if (cmd == OP_GPU_QUERY) {''')
    main=once(main,'\t\t\tzcu_poll();','\t\t\tzcu_poll();\n\t\t\tgrx_firmware_poll();')
    (fw/'src/main.c').write_text(main)
    clock=fw/'src/zz-clock/firmware.h'
    clock.write_text(once(clock.read_text(),'return zz_clock_core1_idle()','return !grx_owns_gpu()&&zz_clock_core1_idle()'))
    make=(fw/'Makefile').read_text()
    # BSP vector references the guard; put its object directly into the link so
    # archive order cannot discard the exception hook.
    make=once(make,'OBJS += $(BOOTROM_IMAGE_OBJ)','OBJS += $(BOOTROM_IMAGE_OBJ) $(SRC_DIR)/grx-measure/mmio_guard.c.o')
    make=once(make,'LIBS = $(COPPER_CORE0)','GLIDE_MEASURE = $(SRC_DIR)/grx-measure/libmeasure.a\nLIBS = $(GLIDE_MEASURE) $(COPPER_CORE0)')
    make=once(make,'$(TARGET): $(OBJS) $(AY_CORE0) $(PAULA_CORE0) $(CLOCK_TEST) $(COPPER_CORE0)',
              '$(TARGET): $(OBJS) $(AY_CORE0) $(PAULA_CORE0) $(CLOCK_TEST) $(COPPER_CORE0) $(GLIDE_MEASURE)')
    (fw/'Makefile').write_text(make)
    subprocess.run(['make','-j4','CC='+tool('gcc'),'OBJCOPY='+tool('objcopy'),
        'SIZE='+tool('size'),'PYTHON=python3','EXTRA_CFLAGS=-std=gnu11'],cwd=fw,check=True)
    elf=fw/'build/ZZ9000OS.elf'
    symbols={p[2]:int(p[0],16) for line in subprocess.check_output([tool('nm'),elf],text=True).splitlines() if len(p:=line.split())==3}
    for name in ('grx_mmio_abort','grx_mmio_load','grx_mmio_store','grx_measure_poll',
                 'zcu_poll','zpm_poll','zay_idle_poll','zc_enter'):assert name in symbols,name
    end=symbols['__low_firmware_end'];limit=symbols['_XACP_LOW_FW_SAFE_LIMIT'];assert end<=limit
    record={'scope':'Build only; no installed firmware or measured GPU FPS','opcode':'0x7a20',
            'elf_sha256':sha(elf),'low_end':hex(end),'remaining_bytes':limit-end,
            'base':PIN,'bsp_vectors_original':original_vectors,'bsp_vectors_patched':sha(vectors),
            'sources':{str(s.relative_to(ROOT)):sha(s) for s in inputs+[Path(__file__),ROOT/'scripts/glide_abort.py']}}
    (out/'build.json').write_text(json.dumps(record,indent=2)+'\n')
    print(json.dumps({k:v for k,v in record.items() if k!='sources'},indent=2))
if __name__=='__main__':main()
