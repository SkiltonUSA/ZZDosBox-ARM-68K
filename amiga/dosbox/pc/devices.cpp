/* GPL-2.0-or-later. Native PC hardware foundation; unavailable devices absent. */
#include "dosbox.h"
#include "cpu.h"
#include "callback.h"
#include "vga.h"
#include "bios_disk.h"
#include "pc.h"
#include "pic.h"
#include "keyboard.h"
#include "inout.h"
extern void db_mouse_reset();
extern void MOUSE_Init(Section*);
extern void IO_Init(Section*),PIC_Init(Section*),TIMER_Init(Section*),CMOS_Init(Section*),KEYBOARD_Init(Section*),BIOS_Init(Section*),VGA_Init(Section*),INT10_Init(Section*);
extern void IO_Destroy(Section*),PIC_Destroy(Section*),TIMER_Destroy(Section*),CMOS_Destroy(Section*),BIOS_Destroy(Section*),zz_vga_memory_close();

bool startup_state_numlock=false,startup_state_capslock=false;
extern void db_midi_setup(),MPU401_Init(Section*),MPU401_Destroy(Section*);
#ifdef DB_NETWORK
extern void db_net_device_open(),db_net_device_close(),db_ipx_close();
#endif
static bool initialized;
static unsigned a20_control;
static Bitu read_a20(Bitu,Bitu){return a20_control|(MEM_A20_Enabled()?2:0);}
static void write_a20(Bitu,Bitu value,Bitu){
 if(value&1)E_Exit("XMS: CPU reset via port 0x92 not supported");
 a20_control=value&~2u;MEM_A20_Enable((value&2)!=0);
}
void db_pc_init(){
 CPU_CycleMax=db_cpu_cycles;CPU_Cycles=0;CPU_CycleLeft=db_cpu_cycles;PIC_Ticks=0;
 db_video_reset();IO_Init(nullptr);PIC_Init(nullptr);TIMER_Init(nullptr);CMOS_Init(nullptr);KEYBOARD_Init(nullptr);BIOS_Init(nullptr);VGA_Init(nullptr);INT10_Init(nullptr);MOUSE_Init(nullptr);db_mouse_reset();db_midi_setup();MPU401_Init(nullptr);initialized=true;
#ifdef DB_NETWORK
 db_net_device_open();
#endif
 a20_control=0;IO_RegisterReadHandler(0x92,read_a20,IO_MB);IO_RegisterWriteHandler(0x92,write_a20,IO_MB);MEM_A20_Enable(false);
}
void db_pc_close(){if(!initialized)return;
#ifdef DB_NETWORK
 db_ipx_close();db_net_device_close();
#endif
 MPU401_Destroy(nullptr);BIOS_Destroy(nullptr);TIMER_Destroy(nullptr);CMOS_Destroy(nullptr);PIC_Destroy(nullptr);zz_vga_memory_close();IO_Destroy(nullptr);initialized=false;}
bool SB_Get_Address(Bitu&,Bitu&,Bitu&){return false;}
bool TS_Get_Address(Bitu&,Bitu&,Bitu&){return false;}
void PCSPEAKER_SetCounter(Bitu,Bitu){}
void PCSPEAKER_SetType(Bitu){}
bool JOYSTICK_IsEnabled(Bitu){return false;}
float JOYSTICK_GetMove_X(Bitu){return 0;}float JOYSTICK_GetMove_Y(Bitu){return 0;}
void Mouse_AutoLock(bool){} /* Intuition retains ownership of the host pointer. */
#include "setup.h"
#include "mapper.h"
bool DOS_LayoutKey(Bitu,Bit8u,Bit8u,Bit8u){return false;}
void MAPPER_AddHandler(MAPPER_Handler*,MapKeys,Bitu,const char*,const char*){}
void Section::AddDestroyFunction(SectionFunction,bool){E_Exit("Unexpected module owner");}
