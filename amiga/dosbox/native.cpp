#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "adapter.h"
#include "fixture.h"
static unsigned stop_count;
static int stop(){return ++stop_count>=4;}
int main(){
 DosboxResult r{};uint32_t hash=2166136261u;
 for(unsigned i=0;i<256;i++)hash=(hash^((i*13+7)&255))*16777619u;
 for(unsigned i=0;i<3;i++){
  int status=db_run(db_fixture,sizeof(db_fixture),100,&r,nullptr);
  printf("DOSBOX interpreter: rc=%d slices=%u exit=%u hash=%08x fault=%u text=%s\n",status,r.slices,r.exit_code,r.hash,r.fault,r.text);
  assert(!status&&!r.fault&&r.hash==hash&&!strcmp(r.text,"ZZDOS x86 arithmetic/memory PASS\r\n"));
 }
 const uint8_t loop[]={0xeb,0xfe};
 assert(db_run(loop,sizeof(loop),100,&r,stop)==2&&r.fault==3&&r.slices==3);
 assert(db_run(loop,sizeof(loop),2,&r,nullptr)==1&&r.fault==1);
 const uint8_t invalid[]={0xb4,0xff,0xcd,0x21};
 assert(db_run(invalid,sizeof(invalid),10,&r,nullptr)==1&&r.fault==1);
 assert(db_run(nullptr,1,1,&r,nullptr)==1&&r.fault==2);
 puts("PASS: guest arithmetic/flags/stack/32-bit registers, RAM, DOS text/exit, repeatability, bounded execution, cancellation and unsupported-service rejection");
}
