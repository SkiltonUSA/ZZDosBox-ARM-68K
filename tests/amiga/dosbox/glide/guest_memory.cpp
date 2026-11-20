/* The direct TLB path must retain the byte path's page faults and handlers. */
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

using HostPt=uint8_t *;
static uint8_t ram[16*4096];
static unsigned mode[4],reads,writes,lookups;
static uint32_t fail_at;
static HostPt mapping(uint32_t a){return ram+(8+(3-(a>>12))-(a>>12))*4096;}
static HostPt get_tlb_read(uint32_t a){lookups++;return a<16384&&(mode[a>>12]==0||mode[a>>12]==4)?mapping(a):nullptr;}
static HostPt get_tlb_write(uint32_t a){lookups++;return a<16384&&mode[a>>12]==0?mapping(a):nullptr;}
static bool mem_readb_checked(uint32_t a,uint8_t *v){
 HostPt p=get_tlb_read(a);if(p){*v=p[a];return false;}
 reads++;if(a>=16384||a==fail_at||mode[a>>12]==3)return true;
 if(mode[a>>12]==1)mode[a>>12]=0;
 *v=mapping(a)[a];return false;
}
static bool mem_writeb_checked(uint32_t a,uint8_t v){
 HostPt p=get_tlb_write(a);if(p){p[a]=v;return false;}
 writes++;if(a>=16384||a==fail_at||mode[a>>12]==3)return true;
 if(mode[a>>12]==1)mode[a>>12]=0;
 mapping(a)[a]=v;return false;
}
#include "guest_memory.h"
static int reference(bool write,uint32_t a,uint8_t *p,unsigned n){
 if(n&&a>UINT32_MAX-(n-1))return 0;
 for(unsigned i=0;i<n;i++)if(write?mem_writeb_checked(a+i,p[i]):mem_readb_checked(a+i,p+i))return 0;
 return 1;
}
static void compare(bool write,uint32_t a,unsigned n,unsigned kind,uint32_t fault){
 static uint8_t initial[sizeof(ram)],expected[sizeof(ram)],input[18000],output[18000];
 assert(n+2<=sizeof(input));
 for(unsigned i=0;i<sizeof(ram);i++)initial[i]=(i*37+(i>>8))&255;
 for(unsigned i=0;i<sizeof(input);i++)input[i]=(i*13)^0x5a;
 memcpy(output,input,sizeof(input));memcpy(ram,initial,sizeof(ram));
 mode[0]=0;mode[1]=kind;mode[2]=0;mode[3]=4;reads=writes=lookups=0;fail_at=fault;
 int want=reference(write,a,input+1,n);unsigned nr=reads,nw=writes;
 memcpy(expected,ram,sizeof(ram));memcpy(ram,initial,sizeof(ram));
 mode[0]=0;mode[1]=kind;mode[2]=0;mode[3]=4;reads=writes=lookups=0;
 int got=write?grx_guest_write_checked(a,output+1,n):grx_guest_read_checked(a,output+1,n);
 assert(got==want);assert(!memcmp(ram,expected,sizeof(ram)));
 assert(!memcmp(input,output,sizeof(input)));assert(reads==nr&&writes==nw);
}
int main(){
 for(unsigned k=0;k<5;k++)for(unsigned o=0;o<16;o++)for(bool w:{false,true}){
  compare(w,4090+o,8200,k,UINT32_MAX);compare(w,4090+o,100,k,4105);
 }
 for(bool w:{false,true}){
  compare(w,UINT32_MAX-2,8,0,UINT32_MAX);compare(w,UINT32_MAX,0,0,UINT32_MAX);
  compare(w,16380,8,0,UINT32_MAX);
 }
 puts("PASS checked guest spans: remapped pages, lazy TLB, device/code handlers, faults, overflow and exact partial effects");
}
