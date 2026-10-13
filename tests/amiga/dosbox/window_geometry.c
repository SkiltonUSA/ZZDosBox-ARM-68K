#include <assert.h>
#include <stdio.h>
#include "window_geometry.h"
int main(void){
 for(unsigned w=320;w<=1000;w++)for(unsigned h=240;h<=760;h++){
  struct pc_window_rect r=pc_window_fit(w,h);
  assert(r.width>=320&&r.width<=960&&r.height>=240&&r.height<=720);
  assert(r.width*3==r.height*4&&r.width<=w&&r.height<=h);
  assert(r.x==(w-r.width)/2&&r.y==(h-r.height)/2);
 }
 puts("PASS resize geometry: 354801 client sizes, 4:3, centred and bounded");
}
