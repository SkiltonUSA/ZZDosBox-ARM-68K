/* GPL-2.0-or-later. Register-run bounds, not a speculative x86 decoder. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../../../amiga/dosbox/pc/paged_plan.h"
int main(void){
 const uint8_t run[]={0xb8,1,0,0,0,0x40,0x31,0xdb,0x83,0xc3,7,0x89,0x18};
 assert(pc_paged_register_count(run,sizeof run,1,32)==4);
 assert(pc_paged_register_count(run,sizeof run,1,2)==2);
 assert(pc_paged_register_count(run,4,1,32)==0);
 assert(pc_paged_register_count(run,5,1,32)==1);
 const uint8_t prefix[]={0x66,0xb8,1,0,0x90};
 assert(pc_paged_register_count(prefix,sizeof prefix,1,32)==0);
 const uint8_t small[]={0xb8,1,0,0x40,0xc3};
 assert(pc_paged_register_count(small,sizeof small,0,32)==2);
 const uint8_t boundary[]={0x90,0xb8,0};
 assert(pc_paged_register_count(boundary,sizeof boundary,1,32)==1);
 assert(pc_paged_register_count(boundary,0,1,32)==0);
 long page=sysconf(_SC_PAGESIZE);
 uint8_t *p=mmap(0,page*2,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
 assert(p!=MAP_FAILED);assert(mprotect(p+page,page,PROT_NONE)==0);
 memset(p,0x90,page);
 assert(pc_paged_register_count(p+page-1,1,1,32)==1);
 p[page-1]=0x31;assert(pc_paged_register_count(p+page-1,1,1,32)==0);
 p[page-1]=0xb8;assert(pc_paged_register_count(p+page-1,1,1,32)==0);
 assert(pc_paged_register_count(p,page,1,32)==32);
 assert(munmap(p,page*2)==0);
 puts("PASS paged register blocks: instruction limits, truncation, memory/prefix stop and inaccessible next page");
}
