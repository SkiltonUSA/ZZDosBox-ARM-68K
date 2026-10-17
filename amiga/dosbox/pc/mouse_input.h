/* GPL-2.0-or-later. Consume bounded host events on the emulation thread. */
#pragma once
#include "mouse.h"
#include "pc_wire.h"
#include "mouse_geometry.h"
extern "C" bool db_mouse_relative();
static unsigned mouse_buttons,mouse_x,mouse_y;
static bool mouse_position;
void db_mouse_reset(){mouse_buttons=mouse_x=mouse_y=0;mouse_position=false;}
static void db_mouse_fence(){
#ifdef DB_NATIVE
 __atomic_thread_fence(__ATOMIC_SEQ_CST);
#else
 __asm__ volatile("dsb sy":::"memory");
#endif
}
static void db_mouse_tick(volatile uint8_t *b){
 unsigned w=ad_get(b,PC_MOUSE_WRITE),r=ad_get(b,PC_MOUSE_READ);db_mouse_fence();
 if(w-r>PC_MOUSE_SIZE)E_Exit("Mouse ring bound");
 /* One notification per tick avoids overflowing the upstream IRQ12 queue.
  * Consecutive motion samples can use their latest endpoint without delaying
  * clicks behind a backlog. Never merge across a button or focus transition. */
 if(w!=r){
  unsigned p=PC_MOUSE_DATA+(r&(PC_MOUSE_SIZE-1))*PC_MOUSE_STRIDE;
  unsigned x=ad_get(b,p),y=ad_get(b,p+4),buttons=ad_get(b,p+8),valid=ad_get(b,p+12);
  if(x>65535||y>65535||buttons>7||valid>1)E_Exit("Mouse event bound");
  while(r+1!=w){
   unsigned next=PC_MOUSE_DATA+((r+1)&(PC_MOUSE_SIZE-1))*PC_MOUSE_STRIDE;
   unsigned nx=ad_get(b,next),ny=ad_get(b,next+4),nb=ad_get(b,next+8),nv=ad_get(b,next+12);
   if(nx>65535||ny>65535||nb>7||nv>1)E_Exit("Mouse event bound");
   if(!pc_mouse_mergeable(mouse_buttons,buttons,valid,nb,nv))break;
   x=nx;y=ny;++r;
  }
  if(valid){
   float dx=mouse_position?((int)x-(int)mouse_x)*(640.0f/65535.0f):0;
   float dy=mouse_position?((int)y-(int)mouse_y)*(480.0f/65535.0f):0;
   Mouse_CursorMoved(dx,dy,x/65535.0f,y/65535.0f,db_mouse_relative());
   mouse_x=x;mouse_y=y;mouse_position=true;
  }else mouse_position=false;
  for(unsigned i=0;i<3;i++)if((buttons^mouse_buttons)&(1u<<i)){
   if(buttons&(1u<<i))Mouse_ButtonPressed(i);else Mouse_ButtonReleased(i);
  }
  mouse_buttons=buttons;db_mouse_fence();ad_put(b,PC_MOUSE_READ,r+1);
 }
}
