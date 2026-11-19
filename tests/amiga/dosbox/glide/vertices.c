#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "guest_vertex.h"
#include "clip.h"
static unsigned char ram[512];
static int read_mem(void *p,uint32_t a,void *b,unsigned n){(void)p;if(a>sizeof(ram)||n>sizeof(ram)-a)return 0;memcpy(b,ram+a,n);return 1;}
static void f32(unsigned a,float f){uint32_t w;memcpy(&w,&f,4);for(unsigned i=0;i<4;i++)ram[a+i]=(unsigned char)(w>>(i*8));}
static int closef(float a,float b){return fabsf(a-b)<0.0001f;}
int main(void){
 grx_guest_io io={read_mem,0,0,32};grx_vertex v[3],out[GRX_CLIP_VERTICES];
 uint32_t addresses[]={0,60,120};
 for(unsigned j=0;j<3;j++)for(unsigned k=0;k<15;k++)f32(32+j*60+k*4,(float)(k+1));
 assert(grx_guest_vertices(v,addresses,0,&io));
 assert(v[0].x==1&&v[0].y==2&&v[0].r==4&&v[0].g==5&&v[0].b==6&&v[0].a==8);
 assert(v[0].sow==10&&v[0].tow==11&&v[0].oow==9);
 assert(grx_guest_vertices(v,addresses,1,&io)&&v[0].oow==12);
 grx_vertex saved[3];memcpy(saved,v,sizeof(v));addresses[2]=500;
 assert(!grx_guest_vertices(v,addresses,0,&io)&&!memcmp(saved,v,sizeof(v)));
 addresses[2]=UINT32_MAX-8;assert(!grx_guest_vertices(v,addresses,0,&io));
 unsigned n=99;grx_vertex tri[3]={{-10,10,0,0,0,255,0,0,1},{10,10,100,0,0,255,100,0,.5f},{10,30,100,0,0,255,100,100,.5f}};
 assert(grx_clip_triangle(tri,64,64,out,&n)==GRX_OK&&n==4);
 unsigned cuts=0;for(unsigned i=0;i<n;i++){assert(out[i].x>=0&&out[i].x<=63&&out[i].y>=0&&out[i].y<=63);if(out[i].x==0){cuts++;assert(closef(out[i].r,50)&&closef(out[i].sow,50)&&closef(out[i].oow,.75f));}}
 assert(cuts==2);
 for(unsigned i=0;i<3;i++)tri[i].x-=100;
 assert(grx_clip_triangle(tri,64,64,out,&n)==GRX_EMPTY&&n==0);
 tri[0].x=NAN;assert(grx_clip_triangle(tri,64,64,out,&n)==GRX_BAD_ARGUMENT&&n==0);
 tri[0]=(grx_vertex){-FLT_MAX,10,0,0,0,255,0,0,1};tri[1]=(grx_vertex){FLT_MAX,10,255,0,0,255,1,0,1};tri[2]=(grx_vertex){10,FLT_MAX,255,0,0,255,1,1,1};
 assert(grx_clip_triangle(tri,64,64,out,&n)==GRX_OK&&n>=3&&n<=7);
 for(unsigned i=0;i<n;i++)assert(isfinite(out[i].x)&&isfinite(out[i].y)&&out[i].x>=0&&out[i].x<=63&&out[i].y>=0&&out[i].y<=63);
 assert(grx_clip_triangle(tri,0,64,out,&n)==GRX_BAD_ARGUMENT);
 const float grid[]={-10,0,10,63,70};
 for(unsigned a=0;a<25;a++)for(unsigned b=0;b<25;b++)for(unsigned c=0;c<25;c++){
  unsigned indices[]={a,b,c};for(unsigned j=0;j<3;j++)tri[j]=(grx_vertex){grid[indices[j]%5],grid[indices[j]/5],100,100,100,255,0,0,1};
  grx_result rc=grx_clip_triangle(tri,64,64,out,&n);
  assert(rc==GRX_OK||rc==GRX_EMPTY);
  for(unsigned j=0;j<n;j++)assert(out[j].x>=0&&out[j].x<=63&&out[j].y>=0&&out[j].y<=63);
 }
 puts("PASS Glide 2 vertex layout, global/TMU W, transactional reads and perspective-preserving viewport clipping");
}
