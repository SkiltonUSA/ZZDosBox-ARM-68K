/* GPL-3.0-or-later. Bounded 4:3 rendering for the Workbench client area. */
#pragma once
struct pc_window_rect {unsigned width,height,x,y;};
static inline struct pc_window_rect pc_window_fit(unsigned width,unsigned height){
 struct pc_window_rect r;unsigned units=width/4;
 if(units>height/3)units=height/3;
 if(units>240)units=240;
 r.width=units*4;r.height=units*3;r.x=(width-r.width)/2;r.y=(height-r.height)/2;
 return r;
}
