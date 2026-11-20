#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "channel_plan.h"
int main(void){
 uint32_t l1[4096]={0},l2[256],save[256];
 memset(l2,0x5a,sizeof(l2));memcpy(save,l2,sizeof(l2));
 assert(!grx_channel_map(l1,l2,0x400000,0x100001,sizeof(grx_stream_channel)));
 assert(!memcmp(l2,save,sizeof(l2))&&!l1[1]);
 assert(!grx_channel_map(l1,l2,0x400001,0x100000,sizeof(grx_stream_channel)));
 assert(!grx_channel_map(l1,l2,0x400000,0x1ff000,sizeof(grx_stream_channel)));
 assert(!grx_channel_map(l1,l2,0x400000,0x100000,sizeof(grx_stream_channel)-1));
 l1[1]=0x123;assert(!grx_channel_map(l1,l2,0x400000,0x100000,sizeof(grx_stream_channel)));l1[1]=0;
 assert(grx_channel_map(l1,l2,0x400000,0x1d9000,sizeof(grx_stream_channel)));
 assert(l1[1]==0x400001);
 for(unsigned i=0;i<256;i++)assert(l2[i]==((i>=0xd9&&i<0xe2)?((0x100000+i*4096)|0x453):0));
 puts("PASS channel mapping: exact NC/XN pages, bounds, existing-map exclusion");
}
