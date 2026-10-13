/* GPL-2.0-or-later. Optional measurement; never changes timer configuration. */
#pragma once
#include "profile_plan.h"
#ifdef DB_NATIVE
static inline void pc_prof_enter(unsigned){}
static inline void pc_prof_leave(){}
static inline void pc_prof_unwind(){}
#else
extern bool pc_prof_enabled;
uint64_t pc_clock_ticks();
void pc_prof_start();void pc_prof_finish();
void pc_prof_unwind();
void pc_prof_push(unsigned);void pc_prof_pop();
static inline void pc_prof_enter(unsigned category){if(pc_prof_enabled)pc_prof_push(category);}
static inline void pc_prof_leave(){if(pc_prof_enabled)pc_prof_pop();}
#endif
