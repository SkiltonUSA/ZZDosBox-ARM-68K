"""Experimental ARM register store-to-load forwarding during translation."""
import difflib


def prepare(out):
    patches=[]
    def change(name,fn):
        path=out/name;old=path.read_text();new=fn(old);assert old!=new
        path.write_text(new)
        patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+name,tofile='b/'+name))
    def backend(text):
        marker='// helper function\nstatic bool val_is_operand2'
        assert marker in text
        helper='''#include "jit_reuse.h"
static_assert(C_TARGETCPU==ARMV7LE && FC_REGS_ADDR==HOST_r10,"ARMv7 resident register base");
static const Bit8u *db_reuse_floor;
unsigned db_jit_reuse_hits;
static void db_cpu_load_word(Bit32u instruction){
 if(db_reuse_floor && cache.pos-db_reuse_floor>=4){
  int source=db_arm_stored_register(host_readd((HostPt)(cache.pos-4)),instruction);
  if(source>=0){gen_mov_regs((HostReg)((instruction>>12)&15),(HostReg)source);db_jit_reuse_hits++;return;}
 }
 cache_addd(instruction);
}

'''
        text=text.replace(marker,helper+marker)
        for opcode in ('LDR_IMM','LDR_IMM_M'):
            old=f'cache_addd( {opcode}(dest_reg, addr_reg, '
            assert old in text
            text=text.replace(old,f'db_cpu_load_word( {opcode}(dest_reg, addr_reg, ')
        direct='cache_addd( LDR_IMM(dest_reg, FC_REGS_ADDR, index) );'
        assert text.count(direct)==2
        text=text.replace(direct,'db_cpu_load_word( LDR_IMM(dest_reg, FC_REGS_ADDR, index) );')
        for name in ('gen_fill_branch(const Bit8u* data)', 'gen_fill_function_ptr(const Bit8u * pos,void* fct_ptr,Bitu flags_type)'):
            marker=name+' {';assert marker in text
            text=text.replace(marker,marker+'\n db_reuse_floor=cache.pos; // a control-flow/patch boundary cannot reuse a prior store')
        return text
    change('src/cpu/core_dynrec/risc_armv4le-o3.h',backend)
    def decoder(text):
        marker='decode.active_block=decode.block=cache_openblock();';assert marker in text
        return text.replace(marker,marker+'\n db_reuse_floor=cache.pos;')
    change('src/cpu/core_dynrec/decoder.h',decoder)
    (out/'jit-reuse-integration.patch').write_text(''.join(patches))
