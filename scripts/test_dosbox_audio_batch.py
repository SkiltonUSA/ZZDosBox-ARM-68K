"""Exercise the actual host drain with bounded queues and wraparound."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'amiga/dosbox/pc/audio_host.c').read_text();s=s[s.index('int db_audio_host_service(void){'):s.index('int db_audio_host_pending(void)')]
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'check.c').write_text(r'''
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "audio_wire.h"
typedef uint32_t ULONG;typedef void *APTR;
#define AUDIO_SLOTS 4u
#define DB_AUDIO_BATCH
struct block {ULONG count;unsigned char data[4096];};
static struct block queue[4],*audio_queue=queue;
static unsigned char memory[PC_AUDIO_DATA+PC_AUDIO_SLOTS*PC_AUDIO_STRIDE];
static volatile uint8_t *shared=memory;
static ULONG audio_write,audio_read,audio_highwater,audio_samples;
static int audio_opened=1,audio_error,audio_done,wakes;
static void sync_range(volatile void *p,uint32_t n,void *c){(void)p;(void)n;(void)c;}
static void fence(void *c){(void)c;}
struct IO {void(*pull)(volatile void*,uint32_t,void*);void(*push)(volatile void*,uint32_t,void*);void(*barrier)(void*);};
static struct IO io={sync_range,sync_range,fence},*syncio=&io;
static void Forbid(void){}static void Permit(void){}
static void CopyMem(const void*s,void*d,unsigned n){memcpy(d,s,n);}
static void audio_wake(void){wakes++;}
''' + s + r'''
int main(void){unsigned cases=0;
 for(unsigned wrap=0;wrap<2;wrap++)for(unsigned have=0;have<=4;have++)for(unsigned ready=0;ready<=4;ready++){
  uint32_t r=wrap?0xfffffffeu:2,initial=wrap?0xfffffffdu:3;
  memset(queue,0xa5,sizeof queue);memset(memory,0,sizeof memory);
  audio_read=initial;audio_write=initial+have;audio_samples=audio_highwater=wakes=0;
  ad_put(memory,PC_AUDIO_READ,r);ad_put(memory,PC_AUDIO_WRITE,r+ready);
  for(unsigned k=0;k<ready;k++)memset(memory+PC_AUDIO_DATA+((r+k)&3)*PC_AUDIO_STRIDE,0x10+k,PC_AUDIO_STRIDE);
  unsigned n=ready<4-have?ready:4-have;
  assert(db_audio_host_service());assert(audio_write==initial+have+n);assert(ad_get(memory,PC_AUDIO_READ)==r+n);
  assert(audio_samples==n*PC_AUDIO_FRAMES);assert(wakes==(n!=0));
  for(unsigned k=0;k<have;k++)for(unsigned j=0;j<4096;j++)assert(queue[(initial+k)&3].data[j]==0xa5);
  for(unsigned k=0;k<n;k++){struct block*b=&queue[(initial+have+k)&3];assert(b->count==PC_AUDIO_FRAMES);for(unsigned j=0;j<4096;j++)assert(b->data[j]==0x10+k);}
  cases++;
 }
 ad_put(memory,PC_AUDIO_WRITE,ad_get(memory,PC_AUDIO_READ)+5);assert(!db_audio_host_service());
 audio_opened=0;assert(db_audio_host_service());audio_opened=1;audio_error=1;assert(!db_audio_host_service());audio_error=0;audio_done=1;assert(!db_audio_host_service());
 printf("PASS audio batch: %u exact-order/capacity/wrap cases, corrupt queue and worker errors\n",cases);
}
''')
 includes=[root/'amiga/dosbox/pc',root/'amiga/dosbox/console',root/'amiga/dosbox/dos',root/'amiga/common/arm',root/'amiga/arm_debug',root/'amiga/arm_debug/zz9000']
 subprocess.run(['clang','-std=c99','-O2','-fsanitize=address,undefined',*['-I'+str(i) for i in includes],str(p/'check.c'),'-o',str(p/'check')],check=True)
 subprocess.run([str(p/'check')],check=True)
