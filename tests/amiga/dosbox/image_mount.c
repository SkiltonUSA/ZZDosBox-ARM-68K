#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "image_mount.h"
static void le(unsigned char*p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i));}
int main(void){unsigned char m[512]={0};m[510]=0x55;m[511]=0xaa;m[450]=6;le(m+454,32);
 for(uint32_t size=33554432;size<=268435456;size*=2){le(m+458,size/512-32);assert(di_raw_fat_image(m,size));assert(!di_raw_fat_image(m,size-512));assert(!di_raw_fat_image(m,size+512));le(m+458,size/512);assert(!di_raw_fat_image(m,size));}
 le(m+458,65504);le(m+454,0xfffffff0u);assert(!di_raw_fat_image(m,33554432));le(m+454,32);m[511]=0;assert(!di_raw_fat_image(m,33554432));m[511]=0xaa;m[450]=0x76;assert(!di_raw_fat_image(m,33554432));
 puts("Raw image bounds and format checks passed");return 0;}
