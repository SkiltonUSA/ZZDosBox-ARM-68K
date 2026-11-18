/* Offscreen 68k->Core0->RasterIX validation. No Core1 launch or display changes.
 * On timeout, stop submitting; the operator must establish GPU quiescence
 * before another GPU client. Never mistake XACP DONE for retired pixels. */
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/expansion.h>
#include <proto/timer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "probe_scene.h"
#include "fixtures.h"
#include "probe_wire.h"
struct ExpansionBase *ExpansionBase;
struct Device *TimerBase;
static ULONG now(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
static UWORD swap(UWORD v){return (v<<8)|(v>>8);}
static UWORD request(volatile UWORD *r,UWORD op)
{
    ULONG t=now();*r=op;
    while(*r!=2&&*r!=3&&(ULONG)(now()-t)<1000000u) {
        if(SetSignal(0,0)&SIGBREAKF_CTRL_C)break;
    }
    return *r;
}
static APTR mapped(ULONG base,ULONG arm){return (APTR)(base+0x10000u+arm-0x200000u);}
static int claim(struct MsgPort *p)
{
    int ok;p->mp_Node.ln_Name="Sixies.ARM.Debug.Owner";
    Forbid();ok=FindPort(p->mp_Node.ln_Name)==NULL;if(ok)AddPort(p);Permit();return ok;
}
static int guards(volatile UWORD *pixels,volatile UWORD *tex)
{
    unsigned bad=0;
    for(unsigned i=0;i<64;i++) {
        if(pixels[(int)i-64]!=(UWORD)(0x1234^i))bad++;
        if(pixels[GRX_PROBE_BYTES/2+i]!=(UWORD)(0x5678^i))bad++;
        if(tex[(int)i-64]!=(UWORD)(0xabcd^i))bad++;
        if(tex[1024+i]!=(UWORD)(0xcdef^i))bad++;
    }
    return bad;
}
static unsigned verify(volatile UWORD *pixels,unsigned scene,unsigned *checked)
{
    unsigned bad=0;*checked=0;
    for(unsigned row=0;row<64;row++)for(unsigned x=0;x<64;x++) {
        unsigned y=GRX_PROBE_HEIGHT-1-row;
        int inside=x>=10&&y>=10&&x+y<=62;
        int outside=x<6||y<6||x+y>66;
        if(scene&&!inside&&!outside)continue; /* edge-rule probe comes later */
        unsigned got=swap(pixels[row*64+x]),want=0;
        if(scene&&inside) {
            if(scene==1)want=0xf800;
            if(scene==2)want=0xffe0;
            if(scene>=3) {
                if(x>=29&&x<=35)continue; /* avoid texture boundary/filter ambiguity */
                want=x<32?0xf800:0x001f;
                if(scene==4&&x>32) {
                    unsigned r=got>>11,g=(got>>5)&63,b=got&31;
                    (*checked)++;
                    if(r<15||r>17||g||b<14||b>16)bad++;
                    continue;
                }
            }
        }
        (*checked)++;if(got!=want)bad++;
    }
    return bad;
}
static int capture(const char *prefix,unsigned scene,volatile UWORD *pixels)
{
    char path[256];snprintf(path,sizeof(path),"%s%u.ppm",prefix,scene);
    FILE *file=fopen(path,"wb");if(!file)return 0;
    int ok=fprintf(file,"P6\n64 64\n255\n")>0;
    for(unsigned row=0;row<64&&ok;row++) {
        unsigned char rgb[64*3];
        for(unsigned x=0;x<64;x++) {
            unsigned p=swap(pixels[row*64+x]);
            rgb[3*x]=(p>>11)*255/31;rgb[3*x+1]=((p>>5)&63)*255/63;rgb[3*x+2]=(p&31)*255/31;
        }
        ok=fwrite(rgb,1,sizeof(rgb),file)==sizeof(rgb);
    }
    return fclose(file)==0&&ok;
}
int main(int argc,char **argv)
{
    struct ConfigDev *cd;struct MsgPort *port=NULL,*owner=NULL;
    struct timerequest *timer=NULL;int opened=0,claimed=0,rc=20;
    volatile UWORD *reg=NULL,*pixels,*texture;volatile ULONG *q;volatile UBYTE *stage;
    ULONG base,start;unsigned last=4,marker_base=0x4120;int triangle_marker=0,log_before=0,preread=3;const char *prefix=NULL;
    for(int arg=1;arg<argc;arg++) {
        if(!strcmp(argv[arg],"--clear"))last=0;
        else if(!strcmp(argv[arg],"--solid"))last=1;
        else if(!strcmp(argv[arg],"--triangle-marker"))triangle_marker=1;
        else if(!strcmp(argv[arg],"--log-before-read"))log_before=1;
        else if(!strcmp(argv[arg],"--no-marker-preread"))preread=0;
        else if(!strcmp(argv[arg],"--marker-preclear-only"))preread=1;
        else if(!strcmp(argv[arg],"--marker-preread-only"))preread=2;
        else if(!strcmp(argv[arg],"--marker-base")&&arg+1<argc) {
            char *end;unsigned long value=strtoul(argv[++arg],&end,0);
            if(*end||!value||value>65531)return 20;
            marker_base=(unsigned)value;
        }
        else if(!strcmp(argv[arg],"--capture")&&arg+1<argc&&strlen(argv[arg+1])<200)prefix=argv[++arg];
        else return 20;
    }
    if(triangle_marker&&GRX_PROBE_HEIGHT<72){puts("Triangle marker needs eight reserved rows");return 20;}
    ExpansionBase=(struct ExpansionBase*)OpenLibrary("expansion.library",0);if(!ExpansionBase)return 20;
    owner=CreateMsgPort();if(!owner)goto done;
    claimed=claim(owner);if(!claimed){puts("Core1 occupied; no GPU access");goto done;}
    cd=FindConfigDev(NULL,0x6d6e,4);if(!cd)goto done;base=(ULONG)cd->cd_BoardAddr;
    reg=(volatile UWORD*)(base+0x64);
    if(*(volatile UWORD*)(base+0xc0)!=0x0113||*reg){puts("Unexpected firmware/status; no GPU request");reg=NULL;goto done;}
    port=CreateMsgPort();if(!port)goto done;
    timer=(struct timerequest*)CreateIORequest(port,sizeof(*timer));if(!timer)goto done;
    if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)timer,0))goto done;
    opened=1;TimerBase=timer->tr_node.io_Device;
    q=(volatile ULONG*)(base+0x10000u+0x04000000u);
    stage=(volatile UBYTE*)(base+0x10000u+0x065f0000u);
    if(request(reg,0x0600)!=2){puts("GPU QUERY failed");goto done;}
    CacheClearE((APTR)q,128,CACRF_ClearD);
    printf("GPU id=%08x version=%08x caps=%08x abi=%08x\n",q[9],q[10],q[5],q[8]);
    if(q[9]!=0x52495831||q[10]!=0x20000||(q[5]&9)!=9||q[8]!=0x10000)goto done;
    *reg=0;
    pixels=mapped(base,GRX_PROBE_COLOR);texture=mapped(base,GRX_PROBE_TEXTURE);
    for(unsigned i=0;i<64;i++) {
        pixels[(int)i-64]=0x1234^i;pixels[GRX_PROBE_BYTES/2+i]=0x5678^i;
        texture[(int)i-64]=0xabcd^i;texture[1024+i]=0xcdef^i;
    }
    for(unsigned i=0;i<GRX_PROBE_BYTES/2;i++)pixels[i]=0xa55a;
    for(unsigned i=0;i<1024;i++)texture[i]=swap(grx_probe_texel(i));
    CacheClearE((APTR)(pixels-64),GRX_PROBE_BYTES+256,CACRF_ClearD);
    CacheClearE((APTR)(texture-64),2048+256,CACRF_ClearD);
    for(unsigned scene=0;scene<=last;scene++) {
        grx_packet p;unsigned marker=marker_base+scene,checked,bad,guard,i;
        p.words=triangle_marker?grx_probe_words1[scene]:grx_probe_words0[scene];
        memcpy(p.bytes,triangle_marker?grx_probe_bytes1[scene]:grx_probe_bytes0[scene],p.words*4);
        if((!triangle_marker||!scene)&&!grx_probe_retag(&p,(uint16_t)marker))goto done;
        unsigned expected_crc=grx_probe_crc32(p.bytes,p.words*4);
        if(marker_base==0x4120&&expected_crc!=grx_probe_crc[triangle_marker][scene]){
            puts("STOP: generated fixture CRC mismatch");goto done;
        }
        unsigned marker_offset=(GRX_PROBE_HEIGHT-1)*64,marker_count=64;
        if(triangle_marker&&scene) {
            marker=scene==2?0xffe0:scene==4?0x07e0:0xf800;
            marker_offset=(GRX_PROBE_HEIGHT-3)*64+4;marker_count=25;
        }
        unsigned before=0x10000;
        if(preread&1)CacheClearE((APTR)(pixels+marker_offset),marker_count*2,CACRF_ClearD);
        if(preread&2) {
            before=swap(pixels[marker_offset]);
            if(before==marker){puts("STOP: marker already present before submit");goto done;}
        }
        if(SetSignal(0,0)&SIGBREAKF_CTRL_C){puts("Cancelled before submit");goto done;}
        for(i=0;i<p.words*4;i++)stage[i]=p.bytes[i];
        CacheClearE((APTR)stage,p.words*4,CACRF_ClearD);
        memset((void*)q,0,52);q[0]=0x0603;q[1]=0x065f0000;q[2]=p.words*4;
        CacheClearE((APTR)q,128,CACRF_ClearD);
        start=now();unsigned status=request(reg,0x0603),ack_us=now()-start;
        CacheClearE((APTR)q,128,CACRF_ClearD);
        if(status!=2||q[12]||q[10]!=p.words||q[9]!=expected_crc){
            printf("STOP: submit status=%u error=%x words=%u crc=%08x; GPU completion unknown\n",status,q[12],q[10],q[9]);goto done;
        }
        if(log_before)printf("ACK diagnostic scene=%u status=%u\n",scene,status);
        *reg=0;start=now();
        do {
            CacheClearE((APTR)(pixels+marker_offset),marker_count*2,CACRF_ClearD);
            for(i=0;i<marker_count&&pixels[marker_offset+i]==swap(marker);i++){}
            if(i==marker_count)break;
            if(SetSignal(0,0)&SIGBREAKF_CTRL_C)break;
        }while((ULONG)(now()-start)<1000000u);
        unsigned fence_us=now()-start;
        if(i!=marker_count){printf("STOP: FENCE timeout scene=%u got=%04x want=%04x\n",scene,pixels[marker_offset+i],swap(marker));goto done;}
        CacheClearE((APTR)(pixels-64),GRX_PROBE_BYTES+256,CACRF_ClearD);
        CacheClearE((APTR)(texture-64),2048+256,CACRF_ClearD);
        bad=verify(pixels,scene,&checked);guard=guards(pixels,texture);
        for(i=0;i<1024;i++)if(texture[i]!=swap(grx_probe_texel(i)))guard++;
        /* No log I/O between submit, marker polling and the first readback. */
        printf("SUBMIT scene=%u mode=%s prepare=%u words=%u before=%04x marker=%04x crc=%08x ack_us=%u\n",scene,triangle_marker?"triangle":"memset",preread,p.words,before,marker,q[9],ack_us);
        printf("PIXELS scene=%u checked=%u bad=%u guards=%u fence_us=%u left=%04x right=%04x\n",scene,checked,bad,guard,fence_us,swap(pixels[(GRX_PROBE_HEIGHT-1-12)*64+16]),swap(pixels[(GRX_PROBE_HEIGHT-1-12)*64+40]));
        if(bad||guard) {
            Delay(2);CacheClearE((APTR)(pixels-64),GRX_PROBE_BYTES+256,CACRF_ClearD);
            unsigned late=verify(pixels,scene,&checked);
            printf("LATE READ scene=%u bad=%u (diagnostic only; run still fails)\n",scene,late);
            capture("RAM:GlideFailure-",scene,pixels);
            goto done;
        }
        if(prefix&&!capture(prefix,scene,pixels)){puts("Capture write failed");goto done;}
    }
    rc=0;
done:
    if(reg&&(*reg==2||*reg==3))*reg=0;
    printf("GLIDE PROBE rc=%d\n",rc);
    if(opened)CloseDevice((struct IORequest*)timer);
    if(timer)DeleteIORequest((struct IORequest*)timer);
    if(port)DeleteMsgPort(port);
    if(claimed)RemPort(owner);
    if(owner)DeleteMsgPort(owner);
    CloseLibrary((struct Library*)ExpansionBase);return rc;
}
