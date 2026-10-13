/* GPL-2.0-or-later. DOSBox MPU-401 output, batched through owned DDR. */
#include "dosbox.h"
#include "midi_wire.h"
extern "C" volatile uint8_t *db_shared();
extern void db_shell_poll();
#ifdef DB_NATIVE
extern "C" void db_native_midi_service();
static void fence(){__atomic_thread_fence(__ATOMIC_SEQ_CST);}
#else
static void fence(){__asm__ volatile("dsb sy":::"memory");}
#endif
unsigned db_midi_mode;
bool MIDI_Available(){return db_midi_mode!=0;}
void MIDI_RawOutByte(Bit8u byte){
 volatile uint8_t *s=db_shared();unsigned w=ad_get(s,PC_MIDI_WRITE);
 if(!db_midi_mode)return;
 for(;;){
  fence();unsigned used=w-ad_get(s,PC_MIDI_READ);
  if(used>PC_MIDI_SIZE)E_Exit("Invalid MIDI queue");
  if(used<PC_MIDI_SIZE)break;
  db_shell_poll();
#ifdef DB_NATIVE
  db_native_midi_service();
#endif
 }
 s[PC_MIDI_DATA+(w&(PC_MIDI_SIZE-1))]=byte;fence();ad_put(s,PC_MIDI_WRITE,w+1);fence();
}
void db_midi_setup(){db_midi_mode=ad_get(db_shared(),PC_MIDI_MODE);if(db_midi_mode>2)E_Exit("Invalid MIDI mode");}
