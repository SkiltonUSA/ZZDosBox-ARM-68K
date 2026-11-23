/* Host timer.device / ARM global-timer calibration. No clock-register writes. */
#pragma once
#include <stdint.h>
static inline uint32_t pc_timer_rate(uint64_t ticks,uint32_t elapsed_us){
 if(elapsed_us<1000000u||elapsed_us>3000000u||ticks>1500000000u)return 0;
 uint32_t measured=(uint32_t)(ticks*1000000u/elapsed_us);
 if(measured>=325000000u&&measured<=342000000u)return 333333333u;
 if(measured>=390000000u&&measured<=410000000u)return 400000000u;
 return 0;
}
