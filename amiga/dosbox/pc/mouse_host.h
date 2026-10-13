/* GPL-3.0-or-later. Intuition input -> owned ARM mailbox, no host pointer warp. */
#include "mouse_geometry.h"
static UWORD *window_mouse_pointer;
static int window_mouse_hidden;
static unsigned host_mouse_buttons,host_mouse_events,host_mouse_down,host_mouse_up;
static int mouse_room(void){
 ULONG w=ad_get(shared,PC_MOUSE_WRITE);syncio->pull(shared+PC_MOUSE_READ,4,NULL);
 return w-ad_get(shared,PC_MOUSE_READ)<PC_MOUSE_SIZE;
}
static int mouse_publish(unsigned x,unsigned y,unsigned buttons,int valid){
 ULONG w,p;if(!mouse_room())return 0;w=ad_get(shared,PC_MOUSE_WRITE);
 p=PC_MOUSE_DATA+(w&(PC_MOUSE_SIZE-1))*PC_MOUSE_STRIDE;
 ad_put(shared,p,x);ad_put(shared,p+4,y);ad_put(shared,p+8,buttons);ad_put(shared,p+12,valid);
 syncio->push(shared+p,PC_MOUSE_STRIDE,NULL);publish(PC_MOUSE_WRITE,w+1);host_mouse_events++;
 if(buttons&~host_mouse_buttons)host_mouse_down++;
 if(host_mouse_buttons&~buttons)host_mouse_up++;
 host_mouse_buttons=buttons;return 1;
}
static void mouse_pointer(int inside){
 if(fullscreen||!window||!window_mouse_pointer)return;
 if(inside&&!window_mouse_hidden){SetPointer(window,window_mouse_pointer,1,16,0,0);window_mouse_hidden=1;}
 if(!inside&&window_mouse_hidden){ClearPointer(window);window_mouse_hidden=0;}
}
static int mouse_release(void){mouse_pointer(0);return mouse_publish(0,0,0,0);}
static int mouse_event(ULONG cl,UWORD code,WORD x,WORD y){
 struct pc_mouse_point p;int left=0,top=0;unsigned width=640,height=480,buttons=host_mouse_buttons,bit=0;
 if(cl!=IDCMP_MOUSEMOVE&&cl!=IDCMP_MOUSEBUTTONS)return 1;
 if(!fullscreen){
  int cw=window->Width-window->BorderLeft-window->BorderRight,ch=window->Height-window->BorderTop-window->BorderBottom;
  if(!window_ready||cw<=0||ch<=0)return 1;
  width=paint_width<(unsigned)cw?paint_width:(unsigned)cw;height=paint_height<(unsigned)ch?paint_height:(unsigned)ch;
  left=window->BorderLeft+(cw-width)/2;top=window->BorderTop+(ch-height)/2;
 }
 p=pc_mouse_map(x,y,left,top,width,height);mouse_pointer(p.inside);
 if(cl==IDCMP_MOUSEBUTTONS){
  switch(code&~IECODE_UP_PREFIX){case SELECTDOWN:bit=1;break;case MENUDOWN:bit=2;break;case MIDDLEDOWN:bit=4;break;default:return 1;}
  if(code&IECODE_UP_PREFIX)buttons&=~bit;else if(p.inside)buttons|=bit;
 }
 /* Outside presses are window controls; outside releases still reach DOS. */
 if(!p.inside&&!host_mouse_buttons&&buttons==host_mouse_buttons)return 1;
 return mouse_publish(p.x,p.y,buttons,1);
}
