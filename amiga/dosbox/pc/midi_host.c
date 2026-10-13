/* GPL-3.0-or-later. CAMD owns serial/USB MIDI, never open serial.device here. */
#include <proto/exec.h>
#include <proto/camd.h>
#include <proto/timer.h>
#include <devices/timer.h>
extern struct Device *TimerBase;
#include <stdio.h>
#include <string.h>
#include "midi_host.h"
struct Library *CamdBase;
static struct MidiNode *node;
static struct MidiLink *link;
static volatile uint8_t *shared;
static const struct ad_io *syncio;
static unsigned active,bytes,sysex_bytes;
static ULONG next_send;
static ULONG midi_time(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
int db_midi_host_open(volatile uint8_t *s,const struct ad_io *io,unsigned mode,const char *cluster){
 struct TagItem nt[]={{MIDI_Name,(ULONG)"ZZDosBox"},{MIDI_MsgQueue,0},{MIDI_SysExSize,0},{TAG_DONE,0}};
 struct TagItem lt[]={{MLINK_Location,(ULONG)cluster},{MLINK_Name,(ULONG)"ZZDosBox MIDI"},{MLINK_Parse,TRUE},{TAG_DONE,0}};
 db_midi_host_close();shared=s;syncio=io;bytes=sysex_bytes=0;next_send=midi_time();
 ad_put(s,PC_MIDI_MODE,0);ad_put(s,PC_MIDI_WRITE,0);ad_put(s,PC_MIDI_READ,0);
 io->push(s+PC_MIDI_MODE,4,NULL);io->push(s+PC_MIDI_WRITE,68,NULL);io->barrier(NULL);
 if(!mode)return 1;
 if(mode>2||!cluster||!*cluster)return 0;
 CamdBase=OpenLibrary("camd.library",37);if(!CamdBase)goto bad;
 node=CreateMidiA(nt);if(!node)goto bad;
 link=AddMidiLinkA(node,MLTYPE_Sender,lt);if(!link||!MidiLinkConnected(link))goto bad;
 active=mode;ad_put(s,PC_MIDI_MODE,mode);io->push(s+PC_MIDI_MODE,4,NULL);io->barrier(NULL);
 printf("MIDI CAMD=%s MPU401=%s port=330 enabled\n",cluster,mode==1?"intelligent":"uart");return 1;
bad:db_midi_host_close();return 0;
}
int db_midi_host_service(void){
 unsigned r,w,n,i,delay=0;ULONG now;UBYTE data[16];if(!active)return 1;now=midi_time();if((LONG)(now-next_send)<0)return 2;
 syncio->pull(shared+PC_MIDI_WRITE,4,NULL);w=ad_get(shared,PC_MIDI_WRITE);r=ad_get(shared,PC_MIDI_READ);n=w-r;
 if(n>PC_MIDI_SIZE||!MidiLinkConnected(link))return 0;
 if(!n)return 1;
 if(n>sizeof(data))n=sizeof(data);
 syncio->pull(shared+PC_MIDI_DATA,PC_MIDI_SIZE,NULL);
 for(i=0;i<n;i++){
  UBYTE b=shared[PC_MIDI_DATA+((r+i)&(PC_MIDI_SIZE-1))];data[i]=b;
  if(b==0xf0)sysex_bytes=1;else if(sysex_bytes&&b<0xf8){if(++sysex_bytes>8192)return 0;if(b==0xf7){delay=sysex_bytes*320u+(active==1?300000u:0);sysex_bytes=0;n=i+1;break;}}
 }
 /* Serial MIDI wire rate; MT-32 receives a conservative processing gap
  * after each complete SysEx. Waiting is nonblocking in the host loop. */
 next_send=now+(delay?delay:n*320u);
 ParseMidi(link,data,n);bytes+=n;ad_put(shared,PC_MIDI_READ,r+n);syncio->push(shared+PC_MIDI_READ,4,NULL);syncio->barrier(NULL);return 2;
}
void db_midi_host_close(void){
 if(active&&link){unsigned ch;/* Cancel active voices on our output at exit. */
  for(ch=0;ch<16;ch++){PutMidi(link,((ULONG)(0xb0|ch)<<24)|(64UL<<16));PutMidi(link,((ULONG)(0xb0|ch)<<24)|(123UL<<16));}
  printf("MIDI bytes=%u closed\n",bytes);
 }
 if(link)RemoveMidiLink(link);link=NULL;if(node)DeleteMidi(node);node=NULL;
 if(CamdBase)CloseLibrary(CamdBase);CamdBase=NULL;active=0;
}
