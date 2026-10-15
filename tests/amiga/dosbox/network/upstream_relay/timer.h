#pragma once
typedef void TIMER_TickHandler(void);
void TIMER_AddTickHandler(TIMER_TickHandler*);
void TIMER_DelTickHandler(TIMER_TickHandler*);
