/* GPL-3.0-or-later. PL330 memory-copy program builder, not a device driver.
 * Instruction encodings: Arm PL330 TRM / Xilinx embeddedsw dmaps driver.
 * This only writes a caller-owned program buffer. It never starts a channel.
 * A firmware lease, physical mappings, cache handoff and quiescent completion
 * are still required before executing any generated program on hardware. */
#pragma once
#include <stdint.h>

#define PC_DMA_PROGRAM_CAPACITY 64u
#define PC_DMA_COPY_LIMIT 0x400000u
struct pc_dma_span { uint32_t address, bytes; };
struct pc_dma_program { uint8_t code[PC_DMA_PROGRAM_CAPACITY]; unsigned size; };

static inline int pc_dma_contains(struct pc_dma_span owned,uint32_t addr,uint32_t n)
{
 return owned.bytes && (uint64_t)owned.address+owned.bytes<=0x100000000ull &&
        addr>=owned.address && n<=owned.bytes && addr-owned.address<=owned.bytes-n;
}
static inline void pc_dma_byte(struct pc_dma_program *p,unsigned byte)
{ p->code[p->size++]=(uint8_t)byte; }
static inline void pc_dma_mov(struct pc_dma_program *p,unsigned reg,uint32_t value)
{
 unsigned i;pc_dma_byte(p,0xbc);pc_dma_byte(p,reg);
 for(i=0;i<4;i++)pc_dma_byte(p,value>>(8*i));
}
static inline void pc_dma_loop(struct pc_dma_program *p,unsigned count)
{
 /* Finite LC0 loop: LD; ST; LPEND LC0, -2. count is in 1..256. */
 pc_dma_byte(p,0x20);pc_dma_byte(p,count-1);
 pc_dma_byte(p,0x04);pc_dma_byte(p,0x08);
 pc_dma_byte(p,0x38);pc_dma_byte(p,2);
}
static inline uint32_t pc_dma_ccr(unsigned size,unsigned beats,unsigned ns,unsigned swap32)
{
 /* Increment both addresses, equal bursts, optional 32-bit swap, no cache
  * allocation request. Nonsecure protection must match the firmware lease. */
 return 1u|(size<<1)|((beats-1)<<4)|(ns<<9)|
        (1u<<14)|(size<<15)|((beats-1)<<18)|(ns<<23)|(swap32<<29);
}
static inline int pc_dma_build_copy(struct pc_dma_program *p,
 struct pc_dma_span source,struct pc_dma_span destination,
 uint32_t src,uint32_t dst,uint32_t bytes,unsigned nonsecure,unsigned swap32)
{
 unsigned bursts,groups,left,tail;
 p->size=0;
 if(!bytes||bytes>PC_DMA_COPY_LIMIT||((src|dst)&7u)||nonsecure>1||swap32>1||
    (swap32&&(bytes&3u))||
    !pc_dma_contains(source,src,bytes)||!pc_dma_contains(destination,dst,bytes)||
    ((uint64_t)src<(uint64_t)dst+bytes&&(uint64_t)dst<(uint64_t)src+bytes))return 0;
 pc_dma_mov(p,0,src);pc_dma_mov(p,2,dst);
 bursts=bytes/128;groups=bursts/256;left=bursts%256;tail=bytes%128;
 if(bursts) {
  pc_dma_mov(p,1,pc_dma_ccr(3,16,nonsecure,swap32)); /* 16 x 8 bytes */
  if(groups) {
   pc_dma_byte(p,0x22);pc_dma_byte(p,groups-1); /* LC1, at most 128 */
   pc_dma_loop(p,256);
   pc_dma_byte(p,0x3c);pc_dma_byte(p,6);
  }
  if(left)pc_dma_loop(p,left);
 }
 if(tail) {
  pc_dma_mov(p,1,pc_dma_ccr(swap32?2:0,1,nonsecure,swap32));
  pc_dma_loop(p,swap32?tail/4:tail);
 }
 pc_dma_byte(p,0x13); /* DMAWMB: drain writes before DMAEND. */
 pc_dma_byte(p,0x00);
 /* Maximum emitted length: 18+10+6+12+2 = 48, within 64 bytes. */
 return 1;
}
