/* Private fixed-scene benchmark. Include after the pinned FIFO sender.
 * No boot-time MMIO or GPU work. Start is deferred out of the Zorro handler. */
#include "measure.h"
#include "workload.h"
#include "mmio_transport.h"
#include "probe_readback.h"
#include "submit_prepare.inc"
#define GRX_MEASURE_OPCODE 0x7a20u
#define GRX_MEASURE_MAGIC 0x47524d31u
static grx_measure grx_meter;
static grx_mmio_transport grx_transport;
static GRX_TaggedSubmission grx_packet_private;
static grx_packet grx_source,grx_source_second;
static volatile unsigned grx_claimed,grx_queued,grx_target,grx_epoch,grx_profile;
static unsigned grx_last_epoch;
static unsigned grx_detail,grx_checked;
static volatile uint32_t grx_reply[15];

static volatile unsigned grx_stream_active;
static void grx_stream_idle_poll(void);
static int grx_owns_gpu(void) { return grx_claimed!=0||grx_stream_active; }
static uint64_t grx_ticks(void *unused)
{ (void)unused;XTime t;XTime_GetTime(&t);return t; }

static int grx_send_scene(void *unused,unsigned scene,uint32_t epoch,uint32_t sequence)
{
    uint32_t bad,detail,isr;int result;(void)unused;
    unsigned paired=grx_profile>=3&&scene>5;
    unsigned batch=paired?(scene-5)*2-1:scene-5;
    if(scene<5 ? !grx_probe_scene(&grx_source,scene,0x4120+scene) :
       !grx_workload_packet(&grx_source,grx_profile,batch))return 0;
    if(paired){
        if(!grx_workload_packet(&grx_source_second,grx_profile,batch+1))return 0;
        result=grx_prepare_tagged_pair(&grx_packet_private,(const volatile uint32_t*)grx_source.bytes,
                            grx_source.words,(const volatile uint32_t*)grx_source_second.bytes,
                            grx_source_second.words,epoch,sequence,&bad,&detail);
    }else result=grx_prepare_tagged(&grx_packet_private,(const volatile uint32_t*)grx_source.bytes,
                            grx_source.words,epoch,sequence,&bad,&detail);
    if(result){grx_detail=(unsigned)result;return 0;}
    result=xacpgpu_fifo_send_be(grx_packet_private.wire,grx_packet_private.words,&isr);
    if(result){grx_detail=0x10000u|(unsigned)(-result);return 0;}
    return 1;
}

static int grx_verify_scene(void *unused,unsigned scene)
{
    unsigned checked,bad;(void)unused;
    /* Completion has retired all responses. Invalidate only now, before CPU
     * readback; never clean stale CPU lines over GPU-written pixels. */
    unsigned bytes=scene<5?GRX_PROBE_BYTES:grx_workload_width(grx_profile)*grx_workload_height(grx_profile)*2;
    Xil_DCacheInvalidateRange(GRX_PROBE_COLOR,bytes);
    bad=scene<5?grx_probe_verify((const volatile uint16_t*)GRX_PROBE_COLOR,scene,&checked):
        grx_workload_verify((const volatile uint16_t*)GRX_WORKLOAD_COLOR,grx_profile,&checked);
    grx_checked+=checked;
    if(bad){grx_detail=bad;return 0;}
    xacpgpu_safe_state=grx_packet_private.next_state;
    return 1;
}

static void grx_publish(void)
{
    uint64_t now=grx_ticks(0);uint32_t cpsr;
    /* The command handler may interrupt the idle poll. Publish one consistent
     * snapshot, including both halves of each 64-bit timer. */
    __asm__ volatile("mrs %0,cpsr\n\tcpsid if":"=r"(cpsr)::"memory");
    grx_reply[0]=GRX_MEASURE_MAGIC;grx_reply[1]=1;
    grx_reply[2]=grx_meter.state;grx_reply[3]=grx_meter.error;
    grx_reply[4]=grx_meter.frames;grx_reply[5]=grx_meter.target;
    grx_reply[6]=(uint32_t)grx_meter.elapsed;grx_reply[7]=grx_meter.elapsed>>32;
    grx_reply[8]=(uint32_t)now;grx_reply[9]=now>>32;
    grx_reply[10]=grx_checked;grx_reply[11]=grx_detail;grx_reply[12]=grx_claimed;
    grx_reply[13]=31;grx_reply[14]=grx_profile;
    __asm__ volatile("msr cpsr_c,%0"::"r"(cpsr):"memory");
}

