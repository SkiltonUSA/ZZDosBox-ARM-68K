/* GPL-2.0-or-later. Diagnostic only: advertise no hardware, never render. */
#include "guest.h"
#include "guest_calls.h"
int grx_guest_decode(grx_guest_call *c,unsigned id,uint32_t stack,const grx_guest_io *io){
 uint8_t bytes[64];unsigned n;
 c->id=GRX_CALL_INVALID;c->count=0;
 if(!id||id>=GRX_CALL_COUNT)return 0;
 n=grx_calls[id].bytes;
 if(n>sizeof(bytes)||(n&&stack>UINT32_MAX-(n-1)))return 0;
 if(n&&!io->read(io->opaque,stack,bytes,n))return 0;
 for(unsigned i=0;i<n/4;i++)c->args[i]=(uint32_t)bytes[i*4]|(uint32_t)bytes[i*4+1]<<8|(uint32_t)bytes[i*4+2]<<16|(uint32_t)bytes[i*4+3]<<24;
 c->id=id;c->count=n/4;return 1;
}
uint32_t grx_guest_diagnostic(const grx_guest_call *c,const grx_guest_io *io){
 if(c->id==GRX_CALL_grGlideGetVersion){
  /* Glide promises an 80-byte destination; emit just the terminated text. */
  static const char version[]="ZZDosBox Glide diagnostic";
  uint64_t address=(uint64_t)io->data_base+c->args[0];
  if(address<=UINT32_MAX-(sizeof(version)-1))
   io->write(io->opaque,(uint32_t)address,version,sizeof(version));
 }
 /* In particular QueryHardware/QueryBoards/WinOpen must fail until a real
    rendering transport exists. Import coverage does not imply rendering. */
 return 0;
}
