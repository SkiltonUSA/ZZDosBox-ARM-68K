#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "clock_rate.h"
#include "presentation_plan.h"
#include "fps.h"
int main(){
 assert(pc_timer_rate(666666666,2000000)==333333333);
 assert(pc_timer_rate(800000000,2000000)==400000000);
 assert(!pc_timer_rate(0,2000000)&&!pc_timer_rate(400000000,0));
 assert(!pc_timer_rate(400000000,500000)&&!pc_timer_rate(100000000,2000000));
 pc_presentation stock{},fast{};
 for(unsigned i=0;i<600;i++){
  uint64_t a=stock.end+333333333ULL*10/1000,b=fast.end+400000000ULL*10/1000;
  bool show_a=pc_presentation_begin(&stock,1,60,a,333333333),show_b=pc_presentation_begin(&fast,1,60,b,400000000);
  assert(show_a==show_b);
  pc_presentation_end(&stock,a,a+(show_a?333333333ULL*20/1000:0),show_a);
  pc_presentation_end(&fast,b,b+(show_b?400000000ULL*20/1000:0),show_b);
  assert(stock.divisor==fast.divisor&&stock.skipped==fast.skipped);
 }
 for(unsigned hz: {333333333u,400000000u}){
  pc_fps f{};pc_fps_update(&f,1,0,hz);pc_fps_update(&f,1+hz,60,hz);assert(f.tenths==600);
 }
 puts("DOS runtime clock: calibration bounds, matched adaptive cadence and FPS at 666/800 MHz PASS");
}
