#include "amiga/dosbox/pc/row_cache.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main(){
 for(unsigned format: {3u,4u})for(unsigned n:{256u,320u,640u})
 for(unsigned width:{320u,640u,960u})for(unsigned source_height:{200u,240u,480u}){
  pc_row_cache cache={};std::vector<uint32_t> src(n*source_height);
  for(unsigned i=0;i<src.size();i++)src[i]=i*0x19273651u;
  std::vector<uint32_t> out(width+2,0xfeedface),reference(width);
  for(unsigned y=0;y<480;y++){
   unsigned sy=y*source_height/480,level=(y/4)%9;
   pc_scan_scale_row(reference.data(),src.data()+sy*n,n,format,width,level);
   pc_cached_row(&cache,out.data()+1,src.data()+sy*n,n,format,width,level,source_height<480);
   assert(out.front()==0xfeedface&&out.back()==0xfeedface);
   for(unsigned x=0;x<width;x++)assert(out[x+1]==reference[x]);
  }
  // Cache lifetime is one frame: changed source contents must be observed.
  cache.valid=false;src[0]^=0xffffffffu;
  pc_cached_row(&cache,out.data()+1,src.data(),n,format,width,8,true);
  pc_scan_scale_row(reference.data(),src.data(),n,format,width,8);
  for(unsigned x=0;x<width;x++)assert(out[x+1]==reference[x]);
 }
 pc_row_cache c={};uint32_t src[2]={1,2},out[4];
 pc_cached_row(&c,out,src,2,3,4,8,true);
 pc_cached_row(&c,out,src,2,3,4,8,true);assert(c.hits==1&&c.conversions==1);
 pc_cached_row(&c,out,src,2,4,4,8,true);assert(c.conversions==2);
 pc_cached_row(&c,out,src,2,4,4,4,true);assert(c.conversions==3);
 puts("PASS converted rows: formats, scaling, dimming, guards, frame invalidation and hits");
}