static void grx_firmware_poll(void)
{
    grx_stream_idle_poll();
    if(!grx_claimed)return;
    const grx_measure_io io={0,grx_send_scene,grx_verify_scene,grx_ticks,
                            {&grx_transport,grx_mmio_read,grx_mmio_write}};
    if(grx_queued) {
        uint32_t id,status;grx_queued=0;grx_detail=0;grx_checked=0;
        grx_meter=(grx_measure){0};
        /* These checks run before touching texture/framebuffer memory. The
         * candidate must be cold-booted; this is not a stock-firmware probe. */
        if(!grx_mmio_init(&grx_transport) ||
           !grx_mmio_read(&grx_transport,0x1c,&id) || id!=0x47525831u ||
           !grx_mmio_read(&grx_transport,0,&status) || status ||
           ((Xil_In32(XACPGPU_HP3_RDCTRL)|Xil_In32(XACPGPU_HP3_WRCTRL))&XACPGPU_HP3_32BITEN)) {
            grx_meter.state=GRX_MEASURE_FAULT;grx_meter.error=GRX_MEASURE_RETIRE;
            grx_detail=0x20001;grx_publish();return;
        }
        /* Fixed firmware-owned sandbox, used only while the cooperative host
         * holds the existing GPU/Core1 application reservation. */
        volatile uint16_t *texture=(volatile uint16_t*)GRX_PROBE_TEXTURE;
        for(unsigned i=0;i<1024;i++)texture[i]=grx_probe_texel(i);
        Xil_DCacheFlushRange(GRX_PROBE_TEXTURE,2048);
        unsigned color_bytes=GRX_PROBE_BYTES;
        if(grx_profile){
            texture=(volatile uint16_t*)GRX_WORKLOAD_TEXTURE;
            for(unsigned i=0;i<1024;i++)texture[i]=grx_probe_texel(i);
            Xil_DCacheFlushRange(GRX_WORKLOAD_TEXTURE,2048);
            color_bytes=grx_workload_width(grx_profile)*grx_workload_height(grx_profile)*2;
        }
        Xil_DCacheFlushRange(GRX_PROBE_COLOR,color_bytes);
        /* At either supported CPU clock this exceeds one second. Frequency
         * calibration belongs to the host; this is only a liveness deadline. */
        (void)grx_measure_begin_batches(&grx_meter,grx_epoch,grx_target,800000000ull,
                                     grx_profile?grx_workload_submissions(grx_profile):1);
    }
    grx_measure_poll(&grx_meter,&io);
    grx_publish();
}

static void grx_firmware_command(void *data)
{
    unsigned char *p=data;uintptr_t aligned=(uintptr_t)data&~31u;
    Xil_DCacheInvalidateRange(aligned,64);
    unsigned op=zc_get(p),abi=zc_get(p+4),frames=zc_get(p+8),epoch=zc_get(p+12);
    unsigned profile=abi==2?zc_get(p+16):0;
    uint32_t error=0;
    if((abi!=1&&abi!=2)||op>2||profile>4)error=0x30001;
    else if(op==1) {
        if(grx_owns_gpu()||!zc_idle()||xacpgpu_submit_pending||xacpgpu_test2_pending||
           !frames||frames>1000||!epoch||epoch==grx_last_epoch)error=0x30002;
        else {
            grx_target=frames;grx_epoch=epoch;grx_profile=profile;
            grx_last_epoch=epoch;grx_claimed=1;grx_queued=1;
            for(unsigned i=0;i<15;i++)grx_reply[i]=0;
            grx_reply[5]=frames;
        }
    } else if(op==2) {
        if(grx_reply[2]!=GRX_MEASURE_DONE||grx_queued)error=0x30003;
        else grx_claimed=0; /* Faults never release ownership through CLOSE. */
    }
    for(unsigned i=0;i<15;i++)zc_put(p+4*i,grx_reply[i]);
    uint64_t now=grx_ticks(0);
    zc_put(p+32,(uint32_t)now);zc_put(p+36,now>>32);
    if(error)zc_put(p+12,error);
    zc_put(p,GRX_MEASURE_MAGIC);zc_put(p+4,abi==2?2:1);zc_put(p+48,grx_claimed);
    zc_put(p+52,31);zc_put(p+56,grx_profile);
    Xil_DCacheFlushRange(aligned,64);
}

#include "stream_firmware.inc"
