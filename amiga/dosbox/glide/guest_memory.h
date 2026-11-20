/* Include after DOSBox paging.h. Use only its established direct TLB mappings.
 * A null mapping retains the checked byte handler (faults, MMIO, code writes).
 * Re-read the mapping after each handler and at every 4 KiB page boundary. */
#ifndef GRX_GUEST_MEMORY_H
#define GRX_GUEST_MEMORY_H
#include <stdint.h>
#include <string.h>
static inline int grx_guest_read_checked(uint32_t a,void *out,unsigned n){
 uint8_t *p=(uint8_t *)out;
 if(n&&a>UINT32_MAX-(n-1))return 0;
 while(n){
  HostPt mapped=get_tlb_read(a);
  unsigned part=1;
  if(mapped){
   part=4096-(a&4095);if(part>n)part=n;
   memcpy(p,mapped+a,part);
  }else if(mem_readb_checked(a,p))return 0;
  a+=part;p+=part;n-=part;
 }
 return 1;
}
static inline int grx_guest_write_checked(uint32_t a,const void *in,unsigned n){
 const uint8_t *p=(const uint8_t *)in;
 if(n&&a>UINT32_MAX-(n-1))return 0;
 while(n){
  HostPt mapped=get_tlb_write(a);
  unsigned part=1;
  if(mapped){
   part=4096-(a&4095);if(part>n)part=n;
   memcpy(mapped+a,p,part);
  }else if(mem_writeb_checked(a,*p))return 0;
  a+=part;p+=part;n-=part;
 }
 return 1;
}
#endif
