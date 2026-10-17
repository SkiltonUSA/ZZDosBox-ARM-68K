/* GPL-2.0-or-later. Amiga digital stick presented as a PC two-axis gameport. */
#pragma once
#include "joystick.h"
#include "pc_wire.h"
extern "C" volatile uint8_t *db_shared();
static inline void db_joystick_tick(){
 unsigned state=ad_get(db_shared(),PC_JOYSTICK_STATE);
 JOYSTICK_Move_X(0,((state&8)?1.0f:0.0f)-((state&4)?1.0f:0.0f));
 JOYSTICK_Move_Y(0,((state&2)?1.0f:0.0f)-((state&1)?1.0f:0.0f));
 JOYSTICK_Button(0,0,(state&16)!=0);JOYSTICK_Button(0,1,(state&32)!=0);
}
