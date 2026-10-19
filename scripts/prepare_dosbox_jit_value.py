"""Experimental ARMv7 checked-read ABI: value in r0, exception flag in r1.

The slow path retains the upstream permission/device/page-boundary handling.
Only the direct TLB hit avoids a readdata store and subsequent generated load.
"""
import difflib


def prepare(out):
    path = out / 'src/cpu/core_dynrec/decoder_basic.h'
    original = path.read_text()
    marker = '// functions that enable access to the memory'
    assert original.count(marker) == 1
    helpers = '''
#if C_TARGETCPU != ARMV7LE
#error Checked-read value ABI is ARMv7-only
#endif
static_assert(sizeof(Bitu)==4, "AAPCS32 checked-read ABI");
'''
    for suffix, width, typ in [('b', 1, 'Bit8u'), ('w', 2, 'Bit16u'), ('d', 4, 'Bit32u')]:
        guard = 'true' if width == 1 else f'(address & 4095u)<={4096-width}u'
        helpers += f'''
static Bit64u DRC_CALL_CONV db_read{suffix}_value(PhysPt address) {{
 if({guard}){{
  HostPt p=get_tlb_read(address);
  if(p)return host_read{suffix}(p+address);
 }}
 bool error=mem_read{suffix}_checked_drc(address);
 return ((Bit64u)error<<32)|*(({typ}*)&core_dynrec.readdata);
}}
'''
    text = original.replace(marker, helpers + '\n' + marker)
    for function, end in [('dyn_read_byte', 'dyn_read_byte_canuseword'),
                          ('dyn_read_byte_canuseword', 'dyn_write_byte'),
                          ('dyn_read_word', 'dyn_write_word')]:
        start = text.index('static void ' + function + '(')
        stop = text.index('static void ' + end + '(', start)
        fragment = text[start:stop]
        for suffix in 'bwd':
            fragment = fragment.replace('&mem_read' + suffix + '_checked_drc', '&db_read' + suffix + '_value')
        assert 'dyn_check_exception(FC_RETOP);' in fragment
        fragment = fragment.replace('dyn_check_exception(FC_RETOP);', 'dyn_check_exception(FC_OP2);')
        for load in ['gen_mov_byte_to_reg_low(reg_dst,&core_dynrec.readdata);',
                     'gen_mov_byte_to_reg_low_canuseword(reg_dst,&core_dynrec.readdata);',
                     'gen_mov_word_to_reg(reg_dst,&core_dynrec.readdata,dword);']:
            fragment = fragment.replace(load, 'gen_mov_regs(reg_dst,FC_RETOP);')
        text = text[:start] + fragment + text[stop:]
    path.write_text(text)
    (out / 'jit-value-integration.patch').write_text(''.join(difflib.unified_diff(
        original.splitlines(True), text.splitlines(True),
        fromfile='a/src/cpu/core_dynrec/decoder_basic.h',
        tofile='b/src/cpu/core_dynrec/decoder_basic.h')))
