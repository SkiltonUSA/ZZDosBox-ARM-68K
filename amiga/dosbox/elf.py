"""Strict benchmark-only image packer: low code/data plus high zeroed BSS."""
import struct

def unpack(data, *, expanded=False):
    image_limit=0x140000 if expanded else 0x100000
    bss_limit=0x3b00000 if expanded else 0x1fd0000
    if data[:6]!=b'\x7fELF\x01\x01' or struct.unpack_from('<H',data,18)[0]!=40:
        raise ValueError('Expected ELF32 little-endian ARM')
    h=struct.unpack_from('<HHIIIIIHHHHHH',data,16)
    entry,phoff,shoff,phsize,phnum,shsize,shnum=h[3],h[4],h[5],h[8],h[9],h[10],h[11]
    segments=[struct.unpack_from('<8I',data,phoff+i*phsize) for i in range(phnum)]
    loads=[p for p in segments if p[0]==1]
    if len(loads)!=1:raise ValueError('One low image required')
    _,off,addr,_,filesz,memsz,_,_=loads[0]
    if addr or not 0<memsz<image_limit or filesz!=memsz or off+filesz>len(data) or entry>=memsz:
        raise ValueError('Invalid image bounds')
    image=bytearray(data[off:off+filesz]);sections=[struct.unpack_from('<10I',data,shoff+i*shsize) for i in range(shnum)]
    bss=[s for s in sections if s[1]==8 and s[2]&2]
    if len(bss)!=1 or bss[0][3]!=0x220000 or not 0<bss[0][5]<bss_limit-0x220000:
        raise ValueError('Expected bounded high BSS')
    end=bss[0][3]+bss[0][5]
    for s in sections:
        if s[1] in (4,15,16):raise ValueError('Unsupported relocation/constructor')
        if s[2]&2 and s[1]!=8 and (s[3]+s[5]>memsz):raise ValueError('Unloaded allocated section')
    rel=[]
    for s in sections:
        if s[1]!=9:continue
        if s[9]!=8 or s[5]%8 or s[4]+s[5]>len(data):raise ValueError('Invalid relocation table')
        for pos in range(s[4],s[4]+s[5],8):
            at,info=struct.unpack_from('<II',data,pos)
            if info!=23 or at%4 or at+4>memsz or at in rel:raise ValueError('Unsupported relocation')
            target=struct.unpack_from('<I',image,at)[0]
            if not (target<memsz or 0x220000<=target<end):raise ValueError('Relocation escapes data/code')
            rel.append(at)
    # Each constructor must be relocated into loaded executable code.
    for s in sections:
        if s[1]!=14:continue
        if s[3]%4 or s[5]%4:raise ValueError('Unaligned constructor array')
        for at in range(s[3],s[3]+s[5],4):
            target=struct.unpack_from('<I',image,at)[0]
            if at not in rel or not any(t[2]&4 and t[3]<=target<t[3]+t[5] for t in sections):
                raise ValueError('Constructor escapes executable image')
    return image,rel,entry
