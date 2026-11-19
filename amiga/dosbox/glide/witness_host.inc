/* Included only by the cooperative launcher's host callback build. */
#include "witness_wire.h"
static volatile uint8_t *witness_base;
static const struct ad_io *witness_io;
static unsigned witness_seq,witness_mode,witness_reverse;
static ULONG now(void);
static int witness_wait(unsigned field) {
 ULONG start=now();
 for(unsigned i=0;(ULONG)(now()-start)<1000000u;i++) {
  witness_io->pull(witness_base+GW_REPLY,64,witness_io->user);
  if(ad_get(witness_base,GW_REPLY+field)==witness_seq)return 1;
  if(SetSignal(0,0)&SIGBREAKF_CTRL_C)break;
  if(witness_mode<3){if(i>=49)break;Delay(1);}
 }
 puts("STOP: Core1 witness timeout");return 0;
}
static int witness_prepare(unsigned scene,unsigned marker,unsigned offset,unsigned count) {
 volatile uint8_t *c=witness_base+GW_CMD;
 ++witness_seq;
 ad_put(c,GW_NONCE,witness_seq^ad_get(witness_base,ZZ_CONTROL));
 ad_put(c,GW_SCENE,scene);ad_put(c,GW_MARKER,marker);
 ad_put(c,GW_OFFSET,offset);ad_put(c,GW_COUNT,count);ad_put(c,GW_IDLE,witness_mode==2);ad_put(c,GW_REVERSE,witness_reverse);
 witness_io->push(c,64,witness_io->user);witness_io->barrier(witness_io->user);
 ad_put(c,GW_SEQ,witness_seq);witness_io->push(c,4,witness_io->user);
 return witness_wait(GW_ARMED);
}
static void witness_go(void) {
 ad_put(witness_base,GW_GO,witness_seq);
 witness_io->push(witness_base+GW_GO,4,witness_io->user);
}
static int witness_finish(void) {
 if(witness_mode==1)witness_go();
 if(!witness_wait(GW_DONE))return 0;
 volatile uint8_t *r=witness_base+GW_REPLY;
 printf("ARM scene=%u error=%u checked=%u bad=%u polls=%u wait_ticks=%u\n",
  (unsigned)ad_get(witness_base,GW_CMD+GW_SCENE),(unsigned)ad_get(r,GW_ERROR),
  (unsigned)ad_get(r,GW_CHECKED),(unsigned)ad_get(r,GW_BAD),
  (unsigned)ad_get(r,GW_POLLS),(unsigned)ad_get(r,GW_TICKS));
 printf("ARMREAD reverse=%u verify_ticks=%u first_offset=%u got=%04x want=%04x\n",
  witness_reverse,(unsigned)ad_get(r,GW_VERIFY_TICKS),(unsigned)ad_get(r,GW_FIRST_OFFSET),
  (unsigned)ad_get(r,GW_FIRST_GOT),(unsigned)ad_get(r,GW_FIRST_WANT));
 return !ad_get(r,GW_ERROR)&&!ad_get(r,GW_BAD);
}
