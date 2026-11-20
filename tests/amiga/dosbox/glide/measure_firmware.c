/* ARM integration fixture; the preceding validator is the pinned firmware's.
 * Hardware callbacks are modeled. Pixel expectations come from the independent
 * Python fixture, while the production verifier executes unchanged. */
#include <string.h>
#include "amiga/dosbox/glide/experimental/mmio_transport.h"
typedef uint64_t XTime;
static uint64_t clock_ticks;
static uint32_t token_epoch,token_sequence,token_valid;
unsigned test_failure,test_sends,test_profile;
unsigned test_words,test_max_words,test_payload_hash=2166136261u;
static int xacpgpu_submit_pending,xacpgpu_test2_pending;
static void XTime_GetTime(XTime *t){*t=(clock_ticks+=1000);}
static void Xil_DCacheInvalidateRange(uintptr_t p,unsigned n){(void)p;(void)n;}
static void Xil_DCacheFlushRange(uintptr_t p,unsigned n){(void)p;(void)n;}
static uint32_t Xil_In32(uintptr_t p){(void)p;return 0;}
static int zc_idle(void){return test_failure!=1;}
static uint32_t zc_get(const unsigned char *p)
{return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static void zc_put(unsigned char *p,uint32_t x)
{p[0]=x>>24;p[1]=x>>16;p[2]=x>>8;p[3]=x;}
int grx_mmio_init(grx_mmio_transport *t){t->ready=1;return test_failure!=2;}
int grx_mmio_read(void *p,unsigned offset,uint32_t *value)
{
    (void)p;
    if(offset==28)*value=0x47525831;
    else if(offset==0)*value=token_valid;
    else if(offset==4)*value=token_epoch;
    else if(offset==8)*value=token_sequence;
    else return 0;
    return 1;
}
int grx_mmio_write(void *p,unsigned offset,uint32_t value)
{(void)p;(void)value;if(offset==20)token_valid=0;return 1;}
static int xacpgpu_fifo_send_be(const volatile uint32_t *words,uint32_t n,uint32_t *isr)
{
    test_sends++;*isr=0;
    if(test_failure==3)return -4;
    XACPGPU_SafeState next;uint32_t bad,detail;
    if(n>512||n<10||xacpgpu_validate_submit(words,n-7,&next,&bad,&detail))return -98;
    if(n>test_max_words)test_max_words=n;
    for(unsigned i=2;i<n-7;++i){test_words++;test_payload_hash=(test_payload_hash^be32(words[i]))*16777619u;}
    token_epoch=be32(words[n-2]);token_sequence=be32(words[n-1]);token_valid=1;
    return 0;
}
#include "amiga/dosbox/glide/experimental/measure_firmware.inc"
static unsigned char message[64];
uint32_t *test_command(unsigned op,unsigned frames,unsigned epoch)
{
    zc_put(message,op);zc_put(message+4,test_profile?2:1);zc_put(message+8,frames);zc_put(message+12,epoch);
    zc_put(message+16,test_profile);
    grx_firmware_command(message);
    return (uint32_t*)message;
}
void test_poll(void){grx_firmware_poll();}
unsigned test_claimed(void){return grx_owns_gpu();}
unsigned test_stat(unsigned i)
{return i==0?test_sends:i==1?test_words:i==2?test_payload_hash:test_max_words;}

uint32_t *test_stream_command(unsigned op,unsigned nonce){
 zc_put(message,op);zc_put(message+4,1);zc_put(message+8,nonce);
 grx_stream_command(message);return (uint32_t*)message;
}
unsigned test_stream_request(unsigned op,unsigned nonce,unsigned seq){
 grx_stream_request q={.sequence=seq,.nonce=nonce,.op=op,.address=0x06610000,.bytes=8};
 memset(grx_channel.data,0x5a,8);
 if(op==GRX_STREAM_SUBMIT){grx_packet p;grx_probe_scene(&p,0,0x1234);q.address=0;q.bytes=p.words*4;memcpy(grx_channel.data,p.bytes,q.bytes);}
 grx_channel.request=q;return q.bytes;
}
uint32_t *test_stream_result(void){return (uint32_t*)&grx_channel.result;}
