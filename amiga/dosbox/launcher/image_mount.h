/* GPL-3.0-or-later. Raw PC images accepted by the Workbench image viewer. */
#ifndef ZZ_IMAGE_MOUNT_H
#define ZZ_IMAGE_MOUNT_H
#include <stdint.h>
static uint32_t di_le32(const unsigned char*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int di_raw_fat_image(const unsigned char*m,uint32_t bytes){
 unsigned i;uint32_t sectors=bytes/512;
 if(bytes<33554432u||bytes>268435456u||(bytes%262144u)||m[510]!=0x55||m[511]!=0xaa)return 0;
 for(i=0;i<4;i++){const unsigned char*p=m+446+i*16;uint32_t start=di_le32(p+8),count=di_le32(p+12);unsigned type=p[4];
  if(type==1||type==4||type==6||type==11||type==12||type==14){if(!start||!count||start>=sectors||count>sectors-start)return 0;return 1;}
 }
 return 0;
}
#endif
