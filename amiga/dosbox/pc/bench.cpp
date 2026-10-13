/* GPL-2.0-or-later. PC interpreter workload in the existing owned worker. */
#include <string.h>
#include "dosbox.h"
#include "pc.h"
#include "pic.h"
#include "keyboard.h"
#include "adapter.h"
#include "console_io.h"
#include "bench.h"
#include "pc_wire.h"
#include "profile.h"
#include "options.h"
#include "keys.h"
#include "fs_wire.h"
extern "C" void db_runtime_init();
extern "C" void db_output(const char*);
extern "C" volatile uint8_t *db_shared();
extern "C" const DbConsoleIO *db_stream(int (*)());
void db_pc_tick(){
 unsigned t=ad_get(db_shared(),PC_DURATION);
 if(t&&PIC_Ticks>=t)ad_put(db_shared(),PC_DONE,1);
 volatile uint8_t *b=db_shared();unsigned w=ad_get(b,PC_KEY_WRITE),r=ad_get(b,PC_KEY_READ);
 __asm__ volatile("dsb sy":::"memory");
 if(w-r>PC_KEY_SIZE)E_Exit("Keyboard ring bound");
 while(r!=w){unsigned raw=b[PC_KEY_DATA+(r&(PC_KEY_SIZE-1))];KBD_KEYS key=pc_key(raw);if(key!=KBD_NONE)KEYBOARD_AddKey(key,!(raw&128));r++;}
 __asm__ volatile("dsb sy":::"memory");ad_put(b,PC_KEY_READ,r);
 if(t&&PIC_Ticks==4000)KEYBOARD_AddKey(KBD_enter,true);
 if(t&&PIC_Ticks==4100)KEYBOARD_AddKey(KBD_enter,false);
}
static int (*host_stop)();
static int stop(){return host_stop()||ad_get(db_shared(),PC_DONE);}
extern "C" int mesen_test(int verify,MesenResult*out,int(*halt)(),uint32_t(*clock)()){
 db_runtime_init();host_stop=halt;memset(out,0,sizeof(*out));DosboxResult r{};
 volatile uint8_t *b=db_shared();
 if(ad_get(b,PC_OPTIONS)==PC_OPTIONS_MAGIC){
  db_memory_mb=ad_get(b,PC_OPTIONS+4);db_machine_option=ad_get(b,PC_OPTIONS+8);db_cpu_cycles=ad_get(b,PC_OPTIONS+12);
  if(!pc_options_valid(db_memory_mb,db_machine_option,db_cpu_cycles)){db_output("Invalid PC options");return 1;}
 }
 for(unsigned i=0;i<13;i++)db_boot_name[i]=b[PC_BOOT_NAME+i];
 if(*db_boot_name&&!fs_name_valid(db_boot_name)){db_output("Invalid boot image name");return 1;}
 for(unsigned i=0;i<13;i++)db_hdd_name[i]=b[PC_HDD_NAME+i];
 if(*db_hdd_name&&!fs_name_valid(db_hdd_name)){db_output("Invalid HDD image name");return 1;}
 pc_prof_start();
 uint32_t start=clock();int rc=db_run_io(nullptr,0,ad_get(b,PC_DURATION)?10000000:0xffffffffu,&r,stop,db_stream(stop));
 pc_prof_finish();
 out->ticks=clock()-start;out->checked=verify;out->cycles=r.slices;out->errors=r.fault;out->actual_pc=r.exit_code;out->expected_pc=r.bytes;out->actual_regs=db_frames;out->expected_regs=PIC_Ticks;
 db_output(r.text);return rc;
}
