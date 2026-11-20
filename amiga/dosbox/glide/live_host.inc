/* Included in the isolated generated 68k launcher; Open before Core1 GO,
 * Close after reset-to-idle. Never allow a failed GPU lease to look released. */
#include "amiga/dosbox/glide/live_wire.h"
static volatile UWORD *glide_command;
static volatile ULONG *glide_mailbox;
static ULONG glide_nonce,glide_opened;
static int glide_request(ULONG op){
 unsigned i;
 if(*glide_command)return 0;
 for(i=0;i<16;i++)glide_mailbox[i]=0;
 glide_mailbox[0]=op;glide_mailbox[1]=1;glide_mailbox[2]=glide_nonce;
 CacheClearE((APTR)glide_mailbox,64,CACRF_ClearD);*glide_command=0x7a21;
 for(i=0;i<100&&*glide_command!=2&&*glide_command!=3;i++)Delay(1);
 if(*glide_command==3){*glide_command=0;return 0;}
 if(*glide_command!=2)return 0;
 CacheClearE((APTR)glide_mailbox,64,CACRF_ClearD);
 int ok=glide_mailbox[0]==0x47525331&&glide_mailbox[1]==1&&!glide_mailbox[2];
 *glide_command=0;return ok;
}
static int glide_prepare(volatile UBYTE *board,volatile uint8_t *mem,ULONG nonce){
 glide_command=(volatile UWORD*)(board+0x64);glide_mailbox=(volatile ULONG*)(board+0x4010000u);glide_nonce=nonce;
 /* Query verifies the candidate service before claiming its GPU. */
 if(!glide_request(0)||glide_mailbox[3])return 0;
 glide_opened=1;if(!glide_request(1))return 0;
 ad_put(mem,PC_GLIDE_HOST,PC_GLIDE_MAGIC);ad_put(mem,PC_GLIDE_HOST+4,glide_mailbox[4]);
 ad_put(mem,PC_GLIDE_HOST+8,glide_mailbox[5]);ad_put(mem,PC_GLIDE_HOST+12,nonce);
 printf("GLIDE_OPEN channel=%08x bytes=%u nonce=%08x\n",glide_mailbox[4],glide_mailbox[5],nonce);return 1;
}
static int glide_finish(volatile uint8_t *mem){
 if(!glide_opened)return 1;
 CacheClearE((APTR)(mem+PC_GLIDE_REPORT),64,CACRF_ClearD);
 printf("GTA_PRESENT magic=%08x frames=%u warmup=10 first=%08x%08x last=%08x%08x call_error=%u status=%u\n",ad_get(mem,PC_GLIDE_REPORT),ad_get(mem,PC_GLIDE_REPORT+4),ad_get(mem,PC_GLIDE_REPORT+8),ad_get(mem,PC_GLIDE_REPORT+12),ad_get(mem,PC_GLIDE_REPORT+16),ad_get(mem,PC_GLIDE_REPORT+20),ad_get(mem,PC_GLIDE_REPORT+24),ad_get(mem,PC_GLIDE_REPORT+28));
 CacheClearE((APTR)(mem+PC_GLIDE_PROFILE),PC_GLIDE_PROFILE_STAGES*PC_GLIDE_PROFILE_STRIDE,CACRF_ClearD);
 for(unsigned i=0;i<PC_GLIDE_PROFILE_STAGES;i++){
  unsigned at=PC_GLIDE_PROFILE+i*PC_GLIDE_PROFILE_STRIDE;
  printf("GTA_STAGE stage=%u calls=%u bytes=%08x%08x ticks=%08x%08x\n",i,ad_get(mem,at),ad_get(mem,at+4),ad_get(mem,at+8),ad_get(mem,at+12),ad_get(mem,at+16));
 }
 if(!glide_request(2)){puts("GLIDE_CLOSE FAILED: GPU ownership retained; restart required before further GPU work.");return 0;}
 glide_opened=0;puts("GLIDE_CLOSE OK");return 1;
}
