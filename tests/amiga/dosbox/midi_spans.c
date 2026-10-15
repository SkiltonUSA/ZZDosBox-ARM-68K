#include "midi_copy_plan.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 struct pc_midi_span s[2];unsigned read,n,i,k,total;
 for(read=0;read<8192;read++)for(n=1;n<=16;n++){
  unsigned parts=pc_midi_spans(read,n,4096,s);assert(parts>=1&&parts<=2);total=0;
  for(i=0;i<parts;i++){
   assert(s[i].size&&s[i].offset<4096&&s[i].size<=4096-s[i].offset);
   for(k=0;k<s[i].size;k++)assert(s[i].offset+k==((read+total++)&4095));
  }assert(total==n);
 }
 assert(pc_midi_spans(UINT32_MAX-3,16,4096,s)==2&&s[0].size==4&&s[1].size==12);
 assert(!pc_midi_spans(0,0,4096,s)&&!pc_midi_spans(0,4097,4096,s)&&!pc_midi_spans(0,1,4000,s));
 puts("PASS MIDI publication spans: every ring position, 1..16-byte batches, wrap and invalid bounds");
}
