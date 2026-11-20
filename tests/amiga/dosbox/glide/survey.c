/* Research-only virtual board replies for discovering guest API requirements.
 * Never linked into the ARM player. All draw/state calls are observed, not
 * executed. LFB writes use RAM explicitly reserved by the native harness;
 * pixel-pipeline effects are not applied. No hardware pointer is fabricated. */
#include <string.h>
#include "survey.h"
#include "guest_calls.h"
uint32_t grx_survey_call(grx_survey *s,const grx_guest_call *c,const grx_guest_io *io)
{
 switch(c->id){
 case GRX_CALL_grSstQueryHardware:case GRX_CALL_grSstQueryBoards:{
  unsigned char hw[148]={0};hw[0]=1;hw[8]=4;hw[12]=1;hw[16]=1;hw[24]=1;hw[28]=2;
  uint64_t a=(uint64_t)io->data_base+c->args[0];
  return a<=UINT32_MAX-sizeof(hw)+1&&io->write(io->opaque,(uint32_t)a,hw,sizeof(hw));
 }
 case GRX_CALL_grGlideGetVersion:return grx_guest_diagnostic(c,io);
 case GRX_CALL_grGlideShutdown:case GRX_CALL_grGlideInit:{
  uint32_t lfb=s->lfb_address;memset(s,0,sizeof(*s));s->lfb_address=lfb;return 0;
 }
 case GRX_CALL_grSstWinOpen:
  if(c->args[1]!=7)return 0;
  s->width=640;s->height=480;s->open=1;return 1;
 case GRX_CALL_grSstWinClose:s->open=0;return 0;
 case GRX_CALL_grSstScreenWidth:return s->width;
 case GRX_CALL_grSstScreenHeight:return s->height;
 case GRX_CALL_guTexMemReset:s->next_texture=0;return 0;
 case GRX_CALL_guTexAllocateMemory:return s->next_texture<1024?++s->next_texture:UINT32_MAX;
 case GRX_CALL_guTexMemQueryAvail:return 2*1024*1024;
 case GRX_CALL_grTexMinAddress:return 0;
 case GRX_CALL_grTexMaxAddress:return 2*1024*1024-1;
 case GRX_CALL_grLfbLock:{
  if(!s->open||!s->lfb_address||s->lfb_address<io->data_base||c->args[2]>2)return 0;
  uint32_t info[5]={20,s->lfb_address-io->data_base,s->width*2,c->args[2],c->args[3]};
  unsigned char bytes[20];for(unsigned i=0;i<5;i++)for(unsigned b=0;b<4;b++)bytes[i*4+b]=info[i]>>(8*b);
  uint64_t a=(uint64_t)io->data_base+c->args[5];
  return a<=UINT32_MAX-19&&io->write(io->opaque,(uint32_t)a,bytes,20);
 }
 case GRX_CALL_grLfbUnlock:return s->open;
 case GRX_CALL_grSstVRetraceOn:return ++s->retrace&1;
 case GRX_CALL_grSstStatus:return 0x0ffff03fu|((++s->retrace&1)<<6);
 default:return 0;
 }
}
