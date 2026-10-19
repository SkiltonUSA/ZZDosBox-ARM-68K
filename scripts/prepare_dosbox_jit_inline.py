"""Experimental emitted ARM fast path for checked guest reads.

Only a currently linked low-bank TLB read pointer qualifies. Unaligned wide
reads, high banks and null pointers use the existing checked helper unchanged.
"""
import difflib


def prepare(out):
    path=out/'src/cpu/core_dynrec/decoder_basic.h';old=path.read_text()
    marker='// functions that enable access to the memory';assert old.count(marker)==1
    code='''
static_assert(C_TARGETCPU==ARMV7LE && sizeof(tlb_entry)==20 && offsetof(tlb_entry,read)==0,
              "ARMv7 banked TLB layout");
static const Bit8u *db_inline_read(unsigned width,HostReg dst){
 // r0 remains the original address on every branch to the checked helper.
 cache_addd(TST_IMM(FC_OP1,0xff,8));
 const Bit8u *high=cache.pos;cache_addd(BNE_FWD(0));
 const Bit8u *unaligned=nullptr;
 if(width>1){cache_addd(TST_IMM(FC_OP1,width-1,0));unaligned=cache.pos;cache_addd(BNE_FWD(0));}
 gen_mov_dword_to_reg_imm(temp1,(Bit32u)&paging.tlbh[0]);
 cache_addd(MOV_REG_LSR_IMM(HOST_a3,FC_OP1,12));
 cache_addd(ADD_REG_LSL_IMM(HOST_a3,HOST_a3,HOST_a3,2));
 cache_addd(LDR_REG_LSL_IMM(temp1,temp1,HOST_a3,2));
 const Bit8u *unlinked=gen_create_branch_on_zero(temp1,true);
 cache_addd(ADD_REG_LSL_IMM(temp1,temp1,FC_OP1,0));
 if(width==1)cache_addd(LDRB_IMM(dst,temp1,0));
 else if(width==2)cache_addd(LDRH_IMM(dst,temp1,0));
 else cache_addd(LDR_IMM(dst,temp1,0));
 if(dst!=FC_RETOP)cache_addd(MOV_IMM(FC_RETOP,0,0));
 const Bit8u *done=cache.pos;cache_addd(B_FWD(0));
 gen_fill_branch(high);if(unaligned)gen_fill_branch(unaligned);gen_fill_branch(unlinked);
 return done;
}

'''
    text=old.replace(marker,code+marker)
    for function,end,width in [('dyn_read_byte','dyn_read_byte_canuseword','1'),
                               ('dyn_read_byte_canuseword','dyn_write_byte','1'),
                               ('dyn_read_word','dyn_write_word','dword?4:2')]:
        start=text.index('static void '+function+'(');stop=text.index('static void '+end+'(',start)
        part=text[start:stop];move='gen_mov_regs(FC_OP1,reg_addr);';assert part.count(move)==1
        part=part.replace(move,move+f'\n const Bit8u *done=db_inline_read({width},reg_dst);')
        close=part.index('\n}');part=part[:close]+'\n gen_fill_branch(done);'+part[close:]
        text=text[:start]+part+text[stop:]
    path.write_text(text)
    (out/'jit-inline-integration.patch').write_text(''.join(difflib.unified_diff(
        old.splitlines(True),text.splitlines(True),fromfile='a/src/cpu/core_dynrec/decoder_basic.h',tofile='b/src/cpu/core_dynrec/decoder_basic.h')))
