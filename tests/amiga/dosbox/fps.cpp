#include <assert.h>
#include <stdio.h>
#include <vector>
#include "fps.h"
int main(){
 pc_fps f{};pc_fps_update(&f,100,10,1000);pc_fps_update(&f,599,40,1000);assert(f.tenths==0);
 pc_fps_update(&f,600,40,1000);assert(f.tenths==600);
 pc_fps_update(&f,1600,65,1000);assert(f.tenths==250);
 f={100,0xfffffff0u,0,true};pc_fps_update(&f,1100,44,1000);assert(f.tenths==600);
 for(unsigned width: {320u,504u,640u,960u})for(unsigned format: {3u,4u}){
  std::vector<uint32_t> pixels(width*480+2,0x12345678);pc_fps_draw((uint8_t*)&pixels[1],width*4,format,599,width);
  assert(pixels[0]==0x12345678&&pixels.back()==0x12345678);
  unsigned white=0;
  for(unsigned y=0;y<480;y++)for(unsigned x=0;x<width;x++){
   unsigned v=pixels[1+y*width+x];
   if(y>=4&&y<22&&x>=width-116&&x<width-4){assert(v==0||v==(format==3?0x00ffffffu:0xffffff00u));white+=v!=0;}
   else assert(v==0x12345678);
  }
  assert(white>50);
 }
 puts("PASS real-time FPS interval, frame wrap, formats and overlay bounds");
}
