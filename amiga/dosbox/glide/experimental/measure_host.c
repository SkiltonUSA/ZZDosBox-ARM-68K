/* Candidate firmware only. Fixed offscreen scene throughput, not GTA FPS. */
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
#include <stdlib.h>
#include <string.h>
struct ExpansionBase *ExpansionBase;
struct Device *TimerBase;
static volatile UWORD *command;
static volatile ULONG *mailbox;
static ULONG reply[15];
static unsigned profile;
static ULONG now(void)
{struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
static unsigned long long ticks(const ULONG *r,unsigned low)
{return ((unsigned long long)r[low+1]<<32)|r[low];}
static int request(unsigned op,unsigned frames,unsigned epoch)
{
    unsigned n;ULONG start;
    if(*command)return 0;
    for(n=0;n<15;n++)mailbox[n]=0;
    mailbox[0]=op;mailbox[1]=profile?2:1;mailbox[2]=frames;mailbox[3]=epoch;mailbox[4]=profile;
    CacheClearE((APTR)mailbox,64,CACRF_ClearD);
    start=now();*command=0x7a20;
    while(*command!=2&&*command!=3&&(ULONG)(now()-start)<1000000u) {}
    if(*command!=2)return 0;
    CacheClearE((APTR)mailbox,64,CACRF_ClearD);
    for(n=0;n<15;n++)reply[n]=mailbox[n];
    *command=0;
    return reply[0]==0x47524d31&&reply[1]==(profile?2u:1u)&&
           (!profile||(reply[13]&(1u<<profile)));
}
static ULONG calibrate(void)
{
    ULONG a,b,c,d;unsigned long long t;
    a=now();if(!request(0,0,0))return 0;b=now();t=ticks(reply,8);
    Delay(25);
    c=now();if(!request(0,0,0))return 0;d=now();
    if((ULONG)(b-a)>5000||(ULONG)(d-c)>5000)return 0;
    unsigned long long us=((unsigned long long)(ULONG)(c-a)+(ULONG)(d-b))/2u;
    if(us<400000)return 0;
    unsigned long long rate=(ticks(reply,8)-t)*1000000ull/us;
    return rate<=0xffffffffull?(ULONG)rate:0;
}
int main(int argc,char **argv)
{
    struct MsgPort *owner=NULL,*port=NULL;struct timerequest *timer=NULL;
    struct ConfigDev *cd;int claimed=0,opened=0,rc=20,active=0;
    unsigned frames=100,seen=0;ULONG base,start,total;unsigned long long elapsed;
    ULONG before,after;unsigned long long fps_milli;
    if(argc<2||strcmp(argv[1],"--candidate")){puts("Use --candidate [frames 1..1000] [profile 0=tiny,1=320x240,2=640x480,3=320x240 paired,4=640x480 paired]. Requires matching experimental firmware.");return 20;}
    if(argc>2){char *end;unsigned long v=strtoul(argv[2],&end,10);if(*end||v<1||v>1000)return 20;frames=v;}
    if(argc>3){char *end;unsigned long v=strtoul(argv[3],&end,10);if(*end||v>4)return 20;profile=v;}
    if(argc>4)return 20;
    ExpansionBase=(struct ExpansionBase*)OpenLibrary("expansion.library",37);if(!ExpansionBase)goto done;
    owner=CreateMsgPort();port=CreateMsgPort();if(!owner||!port)goto done;
    owner->mp_Node.ln_Name="Sixies.ARM.Debug.Owner";
    Forbid();if(!FindPort(owner->mp_Node.ln_Name)){AddPort(owner);claimed=1;}Permit();
    if(!claimed){puts("Another ARM application is active.");goto done;}
    cd=FindConfigDev(NULL,0x6d6e,4);if(!cd)goto done;base=(ULONG)cd->cd_BoardAddr;
    command=(volatile UWORD*)(base+0x64);
    if(*(volatile UWORD*)(base+0xc0)!=0x0113||*command)goto done;
    mailbox=(volatile ULONG*)(base+0x10000u+0x04000000u);
    timer=(struct timerequest*)CreateIORequest(port,sizeof(*timer));if(!timer)goto done;
    if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)timer,0))goto done;
    opened=1;TimerBase=timer->tr_node.io_Device;
    before=calibrate();if(before<300000000u||before>450000000u){puts("Timer calibration failed.");goto done;}
    static const char *names[]={"64x72 solid triangle","320x240 / 320 textured triangles / 65 submissions",
        "640x480 / 320 textured triangles / 65 submissions","320x240 / 320 textured triangles / 33 submissions",
        "640x480 / 320 textured triangles / 33 submissions"};
    printf("Global timer %u Hz; offscreen %s, %u frames\n",before,names[profile],frames);
    active=1;
    if(!request(1,frames,now()|1u)||reply[3]||!reply[12]||(profile&&reply[14]!=profile))goto done;
    start=now();
    do {
        Delay(1);
        if(!request(0,0,0)||reply[3]||reply[2]==4)goto done;
        if(reply[4]!=seen){seen=reply[4];printf("GPU completed %u/%u\n",seen,frames);fflush(stdout);}
        if((ULONG)(now()-start)>30000000u){puts("Measurement deadline expired.");goto done;}
    } while(reply[2]!=3);
    total=reply[4];elapsed=ticks(reply,6);
    if(total!=frames||!elapsed||reply[10]<(profile?19206u:20000u))goto done;
    after=calibrate();
    if((unsigned long long)after*100< (unsigned long long)before*99 || (unsigned long long)after*100>(unsigned long long)before*101){puts("Clock calibration changed; no FPS result.");goto done;}
    if(!request(2,0,0)||reply[3]||reply[12])goto done;
    active=0;
    fps_milli=(unsigned long long)total*((before+after)/2u)*1000ull/elapsed;
    printf("GPU_COMPLETED frames=%u ticks=%llu FPS=%llu.%03u\n",total,elapsed,
           fps_milli/1000ull,(unsigned)(fps_milli%1000ull));
    puts("Offscreen batch throughput including Core0 submission/polling; NOT gameplay or display FPS.");
    rc=0;
done:
    if(active) {
        if(reply[0]==0x47524d31&&(reply[1]==1||reply[1]==2))
            printf("Last valid reply: state=%u error=0x%08x completed=%u/%u checked=%u detail=0x%08x claimed=%u\n",
                   reply[2],reply[3],reply[4],reply[5],reply[10],reply[11],reply[12]);
        puts("STOP: GPU session was not cleanly closed. Do not start another GPU client; cold reboot required.");
    }
    if(opened)CloseDevice((struct IORequest*)timer);
    if(timer)DeleteIORequest((struct IORequest*)timer);
    if(port)DeleteMsgPort(port);
    if(claimed){Forbid();RemPort(owner);Permit();}
    if(owner)DeleteMsgPort(owner);
    if(ExpansionBase)CloseLibrary((struct Library*)ExpansionBase);
    return rc;
}
