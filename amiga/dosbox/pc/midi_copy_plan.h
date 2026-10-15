/* GPL-3.0-or-later. Pull only the published, bounded MIDI batch.
 * The producer cannot reuse these bytes until the consumer publishes READ. */
#pragma once
#include <stdint.h>
struct pc_midi_span {unsigned offset,size;};
static inline unsigned pc_midi_spans(unsigned read,unsigned count,unsigned capacity,struct pc_midi_span spans[2]){
 unsigned at,first;
 if(!capacity||(capacity&(capacity-1))||!count||count>capacity)return 0;
 at=read&(capacity-1);first=capacity-at;if(first>count)first=count;
 spans[0].offset=at;spans[0].size=first;
 if(first==count)return 1;
 spans[1].offset=0;spans[1].size=count-first;return 2;
}
