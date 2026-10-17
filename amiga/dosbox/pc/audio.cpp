/* GPL-2.0-or-later. Batch signed stereo PCM; canonical big-endian for AHI. */
#include "dosbox.h"
#include "audio.h"
#include "audio_wire.h"
extern "C" volatile uint8_t *db_shared();
extern void db_shell_poll(),MIXER_Init(Section*),db_mixer_close(),SBLASTER_Init(Section*),SBLASTER_ShutDown(Section*);
extern void PCSPEAKER_Init(Section*),PCSPEAKER_ShutDown(Section*);
Bit8u adlib_commandreg;
bool db_audio_enabled=true,db_pc_speaker=false;
unsigned db_sound_card=1,db_extra_sound=0;
#ifdef DB_EXPANDED
extern void GUS_Init(Section*),GUS_ShutDown(Section*),CMS_Init(Section*),CMS_ShutDown(Section*),TANDYSOUND_Init(Section*),TANDYSOUND_ShutDown(Section*);
#endif
static bool opened;
static int16_t pending[PC_AUDIO_FRAMES*2];static unsigned frames;
#ifdef DB_NATIVE
extern void db_native_audio_service();
static void fence(){__atomic_thread_fence(__ATOMIC_SEQ_CST);}
#else
static void fence(){__asm__ volatile("dsb sy":::"memory");}
#endif
void db_audio_samples(const int16_t *p,unsigned n){
 if(!opened)return;
 for(unsigned i=0;i<n;i++){
  pending[frames*2]=p[i*2];pending[frames*2+1]=p[i*2+1];
  if(++frames!=PC_AUDIO_FRAMES)continue;
  volatile uint8_t *b=db_shared();unsigned w=ad_get(b,PC_AUDIO_WRITE);
  for(;;){
   fence();unsigned used=w-ad_get(b,PC_AUDIO_READ);
   if(used>PC_AUDIO_SLOTS)E_Exit("PCM queue bound");
   if(used<PC_AUDIO_SLOTS)break;
   db_shell_poll();
#ifdef DB_NATIVE
   db_native_audio_service();
#endif
  }
  unsigned at=PC_AUDIO_DATA+(w&(PC_AUDIO_SLOTS-1))*PC_AUDIO_STRIDE;
  for(unsigned j=0;j<PC_AUDIO_FRAMES*2;j++){uint16_t v=(uint16_t)pending[j];b[at+j*2]=v>>8;b[at+j*2+1]=v;}
  fence();ad_put(b,PC_AUDIO_WRITE,w+1);fence();frames=0;
 }
}
void db_audio_flush(){
 if(opened&&frames){static const int16_t silence[PC_AUDIO_FRAMES*2]={};db_audio_samples(silence,PC_AUDIO_FRAMES-frames);}
}
void db_audio_open(){
 db_audio_enabled=db_sound_card||db_pc_speaker||db_extra_sound;
 if(!db_audio_enabled)return;
 frames=0;MIXER_Init(nullptr);opened=true;if(db_sound_card)SBLASTER_Init(nullptr);PCSPEAKER_Init(nullptr);
#ifdef DB_EXPANDED
 if(db_extra_sound==1)GUS_Init(nullptr);
 else if(db_extra_sound==2)CMS_Init(nullptr);
 else if(db_extra_sound==3)TANDYSOUND_Init(nullptr);
#endif
}
void db_audio_close(){
 if(!opened)return;
#ifdef DB_EXPANDED
 if(db_extra_sound==1)GUS_ShutDown(nullptr);
 else if(db_extra_sound==2)CMS_ShutDown(nullptr);
 else if(db_extra_sound==3)TANDYSOUND_ShutDown(nullptr);
#endif
 PCSPEAKER_ShutDown(nullptr);if(db_sound_card)SBLASTER_ShutDown(nullptr);db_mixer_close();opened=false;frames=0;
}
