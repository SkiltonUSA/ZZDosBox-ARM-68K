/* GPL-2.0-or-later. PC interpreter workload in the existing owned worker. */
#include <string.h>
#include "dosbox.h"
#ifdef DB_COMPAT
#include "audio.h"
#include "compat.h"
#include "audio_wire.h"
#endif
#include "pc.h"
#include "pic.h"
#include "cpu.h"
#include "keyboard.h"
#include "bios.h"
#include "mem.h"
#include "adapter.h"
#include "console_io.h"
#include "bench.h"
#include "pc_wire.h"
#include "mouse_input.h"
#include "profile.h"
#include "options.h"
#include "keys.h"
#include "fs_wire.h"
extern "C" void db_runtime_init();
extern "C" void db_output(const char*);
extern "C" volatile uint8_t *db_shared();
#ifdef DB_VIDEO_TRACE
extern "C" void db_video_debug_end();
#endif
#ifdef DB_PRESENT_TEST
extern void db_presentation_report();
#endif
extern "C" const DbConsoleIO *db_stream(int (*)());
static unsigned boot_pos;static bool boot_prompt;static char boot_command[160];
static void boot_keys(){
 if(!*boot_command||(!*db_boot_name&&!*db_hdd_name))return;
 if(!boot_prompt){
  unsigned cur=mem_readw(0x450),row=cur>>8,col=cur&255;
  if(row<25&&col>=4&&col<=80){
   const char *prompt="C:\\>";unsigned at=(machine==MCH_HERC?0xb0000:0xb8000)+(row*80+col-4)*2,i;
   for(i=0;i<4&&mem_readb(at+i*2)==(unsigned)prompt[i];i++){}
   if(i==4)boot_prompt=true;
  }
 }
 if(boot_prompt&&boot_pos<=strlen(boot_command)){
  unsigned c=boot_command[boot_pos];if(!c)c=13;
  if(BIOS_AddKeyToBuffer(c==13?0x1c0d:c))boot_pos++;
 }
}
#ifdef DB_EXPANDED
#include "joystick_input.h"
#endif
void db_pc_tick(){
#ifdef DB_EXPANDED
 db_joystick_tick();
 if(!(PIC_Ticks&7u))ad_put(db_shared(),PC_GUEST_TICKS,PIC_Ticks);
#endif
 db_mouse_tick(db_shared());
 unsigned requested=ad_get(db_shared(),PC_CYCLE_REQUEST);
 if(requested){if(requested<500||requested>DB_CYCLES_MAX)E_Exit("CPU speed request out of bounds");if(requested!=db_cpu_cycles){db_cpu_cycles=requested;CPU_CycleMax=requested;}ad_put(db_shared(),PC_CYCLE_APPLIED,requested);}
 unsigned t=ad_get(db_shared(),PC_DURATION);
 if(t&&PIC_Ticks>=t)ad_put(db_shared(),PC_DONE,1);
 volatile uint8_t *b=db_shared();unsigned w=ad_get(b,PC_KEY_WRITE),r=ad_get(b,PC_KEY_READ);
 __asm__ volatile("dsb sy":::"memory");
 if(w-r>PC_KEY_SIZE)E_Exit("Keyboard ring bound");
 while(r!=w){unsigned raw=b[PC_KEY_DATA+(r&(PC_KEY_SIZE-1))];KBD_KEYS key=pc_key(raw);if(key!=KBD_NONE)KEYBOARD_AddKey(key,!(raw&128));r++;}
 __asm__ volatile("dsb sy":::"memory");ad_put(b,PC_KEY_READ,r);
 boot_keys();
 if(t&&PIC_Ticks==4000)KEYBOARD_AddKey(KBD_enter,true);
 if(t&&PIC_Ticks==4100)KEYBOARD_AddKey(KBD_enter,false);
}
static int (*host_stop)();
static int stop(){return host_stop()||ad_get(db_shared(),PC_DONE);}
extern "C" int mesen_test(int verify,MesenResult*out,int(*halt)(),uint32_t(*clock)()){
 db_runtime_init();host_stop=halt;memset(out,0,sizeof(*out));DosboxResult r{};
 volatile uint8_t *b=db_shared();
 if(ad_get(b,PC_OPTIONS)==PC_OPTIONS_MAGIC){
  db_memory_mb=ad_get(b,PC_OPTIONS+4);db_machine_option=ad_get(b,PC_OPTIONS+8);db_cpu_cycles=ad_get(b,PC_OPTIONS+12);db_monochrome=ad_get(b,PC_OPTIONS+16);
  if(db_monochrome>3||!pc_options_valid(db_memory_mb,db_machine_option,db_cpu_cycles)){db_output("Invalid PC options");return 1;}
 }
 if(ad_get(b,PC_HARDWARE_MAGIC)==PC_HARDWARE_VERSION){
  db_cpu_model=ad_get(b,PC_CPU_MODEL);
  unsigned services=ad_get(b,PC_MEMORY_SERVICES);
  if(db_cpu_model>=DB_CPU_COUNT||services>3){db_output("Invalid hardware options");return 1;}
#ifdef DB_COMPAT
  db_vcpi_enabled=(services&1)!=0;db_umb_enabled=(services&2)!=0;
#else
  if(services){db_output("Memory managers require Compat");return 1;}
#endif
 }
 for(unsigned i=0;i<13;i++)db_boot_name[i]=b[PC_BOOT_NAME+i];
 if(*db_boot_name&&!fs_name_valid(db_boot_name)){db_output("Invalid boot image name");return 1;}
 for(unsigned i=0;i<13;i++)db_hdd_name[i]=b[PC_HDD_NAME+i];
 if(*db_hdd_name&&!fs_name_valid(db_hdd_name)){db_output("Invalid HDD image name");return 1;}
 db_vga_console=(ad_get(b,PC_UI_FLAGS)&2)!=0;db_text_attr=ad_get(b,PC_TEXT_COLOURS)&127;
 boot_pos=0;boot_prompt=false;
 for(unsigned i=0;i<sizeof(boot_command);i++)boot_command[i]=b[PC_BOOT_COMMAND+i];
 if(boot_command[159]){db_output("Boot command too long");return 1;}
 for(unsigned i=0;boot_command[i];i++)if((unsigned char)boot_command[i]<32||(unsigned char)boot_command[i]>=127){db_output("Invalid boot command");return 1;}
#ifdef DB_COMPAT
#ifdef DB_EXPANDED
 db_paged_jit=ad_get(b,PC_PAGED_JIT);if(db_paged_jit>1){db_output("Invalid paging JIT mode");return 1;}
 db_extra_sound=ad_get(b,PC_EXTRA_SOUND);
 if(db_extra_sound>3){db_output("Invalid extra sound device");return 1;}
#endif
 db_sound_card=ad_get(b,PC_AUDIO_MODE)&3;db_pc_speaker=(ad_get(b,PC_AUDIO_MODE)&4)!=0;db_ems_enabled=ad_get(b,PC_AUDIO_EMS)!=0;
#endif
 pc_prof_start();
 uint32_t start=clock();int rc=db_run_io(nullptr,0,ad_get(b,PC_DURATION)?10000000:0xffffffffu,&r,stop,db_stream(stop));
#ifdef DB_VIDEO_TRACE
 db_video_debug_end();
#endif
 pc_prof_finish();
#ifdef DB_PRESENT_TEST
 db_presentation_report();
#endif
 out->ticks=clock()-start;out->checked=verify;out->cycles=r.slices;out->errors=r.fault;out->actual_pc=r.exit_code;out->expected_pc=r.bytes;out->actual_regs=db_frames;out->expected_regs=PIC_Ticks;
 db_output(r.text);return rc;
}
