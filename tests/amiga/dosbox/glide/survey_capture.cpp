/* Native-only API/data capture for reproducible renderer development.
 * Blob layout: 3 raw 60-byte vertices; one largest texture level; or a whole
 * 16-bit LFB at unlock. All pointers pass through checked guest reads. */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "dosbox.h"
#include "survey_capture.h"
#include "guest_calls.h"
#include "pic.h"
static FILE *trace;
static unsigned texture_bytes[1025],swaps;
static uint64_t total;
static void bytes(const void *p,size_t n){
 if(fwrite(p,1,n,trace)!=n)E_Exit("Glide trace write failed");
 total+=n;if(total>256u*1024*1024)E_Exit("Glide trace reached its 256 MiB capture bound");
}
static void word(uint32_t n){unsigned char b[4];for(unsigned i=0;i<4;i++)b[i]=n>>(8*i);bytes(b,4);}
static void read_guest(const grx_guest_io *io,uint32_t ptr,void *out,unsigned n){
 uint64_t a=(uint64_t)io->data_base+ptr;
 if(a>UINT32_MAX||n>UINT32_MAX-a+1||!io->read(io->opaque,(uint32_t)a,out,n))E_Exit("Glide trace invalid guest data");
}
void grx_survey_capture_open(){
 const char *path=getenv("ZZ_GLIDE_TRACE");trace=path&&*path?fopen(path,"wb"):nullptr;
 if(path&&*path&&!trace)E_Exit("Cannot create Glide trace");
 swaps=0;total=0;memset(texture_bytes,0,sizeof(texture_bytes));
 if(trace)bytes("GTA1",4);
}
void grx_survey_capture_call(const grx_guest_call *c,uint32_t result,const grx_guest_io *io,const grx_survey *s){
 if(!trace||swaps>=80)return;
 std::vector<unsigned char> blob;
 if(c->id==GRX_CALL_guTexMemReset)memset(texture_bytes,0,sizeof(texture_bytes));
 if(c->id==GRX_CALL_guTexAllocateMemory&&result<=1024){
  if(c->args[2]>256||c->args[3]>256||c->args[4]!=0xb)E_Exit("Glide trace texture format/size not covered");
  texture_bytes[result]=c->args[2]*c->args[3]*2;
 }
 if(c->id==GRX_CALL_grDrawTriangle||c->id==GRX_CALL_guDrawTriangleWithClip){
  blob.resize(180);for(unsigned i=0;i<3;i++)read_guest(io,c->args[i],blob.data()+60*i,60);
 }else if(c->id==GRX_CALL_guTexDownloadMipMap){
  unsigned id=c->args[0];if(id>1024||!texture_bytes[id])E_Exit("Glide trace unknown texture");
  blob.resize(texture_bytes[id]);read_guest(io,c->args[1],blob.data(),blob.size());
 }else if(c->id==GRX_CALL_grLfbUnlock){
  blob.resize(s->width*s->height*2);
  if(!io->read(io->opaque,s->lfb_address,blob.data(),blob.size()))E_Exit("Glide trace invalid LFB");
 }
 word((uint32_t)PIC_Ticks);word(c->id);word(c->count);word(blob.size());word(result);
 for(unsigned i=0;i<c->count;i++)word(c->args[i]);
 if(!blob.empty())bytes(blob.data(),blob.size());
 if(c->id==GRX_CALL_grBufferSwap)++swaps;
}
void grx_survey_capture_close(){if(trace){if(fclose(trace))E_Exit("Glide trace close failed");trace=nullptr;}}
