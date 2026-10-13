#include <exec/memory.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <stdio.h>
#include "wire.h"
#include "dos_wire.h"
#include <string.h>
#ifdef MB_MMU
#include "mmu.h"
#endif
struct Device *TimerBase;
static volatile uint8_t *shared;
static const struct ad_io *syncio;
static uint32_t sequence;
static ULONG us(void) { struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro; }
static ULONG value(unsigned field) { return ad_get(shared+MB_REPLY,MB_RESULT+field*4); }
static int request(ULONG op) {
    ULONG start=us(),cancel_sent=0;volatile uint8_t *cmd=shared+MB_HOST,*reply=shared+MB_REPLY;
    ULONG token=(sequence+1)^ad_get(shared,ZZ_CONTROL);
    ad_put(cmd,8,token);
    ad_put(cmd,MB_OP,op);syncio->push(cmd+4,60,NULL);syncio->barrier(NULL);
    ad_put(cmd,MB_SEQ,++sequence);syncio->push(cmd,4,NULL);
    do {
        syncio->pull(reply,320,NULL);
        if(ad_get(reply,MB_ACK)==sequence) {
            if(ad_get(reply,68)!=(token^ZZ_XOR)){puts("MAILBOX ECHO FAIL");return 0;}
            if(op==MB_CANCEL && cancel_sent && ad_get(reply,MB_ERROR)==2) {
                printf("CANCEL PASS active_work=1 elapsed_us=%u\n",(ULONG)(us()-start));return 1;
            }
            if(ad_get(reply,MB_ERROR))printf("FAIL op=%u error=%u checked=%u mismatch=%u pc=%04x expected_pc=%04x regs=%08x expected_regs=%08x\n",
                op,(ULONG)ad_get(reply,MB_ERROR),value(0),value(1),value(7),value(8),value(9),value(10));
            return ad_get(reply,MB_ERROR)==0;
        }
        if(SetSignal(0,SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C)return 0;
        if(op==MB_CANCEL && !cancel_sent && (ULONG)(us()-start)>=100000u) {
            ad_put(shared,ZZ_STOP,1);syncio->push(shared+ZZ_STOP,4,NULL);cancel_sent=1;
        }
        Delay(1);
    } while((ULONG)(us()-start)<10000000u);
    syncio->pull(shared+MB_REPLY+64,64,NULL);
    printf("TIMEOUT op=%u phase=%08x\n",op,(ULONG)ad_get(shared,MB_REPLY+64));return 0;
}
static int load(const char *path,unsigned offset,unsigned field) {
 BPTR file=Open((STRPTR)path,MODE_OLDFILE);LONG size;unsigned i;UBYTE buffer[1024];ULONG used=0;
 if(!file){printf("Cannot open %s\n",path);return 0;}
 while((size=Read(file,buffer,sizeof(buffer)))>0){
  if(used+(ULONG)size>DB_CAPACITY){Close(file);return 0;}
  for(i=0;i<(unsigned)size;i++){shared[offset+used+i]=buffer[i];}
  used+=size;
 }
 Close(file);if(size<0)return 0;
 syncio->push(shared+offset,used,NULL);ad_put(shared+MB_HOST,field,used);
 printf("LOADED %s bytes=%u\n",path,used);return 1;
}
int ff_window(volatile uint8_t *base,const struct ad_io *io,int connected) {
 struct MsgPort *port=NULL;struct timerequest *timer=NULL;int opened=0,rc=20;
 BPTR file;ULONG i;uint32_t hash;UBYTE buffer[256];
 (void)connected;shared=base;syncio=io;
 port=CreateMsgPort();if(!port)goto done;
 timer=(struct timerequest *)CreateIORequest(port,sizeof(*timer));if(!timer)goto done;
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)timer,0))goto done;
 opened=1;TimerBase=timer->tr_node.io_Device;
 puts("DOSBOX DOS KERNEL: external programs, isolated file drive");fflush(stdout);
 if(!load("PROGDIR:FileTest.com",DB_PROGRAM,DB_PROGRAM_SIZE)||!load("PROGDIR:INPUT.TXT",DB_INPUT,DB_INPUT_SIZE))goto done;
 if(!request(MB_VERIFY))goto done;
 printf("COM exit=%u slices=%u output_bytes=%u hash=%08x text=%.*s\n",value(7),value(2),value(9),value(3),159,(const char *)(shared+MB_REPLY+128));
 if(value(7)||value(9)!=256||value(3)!=0xf23411c5u)goto done;
 syncio->pull(shared+DB_OUTPUT,256,NULL);hash=2166136261u;
 for(i=0;i<256;i++){buffer[i]=shared[DB_OUTPUT+i];if(buffer[i]!=(UBYTE)(i*13+7))goto done;hash=(hash^buffer[i])*16777619u;}
 if(hash!=value(3))goto done;
 file=Open("PROGDIR:FileTest.out",MODE_NEWFILE);if(!file)goto done;
 i=Write(file,buffer,256);Close(file);if(i!=256)goto done;
 if(!load("PROGDIR:ExeTest.exe",DB_PROGRAM,DB_PROGRAM_SIZE))goto done;
 if(!request(MB_VERIFY))goto done;
 printf("EXE exit=%u slices=%u text=%.*s\n",value(7),value(2),159,(const char *)(shared+MB_REPLY+128));
 if(value(7)||strcmp((const char *)(shared+MB_REPLY+128),"ZZDOS MZ relocation/memory PASS\r\n"))goto done;
 shared[DB_PROGRAM]=0xeb;shared[DB_PROGRAM+1]=0xfe;syncio->push(shared+DB_PROGRAM,2,NULL);ad_put(shared+MB_HOST,DB_PROGRAM_SIZE,2);
 if(!request(MB_CANCEL))goto done;
 puts("DOSBOX_DOS PASS; external COM/EXE, file read/write, MZ relocation, memory allocation, cancellation");fflush(stdout);rc=0;
