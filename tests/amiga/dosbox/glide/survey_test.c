#include <assert.h>
#include <string.h>
#include "survey.h"
#include "guest_calls.h"
static unsigned char memory[1024];
static int write_guest(void *p,uint32_t a,const void *data,unsigned n)
{(void)p;if(a>sizeof(memory)||n>sizeof(memory)-a)return 0;memcpy(memory+a,data,n);return 1;}
int main(void){
 grx_survey s={0};grx_guest_io io={0,write_guest,0,100};
 grx_guest_call c={GRX_CALL_grSstQueryHardware,1,{20}};
 memset(memory,0xaa,sizeof(memory));assert(grx_survey_call(&s,&c,&io)==1);
 assert(memory[119]==0xaa&&memory[120]==1&&memory[124]==0&&memory[128]==4);
 assert(memory[136]==1&&memory[148]==2&&memory[267]==0&&memory[268]==0xaa);
 io.data_base=0xfffffff0;assert(!grx_survey_call(&s,&c,&io));io.data_base=100;
 c=(grx_guest_call){GRX_CALL_grSstWinOpen,7,{0,7,0,0,0,2,1}};
 assert(grx_survey_call(&s,&c,&io)==1&&s.width==640&&s.height==480);
 c=(grx_guest_call){GRX_CALL_grLfbLock,6,{0}};assert(!grx_survey_call(&s,&c,&io));
 s.lfb_address=0x200000;c=(grx_guest_call){GRX_CALL_grLfbLock,6,{1,1,1,0,1,300}};
 assert(grx_survey_call(&s,&c,&io));
 assert(memory[400]==20&&memory[404]==0x9c&&memory[405]==0xff&&memory[406]==0x1f);
 assert(memory[408]==0&&memory[409]==5&&memory[412]==1);
 c=(grx_guest_call){GRX_CALL_guTexAllocateMemory,15,{0}};
 assert(grx_survey_call(&s,&c,&io)==1&&grx_survey_call(&s,&c,&io)==2);
 c=(grx_guest_call){GRX_CALL_grGlideShutdown,0,{0}};
 grx_survey_call(&s,&c,&io);assert(!s.open&&!s.next_texture);
 return 0;
}
