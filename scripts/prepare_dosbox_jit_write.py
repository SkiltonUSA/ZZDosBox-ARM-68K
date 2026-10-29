"""Opt-in checked-write fast path. Re-read the live TLB on every access.

Only aligned low-bank writes with a nonnull write mapping qualify. Code-page
handlers remove that mapping, retaining self-modification checks on the fallback.
"""
import difflib

def prepare(out):
    path=out/'src/cpu/core_dynrec/decoder_basic.h';old=path.read_text()
    marker='// functions that enable access to the memory';assert old.count(marker)==1
    helper='''
static_assert(C_TARGETCPU==ARMV7LE && sizeof(tlb_entry)==20 && offsetof(tlb_entry,write)==4,
              "ARMv7 banked write TLB layout");
static const Bit8u *db_inline_write(unsigned width){
 // Preserve r0=address and r1=value on every branch to the original helper.
 cache_addd(TST_IMM(FC_OP1,0xff,8));
 const Bit8u *high=cache.pos;cache_addd(BNE_FWD(0));
 const Bit8u *unaligned=nullptr;
 if(width>1){cache_addd(TST_IMM(FC_OP1,width-1,0));unaligned=cache.pos;cache_addd(BNE_FWD(0));}
 gen_mov_dword_to_reg_imm(temp1,(Bit32u)&paging.tlbh[0].write);
 cache_addd(MOV_REG_LSR_IMM(HOST_a3,FC_OP1,12));
 cache_addd(ADD_REG_LSL_IMM(HOST_a3,HOST_a3,HOST_a3,2));
 cache_addd(LDR_REG_LSL_IMM(temp1,temp1,HOST_a3,2));
 const Bit8u *unlinked=gen_create_branch_on_zero(temp1,true);
 cache_addd(ADD_REG_LSL_IMM(temp1,temp1,FC_OP1,0));
 if(width==1)cache_addd(STRB_IMM(FC_OP2,temp1,0));
 else if(width==2)cache_addd(STRH_IMM(FC_OP2,temp1,0));
 else cache_addd(STR_IMM(FC_OP2,temp1,0));
 cache_addd(MOV_IMM(FC_RETOP,0,0));
 const Bit8u *done=cache.pos;cache_addd(B_FWD(0));
 gen_fill_branch(high);if(unaligned)gen_fill_branch(unaligned);gen_fill_branch(unlinked);
 return done;
}

'''
    text=old.replace(marker,helper+marker)
    for name,width in (('dyn_write_byte','1'),('dyn_write_word','dword?4:2')):
        start=text.index('static void '+name+'(');stop=text.index('\n}',start)+2
        part=text[start:stop];move='gen_mov_regs(FC_OP1,reg_addr);';assert part.count(move)==1
        part=part.replace(move,move+'\n const Bit8u *done=db_inline_write('+width+');')
        part=part[:-2]+'\n gen_fill_branch(done);\n}'
        text=text[:start]+part+text[stop:]
    path.write_text(text)
    (out/'jit-write-integration.patch').write_text(''.join(difflib.unified_diff(old.splitlines(True),text.splitlines(True),fromfile='a/src/cpu/core_dynrec/decoder_basic.h',tofile='b/src/cpu/core_dynrec/decoder_basic.h')))
