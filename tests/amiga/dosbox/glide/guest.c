#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "guest.h"
#include "guest_calls.h"
static unsigned char memory[512];
static unsigned reads,writes;
static int read_bytes(void *p,uint32_t a,void *b,unsigned n){(void)p;reads++;if(a>sizeof(memory)||n>sizeof(memory)-a)return 0;memcpy(b,memory+a,n);return 1;}
static int write_bytes(void *p,uint32_t a,const void *b,unsigned n){(void)p;writes++;if(a>sizeof(memory)||n>sizeof(memory)-a)return 0;memcpy(memory+a,b,n);return 1;}
int main(void){
 grx_guest_io io={read_bytes,write_bytes,0,16};grx_guest_call c;
 memset(memory,0xa5,sizeof(memory));
 assert(grx_guest_decode(&c,GRX_CALL_grDrawTriangle,100,&io));
 assert(c.count==3&&c.args[0]==0xa5a5a5a5&&reads==1);
 unsigned prev=reads;
 assert(!grx_guest_decode(&c,GRX_CALL_COUNT,100,&io)&&reads==prev);
 assert(!grx_guest_decode(&c,0,100,&io)&&reads==prev);
 assert(!grx_guest_decode(&c,GRX_CALL_grDrawTriangle,0xfffffffcu,&io)&&reads==prev);
 assert(!grx_guest_decode(&c,GRX_CALL_grDrawTriangle,508,&io));
 assert(grx_guest_decode(&c,GRX_CALL_grGlideInit,0xffffffffu,&io));
 c.id=GRX_CALL_grGlideGetVersion;c.count=1;c.args[0]=100;
 assert(grx_guest_diagnostic(&c,&io)==0);
 assert(!strcmp((char *)memory+116,"ZZDosBox Glide diagnostic"));
 assert(memory[115]==0xa5&&memory[180]==0xa5);
 c.args[0]=0xfffffff0u;prev=writes;grx_guest_diagnostic(&c,&io);assert(writes==prev);
 c.id=GRX_CALL_grSstQueryHardware;c.args[0]=0xfffffffcu;prev=writes;
 assert(grx_guest_diagnostic(&c,&io)==0&&writes==prev);
 c.id=GRX_CALL_grSstWinOpen;
 assert(grx_guest_diagnostic(&c,&io)==0);
 puts("PASS bounded guest argument decode, version write and no-board diagnostic");
}
