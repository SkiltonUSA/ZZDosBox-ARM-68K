/* GPL-2.0-or-later. Platform bootstrap for the upstream DOSBox shell. */
#include "shell.h"
#include "callback.h"
#include "regs.h"
#include "support.h"
#include "shell_api.h"
#include <string.h>
struct Message{const char *name,*text;};static Message messages[160];static unsigned message_count;
void MSG_Add(const char *name,const char *text){if(message_count>=160)E_Exit("Message capacity");messages[message_count++]={name,text};}
const char *MSG_Get(const char *name){for(unsigned i=0;i<message_count;i++)if(!strcmp(name,messages[i].name))return messages[i].text;return name;}
extern void zz_shell_messages();
static Bitu bios_video(){
 if(reg_ah==0&&reg_al==3){const uint8_t clear[]={27,'[','2','J',27,'[','H'};Bit16u n=sizeof(clear);DOS_WriteFile(STDOUT,const_cast<uint8_t *>(clear),&n);return CBRET_NONE;}
 E_Exit("BIOS video AH=%02x AL=%02x not implemented at %04x:%04x",reg_ah,reg_al,SegValue(cs),reg_ip);
}
static Bitu missing_bios(){E_Exit("PC BIOS not implemented at %04x:%04x AX=%04x",SegValue(cs),reg_ip,reg_ax);}
void zz_shell_run(){
 message_count=0;zz_shell_messages();
 Bit16u env=DOS_FIRST_SHELL+19;DOS_PSP parent(dos.psp());
 DOS_MCB mcb(env-1);mcb.SetPSPSeg(dos.psp());mcb.SetSize(DOS_MEM_START-env);mcb.SetType(0x4d);
 const char environment[]="PATH=C:\\\0COMSPEC=C:\\COMMAND.COM\0\0\1\0C:\\COMMAND.COM\0";
 MEM_BlockWrite(PhysMake(env,0),environment,sizeof(environment));parent.SetEnvironment(env);
 Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,bios_video,CB_IRET,"console clear");RealSetVec(0x10,CALLBACK_RealPointer(cb));
 Bitu missing=CALLBACK_Allocate();CALLBACK_Setup(missing,missing_bios,CB_IRET,"missing PC BIOS");
 const unsigned interrupts[]={0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x1a};
 for(unsigned i:interrupts)RealSetVec(i,CALLBACK_RealPointer(missing));
 first_shell=new DOS_Shell();first_shell->Run();
}
void zz_shell_cleanup(){if(first_shell){while(first_shell->bf)delete first_shell->bf;delete first_shell;first_shell=nullptr;}}
