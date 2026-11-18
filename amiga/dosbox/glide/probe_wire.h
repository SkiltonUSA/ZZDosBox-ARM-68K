#ifndef GRX_PROBE_WIRE_H
#define GRX_PROBE_WIRE_H
#include "translator.h"

static inline uint32_t grx_probe_crc32(const uint8_t *bytes,unsigned count)
{
    uint32_t crc=0xffffffffu;
    while(count--) {
        crc^=*bytes++;
        for(unsigned bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));
    }
    return crc^0xffffffffu;
}

/* Retag only the final clear-colour register, before the scissored memset.
 * Triangle markers have no such tail and are deliberately rejected. */
static inline int grx_probe_retag(grx_packet *p,uint16_t marker)
{
    if(!marker||p->words<3||p->words>GRX_PACKET_WORDS)return 0;
    unsigned at=(p->words-3)*4;
    const uint8_t *b=p->bytes+at;
    if(b[0]!=0x10||b[1]||b[2]||b[3]!=1||b[8]!=0x20||b[11]!=0x12)return 0;
    uint32_t rgba=((marker&0xf800u)<<16)|((marker&0x7e0u)<<13)|((marker&31u)<<11);
    for(unsigned n=0;n<4;n++)p->bytes[at+4+n]=(uint8_t)(rgba>>(24-8*n));
    return 1;
}
#endif
