/* GPL-2.0-or-later. Shared vertex-domain validation for the supported subset. */
#ifndef GRX_VERTEX_H
#define GRX_VERTEX_H
#include "translator.h"
static inline int grx_vertex_valid(const grx_vertex *v){
 return __builtin_isfinite(v->x)&&__builtin_isfinite(v->y)&&
  __builtin_isfinite(v->r)&&v->r>=0&&v->r<=255&&
  __builtin_isfinite(v->g)&&v->g>=0&&v->g<=255&&
  __builtin_isfinite(v->b)&&v->b>=0&&v->b<=255&&
  __builtin_isfinite(v->a)&&v->a>=0&&v->a<=255&&
  __builtin_isfinite(v->sow)&&__builtin_isfinite(v->tow)&&
  __builtin_isfinite(v->oow)&&v->oow>0;
}
#endif
