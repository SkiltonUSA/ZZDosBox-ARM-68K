/* GPL-3.0-or-later. Optional physical Amiga port 2 (library index 1), via OS library. */
#include <libraries/lowlevel.h>
#include <proto/lowlevel.h>
struct Library *LowLevelBase;
static ULONG joy_last,joy_state;
static int joystick_open(void){
 joy_last=0;joy_state=~0u;
 publish(PC_JOYSTICK_ENABLED,joystick_enabled);publish(PC_JOYSTICK_STATE,0);
 if(!joystick_enabled)return 1;
 LowLevelBase=OpenLibrary("lowlevel.library",40);
 return LowLevelBase!=NULL;
}
static void joystick_poll(void){
 ULONG now=us(),state,type,bits=0,buttons=0;
 if(!LowLevelBase||now-joy_last<10000u)return;
 joy_last=now;state=ReadJoyPort(1);type=state&JP_TYPE_MASK;
 if(type==JP_TYPE_JOYSTK||type==JP_TYPE_GAMECTLR){
  if(state&JPF_JOY_UP)bits|=1;
 if(state&JPF_JOY_DOWN)bits|=2;
  if(state&JPF_JOY_LEFT)bits|=4;
 if(state&JPF_JOY_RIGHT)bits|=8;
  if(state&JPF_BUTTON_RED)buttons|=1;
  if(state&JPF_BUTTON_BLUE)buttons|=2;
  if(type==JP_TYPE_GAMECTLR){
   if(state&JPF_BUTTON_GREEN)buttons|=4;
   if(state&JPF_BUTTON_YELLOW)buttons|=8;
   if(state&JPF_BUTTON_FORWARD)buttons|=16;
   if(state&JPF_BUTTON_REVERSE)buttons|=32;
  }
 }
 buttons=db_joy_mask(buttons,joystick_buttons);
 if(!pc_display_input_active()){bits=0;buttons=0;}
 bits|=(buttons&3)<<4;pc_display_joystick_buttons(buttons,joystick_buttons,joystick_keys);
 if(bits!=joy_state){publish(PC_JOYSTICK_STATE,bits);joy_state=bits;}
}
static void joystick_close(void){if(LowLevelBase){CloseLibrary(LowLevelBase);LowLevelBase=NULL;}}
