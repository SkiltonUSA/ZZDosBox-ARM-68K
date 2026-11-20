/* Private streaming service; same sandbox validator and tagged retirement.
 * Firmware publishes its own channel address. No caller-selected DDR arena. */
#include "stream.h"
#define GRX_STREAM_OPCODE 0x7a21u
static grx_stream_channel grx_channel __attribute__((aligned(4096)));
static grx_stream grx_live;
static grx_completion_batch grx_live_batch;
static uint64_t grx_live_start;
static uint32_t grx_live_last_nonce;
static void grx_live_pull(void *unused,void *p,unsigned n){(void)unused;Xil_DCacheInvalidateRange((uintptr_t)p,n);}
static void grx_live_push(void *unused,void *p,unsigned n){(void)unused;Xil_DCacheFlushRange((uintptr_t)p,n);}
static int grx_live_upload(void *unused,uint32_t a,const void *p,unsigned n){
 (void)unused;
 /* All earlier GPU requests have retired. Preserve adjacent bytes before a
  * partial cache-line CPU write; never clean stale framebuffer data over DMA. */
 uintptr_t lo=a&~31u;unsigned size=((a+n+31u)&~31u)-lo;
 Xil_DCacheInvalidateRange(lo,size);memcpy((void *)(uintptr_t)a,p,n);Xil_DCacheFlushRange(lo,size);return 1;
}
static int grx_live_readback(void *unused,uint32_t a,void *p,unsigned n){
 (void)unused;uintptr_t lo=a&~31u;unsigned size=((a+n+31u)&~31u)-lo;
 Xil_DCacheInvalidateRange(lo,size);memcpy(p,(const void *)(uintptr_t)a,n);return 1;
}
static int grx_live_submit(void *unused,const void *p,unsigned n,uint32_t nonce,uint32_t seq){
 (void)unused;uint32_t bad,detail,isr;
 if(grx_prepare_tagged(&grx_packet_private,p,n/4,nonce,seq,&bad,&detail))return 0;
 if(xacpgpu_fifo_send_be(grx_packet_private.wire,grx_packet_private.words,&isr))return 0;
 grx_live_batch=(grx_completion_batch){nonce,seq,GRX_COMPLETION_PENDING};
 grx_live_start=grx_ticks(0);return 1;
}
static int grx_live_retire(void *unused){
 (void)unused;grx_completion_io io={&grx_transport,grx_mmio_read,grx_mmio_write};
 grx_completion_state state=grx_completion_poll(&io,&grx_live_batch);
 if(state==GRX_COMPLETION_RETIRED){xacpgpu_safe_state=grx_packet_private.next_state;return 1;}
 if(state!=GRX_COMPLETION_PENDING||grx_ticks(0)-grx_live_start>800000000ull)return -1;
 return 0;
}
static void grx_stream_idle_poll(void){if(grx_stream_active)grx_stream_poll(&grx_live);}
static void grx_stream_command(void *data){
 unsigned char *p=data;uintptr_t aligned=(uintptr_t)p&~31u;Xil_DCacheInvalidateRange(aligned,64);
 unsigned op=zc_get(p),abi=zc_get(p+4),nonce=zc_get(p+8),error=0;
 if(abi!=1||op>2)error=0x31001;
 else if(op==1){
  if(grx_owns_gpu()||!zc_idle()||xacpgpu_submit_pending||xacpgpu_test2_pending||!nonce||nonce==grx_live_last_nonce)error=0x31002;
  else {
   uint32_t id,status;
   if(!grx_mmio_init(&grx_transport)||!grx_mmio_read(&grx_transport,0x1c,&id)||id!=0x47525831||
      !grx_mmio_read(&grx_transport,0,&status)||status||
      ((Xil_In32(XACPGPU_HP3_RDCTRL)|Xil_In32(XACPGPU_HP3_WRCTRL))&XACPGPU_HP3_32BITEN))error=0x31003;
   else {
    grx_stream_io io={0,grx_live_pull,grx_live_push,grx_live_upload,grx_live_readback,grx_live_submit,grx_live_retire};
    if(!grx_stream_open(&grx_live,&grx_channel,&io,nonce))error=0x31004;
    else {grx_stream_active=1;grx_live_last_nonce=nonce;}
   }
  }
 }else if(op==2){
  if(!zc_workers_idle()||!grx_stream_close(&grx_live,nonce))error=0x31005;
  else grx_stream_active=0;
 }
 zc_put(p,GRX_STREAM_MAGIC);zc_put(p+4,1);zc_put(p+8,error);zc_put(p+12,grx_stream_active);
 zc_put(p+16,(uint32_t)(uintptr_t)&grx_channel);zc_put(p+20,sizeof(grx_channel));
 zc_put(p+24,grx_live.nonce);zc_put(p+28,grx_live.next);zc_put(p+32,grx_live.failed);
 zc_put(p+36,grx_live.working);zc_put(p+40,GRX_STREAM_BYTES);
 Xil_DCacheFlushRange(aligned,64);
}
