/* GPL-2.0-or-later. Native PC hardware foundation; unavailable devices absent. */
#include "dosbox.h"
#include "cpu.h"
#include "callback.h"
#include "vga.h"
#include "bios_disk.h"
#include "pc.h"
#include "pic.h"
#include "keyboard.h"
extern void IO_Init(Section*),PIC_Init(Section*),TIMER_Init(Section*),CMOS_Init(Section*),KEYBOARD_Init(Section*),BIOS_Init(Section*),VGA_Init(Section*),INT10_Init(Section*);
extern void IO_Destroy(Section*),PIC_Destroy(Section*),TIMER_Destroy(Section*),CMOS_Destroy(Section*),BIOS_Destroy(Section*),zz_vga_memory_close();

bool startup_state_numlock=false,startup_state_capslock=false;
static bool initialized;
void db_pc_init(){
 CPU_CycleMax=db_cpu_cycles;CPU_Cycles=0;CPU_CycleLeft=db_cpu_cycles;PIC_Ticks=0;
 db_video_reset();IO_Init(nullptr);PIC_Init(nullptr);TIMER_Init(nullptr);CMOS_Init(nullptr);KEYBOARD_Init(nullptr);BIOS_Init(nullptr);VGA_Init(nullptr);INT10_Init(nullptr);initialized=true;
}
void db_pc_close(){if(!initialized)return;BIOS_Destroy(nullptr);TIMER_Destroy(nullptr);CMOS_Destroy(nullptr);PIC_Destroy(nullptr);zz_vga_memory_close();IO_Destroy(nullptr);initialized=false;}
bool SB_Get_Address(Bitu&,Bitu&,Bitu&){return false;}
bool TS_Get_Address(Bitu&,Bitu&,Bitu&){return false;}
void PCSPEAKER_SetCounter(Bitu,Bitu){}
void PCSPEAKER_SetType(Bitu){}
bool JOYSTICK_IsEnabled(Bitu){return false;}
float JOYSTICK_GetMove_X(Bitu){return 0;}float JOYSTICK_GetMove_Y(Bitu){return 0;}
bool Mouse_SetPS2State(bool){return false;}void Mouse_ChangePS2Callback(Bit16u,Bit16u){}
void Mouse_BeforeNewVideoMode(bool){}void Mouse_AfterNewVideoMode(bool){}
#include "setup.h"
#include "mapper.h"
bool DOS_LayoutKey(Bitu,Bit8u,Bit8u,Bit8u){return false;}
void MAPPER_AddHandler(MAPPER_Handler*,MapKeys,Bitu,const char*,const char*){}
void Section::AddDestroyFunction(SectionFunction,bool){E_Exit("Unexpected module owner");}

