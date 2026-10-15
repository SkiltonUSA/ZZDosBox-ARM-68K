/* GPL-3.0-or-later. Physical CAMD loopback for MIDITEST.COM.
 * Named internal cluster only: never attach to a physical MIDI output. */
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/camd.h>
#include <stdio.h>
#include <string.h>
struct Library *CamdBase;
int main(int argc,char **argv){
 struct MidiNode *n=NULL;struct MidiLink *l=NULL;MidiMsg msg;
 unsigned count=0,errors=0;int rc=20;unsigned char buf[64];
 const unsigned char sysex[]={0xf0,0x7d,1,2,3,0xf7};
 struct TagItem nt[]={{MIDI_Name,(ULONG)"ZZDosBox internal capture"},{MIDI_MsgQueue,65536},{MIDI_SysExSize,8192},{TAG_DONE,0}};
 struct TagItem lt[]={{MLINK_Location,(ULONG)"ZZDosBox.test"},{TAG_DONE,0}};
 CamdBase=OpenLibrary("camd.library",37);if(!CamdBase)return 20;
 n=CreateMidiA(nt);if(!n)goto done;
 l=AddMidiLinkA(n,MLTYPE_Receiver,lt);if(!l)goto done;
 if(argc>1){FILE *ready=fopen(argv[1],"w");if(!ready)goto done;fputs("READY\n",ready);if(fclose(ready))goto done;}
 puts("CAPTURE ready: ZZDosBox.test (internal only)");fflush(stdout);
 for(unsigned i=0;i<3000&&count<5005;i++){
  while(count<5005&&GetMidi(n,&msg)){
   if(count<5000||count==5002){if(msg.mm_Status!=0xf8)errors++;}
   else if(count==5000){if(msg.mm_Status!=0x91||msg.mm_Data1!=60||msg.mm_Data2!=64)errors++;}
   else if(count==5001){if(msg.mm_Status!=0x91||msg.mm_Data1!=62||msg.mm_Data2!=65)errors++;}
   else if(count==5003){if(msg.mm_Status!=0x81||msg.mm_Data1!=60||msg.mm_Data2)errors++;}
   else if(msg.mm_Status!=0xf0||GetSysEx(n,buf,sizeof(buf))!=sizeof(sysex)||memcmp(buf,sysex,sizeof(sysex)))errors++;
   count++;
  }
  Delay(1);
 }
 errors+=GetMidiErr(n);
 printf("CAPTURE messages=%u expected=5005 errors=%u\n",count,errors);
 rc=count==5005&&!errors?0:20;puts(rc?"CAPTURE FAIL":"CAPTURE PASS");fflush(stdout);
 Delay(250); /* Keep the receiver alive while the sender closes. */
done:
 if(l)RemoveMidiLink(l);if(n)DeleteMidi(n);CloseLibrary(CamdBase);return rc;
}
