/* GPL-2.0-or-later. Exclusive timing with explicit scopes (longjmp-safe). */
#pragma once
#include <stdint.h>
#include <string.h>
enum { PP_OTHER,PP_CPU,PP_EVENTS,PP_CAPTURE,PP_GRANT,PP_COPY,PP_COUNT };
struct pc_profile {
 uint64_t ticks[PP_COUNT],start,last,total;
 uint32_t calls[PP_COUNT],stack[64],depth,error,unwound;
};
static inline void pp_init(struct pc_profile *p,uint64_t now){
 memset(p,0,sizeof(*p));p->start=p->last=now;p->depth=1;
}
static inline void pp_charge(struct pc_profile *p,uint64_t now){
 p->ticks[p->stack[p->depth-1]]+=now-p->last;p->last=now;
}
static inline void pp_enter(struct pc_profile *p,unsigned category,uint64_t now){
 if(p->error)return;
 if(category>=PP_COUNT||p->depth==64){p->error=1;return;}
 pp_charge(p,now);p->stack[p->depth++]=category;p->calls[category]++;
}
static inline void pp_leave(struct pc_profile *p,uint64_t now){
 if(p->error)return;
 if(p->depth<=1){p->error=2;return;}
 pp_charge(p,now);p->depth--;
}
static inline void pp_unwind(struct pc_profile *p,uint64_t now){
 if(!p->error)pp_charge(p,now);
 p->unwound+=p->depth-1;p->depth=1;
}
static inline void pp_finish(struct pc_profile *p,uint64_t now){
 pp_unwind(p,now);p->total=now-p->start;
}
