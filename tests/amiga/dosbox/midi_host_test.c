/* Fake CAMD/clock; compile the real host body after its platform includes. */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "midi_host.h"
#include "midi_copy_plan.h"
typedef uint32_t ULONG;typedef int32_t LONG;typedef uint8_t UBYTE;
struct Device {int unused;};struct Library {int unused;};struct MidiNode {int unused;};struct MidiLink {int unused;};
struct TagItem {ULONG ti_Tag,ti_Data;};struct timeval {uint32_t tv_secs,tv_micro;};
enum {MIDI_Name=1,MIDI_MsgQueue,MIDI_SysExSize,TAG_DONE,MLINK_Location,MLINK_Name,MLINK_Parse,MLTYPE_Sender};
#define TRUE 1
static struct Library lib;static struct MidiNode fake_node;static struct MidiLink fake_link;
static uint32_t now_us;static int connected=1;static unsigned sent,pulled,pulls;static uint8_t output[16384],storage[0x190000];
static struct Library *OpenLibrary(const char*n,unsigned version){(void)n;(void)version;return &lib;}
static void CloseLibrary(struct Library*p){(void)p;}
static struct MidiNode *CreateMidiA(struct TagItem*p){(void)p;return &fake_node;}
static void DeleteMidi(struct MidiNode*p){(void)p;}
static struct MidiLink *AddMidiLinkA(struct MidiNode*n,unsigned type,struct TagItem*p){(void)n;(void)type;(void)p;return &fake_link;}
static void RemoveMidiLink(struct MidiLink*p){(void)p;}
static int MidiLinkConnected(struct MidiLink*p){(void)p;return connected;}
static void ParseMidi(struct MidiLink*p,const UBYTE*d,unsigned n){(void)p;assert(sent+n<=sizeof(output));memcpy(output+sent,d,n);sent+=n;}
static void PutMidi(struct MidiLink*p,ULONG n){(void)p;(void)n;}
static void GetSysTime(struct timeval*t){t->tv_secs=now_us/1000000;t->tv_micro=now_us%1000000;}
#include "midi_host_body.inc"
static void pull(volatile void*p,size_t n,void*ctx){
 size_t off=(volatile uint8_t*)p-storage;(void)ctx;
 if(off==PC_MIDI_WRITE){assert(n==4);return;}
 assert(off>=PC_MIDI_DATA&&off+n<=PC_MIDI_DATA+PC_MIDI_SIZE&&n<=16);
 unsigned r=ad_get(storage,PC_MIDI_READ),w=ad_get(storage,PC_MIDI_WRITE);
 for(unsigned i=0;i<n;i++)assert(((off-PC_MIDI_DATA+i-r)&(PC_MIDI_SIZE-1))<(unsigned)(w-r));
 pulled+=n;pulls++;
}
static void push(volatile void*p,size_t n,void*ctx){(void)p;(void)n;(void)ctx;}
static void barrier(void*ctx){(void)ctx;}
static const struct ad_io io={pull,push,barrier,NULL,NULL};
static void begin(unsigned mode,unsigned cursor){
 sent=pulled=pulls=0;now_us=0;connected=1;memset(storage,0,sizeof(storage));
 assert(db_midi_host_open(storage,&io,mode,"test"));ad_put(storage,PC_MIDI_READ,cursor);ad_put(storage,PC_MIDI_WRITE,cursor);
}
static void enqueue(const uint8_t*p,unsigned n){unsigned w=ad_get(storage,PC_MIDI_WRITE);for(unsigned i=0;i<n;i++)storage[PC_MIDI_DATA+((w+i)&4095)]=p[i];ad_put(storage,PC_MIDI_WRITE,w+n);}
int main(void){
 uint8_t notes[64];for(unsigned i=0;i<sizeof(notes);i++)notes[i]=(uint8_t)(i%3==0?0x90:60+i%12);
 begin(0,0);assert(db_midi_host_service()==1&&!pulled);db_midi_host_close();
 begin(2,UINT32_MAX-3);enqueue(notes,sizeof(notes));
 assert(db_midi_host_service()==2&&sent==16&&pulled==16&&pulls==2);
 assert(db_midi_host_service()==2&&sent==16);now_us=5119;assert(db_midi_host_service()==2&&sent==16);
 now_us=5120;assert(db_midi_host_service()==2&&sent==32);
 for(unsigned i=0;i<2;i++){now_us+=5120;assert(db_midi_host_service()==2);}
 assert(sent==64&&pulled==64&&!memcmp(output,notes,64));db_midi_host_close();
 begin(1,4093);uint8_t sysex[]={0xf0,0x7d,1,2,3,0xf7,0x90,60,64};enqueue(sysex,sizeof(sysex));
 assert(db_midi_host_service()==2&&sent==6&&ad_get(storage,PC_MIDI_READ)==4099);
 now_us=301919;assert(db_midi_host_service()==2&&sent==6);now_us=301920;
 assert(db_midi_host_service()==2&&sent==9&&!memcmp(output,sysex,9));
 now_us+=10000;connected=0;assert(!db_midi_host_service());db_midi_host_close();
 begin(2,0);ad_put(storage,PC_MIDI_WRITE,4097);assert(!db_midi_host_service());db_midi_host_close();
 puts("PASS real CAMD service: byte identity, counter/ring wrap, 16-byte pacing, SysEx gap, off/disconnect/overflow");
}
