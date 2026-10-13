/* GPL-3.0-or-later. Host copy/PAN ownership, independent of OS and hardware. */
#pragma once
#include "flip_plan.h"
struct pc_flow { unsigned copying,ready,grants,limit; };
static inline void pc_flow_init(struct pc_flow *p,unsigned limit){
 p->copying=p->ready=p->grants=0;p->limit=limit;
}
static inline int pc_flow_idle(const struct pc_flow *p,const struct nf_slots *s){
 return !p->copying&&!p->ready&&s->pending<0;
}
static inline int pc_flow_release(struct pc_flow *p,const struct nf_slots *s){
 if(!pc_flow_idle(p,s))return 0;
 p->grants=0;return 1;
}
static inline int pc_flow_can_grant(const struct pc_flow *p,const struct nf_slots *s){
 return !p->copying&&!p->ready&&p->grants<p->limit&&s->render<3&&
  s->front<3&&s->render!=s->front&&(int)s->render!=s->pending;
}
static inline int pc_flow_grant(struct pc_flow *p,const struct nf_slots *s){
 if(!pc_flow_can_grant(p,s))return 0;
 p->copying=1;p->grants++;return 1;
}
static inline int pc_flow_complete(struct pc_flow *p){
 if(!p->copying||p->ready)return 0;
 p->copying=0;p->ready=1;return 1;
}
static inline int pc_flow_queue(struct pc_flow *p,struct nf_slots *s,uint16_t ack){
 if(p->copying||!p->ready||!nf_queue(s,ack))return 0;
 p->ready=0;return 1;
}