done:
 if(opened)CloseDevice((struct IORequest *)timer);
 if(timer)DeleteIORequest((struct IORequest *)timer);
 if(port)DeleteMsgPort(port);
 return rc;
}

#ifdef MB_MMU
/* Invoked only after RET1 and synchronous idle reset, before FreeMem. */
int mb_after_stop(volatile uint8_t *base,const struct ad_io *io) {
    ULONG i,bad=0,phase,registers,mask;
    io->pull(base+MB_MMU_REPORT,160,NULL);
    phase=ad_get(base,MB_MMU_REPORT);registers=ad_get(base,MB_MMU_REPORT+4);
    mask=ad_get(base,MB_MMU_REPORT+136);
    io->pull(base+MB_WITNESS-32,1088,NULL);
    for(i=0;i<256;i++)if(ad_get(base,MB_WITNESS+i*4)!=(ad_get(base,ZZ_CONTROL)^i*0x01010101u))bad++;
    for(i=0;i<32;i+=4)if(ad_get(base,MB_WITNESS-32+i) || ad_get(base,MB_WITNESS+1024+i))bad++;
    printf("MMU_RESTORE phase=%u register_mismatches=%08x cpsr_control=%03x expected=%03x ttbr=%08x ccsidr=%08x witness_bad=%u mailbox_rounds=%u\n",
        phase,registers,mask,ad_get(base,MB_MMU_REPORT+16+6*4)&511u,
        ad_get(base,MB_MMU_REPORT+8),ad_get(base,MB_MMU_REPORT+12),bad,sequence);
    for(i=0;i<14;i++)printf("REGISTER n=%u original=%08x restored=%08x\n",i,
        ad_get(base,MB_MMU_REPORT+16+i*4),ad_get(base,MB_MMU_REPORT+80+i*4));
    return phase==3 && !registers && !bad && mask==(ad_get(base,MB_MMU_REPORT+16+6*4)&511u);
}
#endif
