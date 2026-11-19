/* GPL-2.0-or-later. Window-space clipping; interpolate s/w, t/w and 1/w,
 * never deproject UV before clipping. Double intermediates on the cold path
 * avoid float overflow when finite guest coordinates have opposite signs. */
#include <string.h>
#include "clip.h"
#include "vertex.h"
static float coordinate(const grx_vertex *v,unsigned plane){return plane<2?v->x:v->y;}
static int inside(const grx_vertex *v,unsigned plane,float edge){return plane&1?coordinate(v,plane)<=edge:coordinate(v,plane)>=edge;}
static grx_vertex intersection(const grx_vertex *a,const grx_vertex *b,unsigned plane,float edge){
 double t=((double)edge-coordinate(a,plane))/((double)coordinate(b,plane)-coordinate(a,plane));
 grx_vertex v;
#define MIX(field) v.field=(float)((1-t)*(double)a->field+t*(double)b->field)
 MIX(x);MIX(y);MIX(r);MIX(g);MIX(b);MIX(a);MIX(sow);MIX(tow);MIX(oow);
#undef MIX
 if(plane<2)v.x=edge;else v.y=edge;
 return v;
}
grx_result grx_clip_triangle(const grx_vertex tri[3],unsigned width,unsigned height,grx_vertex out[GRX_CLIP_VERTICES],unsigned *count){
 *count=0;
 if(!width||!height||width>1024||height>768)return GRX_BAD_ARGUMENT;
 for(unsigned i=0;i<3;i++)if(!grx_vertex_valid(tri+i))return GRX_BAD_ARGUMENT;
 const float edge[4]={0,(float)(width-1),0,(float)(height-1)};
 unsigned any=0,all=15;
 for(unsigned i=0;i<3;i++){
  unsigned code=0;for(unsigned p=0;p<4;p++)if(!inside(tri+i,p,edge[p]))code|=1u<<p;
  any|=code;all&=code;
 }
 if(all)return GRX_EMPTY;
 if(!any){memcpy(out,tri,3*sizeof(*out));*count=3;return GRX_OK;}
 grx_vertex storage[2][GRX_CLIP_VERTICES];unsigned n=3,current=0;
 memcpy(storage[0],tri,3*sizeof(*tri));
 for(unsigned p=0;p<4&&n;p++){
  grx_vertex *input=storage[current],*output=storage[current^1];unsigned m=0;
  const grx_vertex *a=input+n-1;int was=inside(a,p,edge[p]);
  for(unsigned i=0;i<n;i++){
   const grx_vertex *b=input+i;int is=inside(b,p,edge[p]);
   if(is!=was){if(m==GRX_CLIP_VERTICES)return GRX_BAD_ARGUMENT;output[m++]=intersection(a,b,p,edge[p]);}
   if(is){if(m==GRX_CLIP_VERTICES)return GRX_BAD_ARGUMENT;output[m++]=*b;}
   a=b;was=is;
  }
  n=m;current^=1;
 }
 if(n<3)return GRX_EMPTY;
 memcpy(out,storage[current],n*sizeof(*out));*count=n;return GRX_OK;
}
