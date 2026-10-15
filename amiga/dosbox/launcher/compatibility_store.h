/* GPL-3.0-or-later. Bounded, endian-independent on-demand detail storage.
 * One selected record is resident. Index and record CRCs bind it to this build.
 * Returned strings remain valid until the next selection or close. */
#ifndef ZZ_COMPAT_STORE_H
#define ZZ_COMPAT_STORE_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../compatibility/details_meta.h"
struct compat_game_details {unsigned id,comments;const char *publisher,*tester,*history,*date,*author,*preview,*discussion;};
struct compat_store {
 FILE *file;unsigned char *index;char record[COMPAT_STORE_RECORD_MAX];
 struct compat_game_details detail;const char *error;unsigned selected;
};
static uint32_t compat_be32(const unsigned char *p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static uint32_t compat_crc(const void *data,size_t n){
 const unsigned char *p=data;uint32_t crc=0xffffffffu;unsigned bit;
 while(n--){crc^=*p++;for(bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1)));}
 return ~crc;
}
static void compat_store_close(struct compat_store *s){
 if(s->file)fclose(s->file);free(s->index);memset(s,0,sizeof(*s));
}
static int compat_store_open(struct compat_store *s,const char *path){
 unsigned char header[20];unsigned i;uint32_t previous=0,end=20+COMPAT_STORE_COUNT*16u;
 compat_store_close(s);s->file=fopen(path,"rb");
 if(!s->file){s->error="Compatibility.dat is missing or unreadable.";return 0;}
 if(fread(header,1,sizeof(header),s->file)!=sizeof(header)||memcmp(header,"ZZCD0001",8)||
    compat_be32(header+8)!=COMPAT_STORE_COUNT||compat_be32(header+12)!=COMPAT_STORE_INDEX_CRC||
    compat_be32(header+16)!=COMPAT_STORE_BYTES)goto bad;
 if(fseek(s->file,0,SEEK_END)||ftell(s->file)!=(long)COMPAT_STORE_BYTES||fseek(s->file,20,SEEK_SET))goto bad;
 s->index=malloc(COMPAT_STORE_COUNT*16u);if(!s->index)goto bad;
 if(fread(s->index,16,COMPAT_STORE_COUNT,s->file)!=COMPAT_STORE_COUNT||
    compat_crc(s->index,COMPAT_STORE_COUNT*16u)!=COMPAT_STORE_INDEX_CRC)goto bad;
 for(i=0;i<COMPAT_STORE_COUNT;i++){
  const unsigned char *p=s->index+i*16;uint32_t id=compat_be32(p),off=compat_be32(p+4),len=compat_be32(p+8);
  if(id<=previous||off!=end||len<11||len>sizeof(s->record)||off>COMPAT_STORE_BYTES||len>COMPAT_STORE_BYTES-off)goto bad;
  previous=id;end=off+len;
 }
 if(end!=COMPAT_STORE_BYTES)goto bad;
 return 1;
bad:compat_store_close(s);s->error="Compatibility.dat is damaged or from another build.";return 0;
}
static const struct compat_game_details *compat_store_find(struct compat_store *s,unsigned id){
 unsigned lo=0,hi=COMPAT_STORE_COUNT,i;const unsigned char *index;char *p,*end;const char *fields[7];uint32_t n;
 if(!s->file||!s->index)return NULL;
 if(s->selected==id&&id)return &s->detail;
 s->selected=0;s->error=NULL;
 while(lo<hi){unsigned mid=lo+(hi-lo)/2;if(compat_be32(s->index+mid*16)<id)lo=mid+1;else hi=mid;}
 if(lo==COMPAT_STORE_COUNT||compat_be32(s->index+lo*16)!=id){s->error="No offline details for this game.";return NULL;}
 index=s->index+lo*16;n=compat_be32(index+8);
 if(fseek(s->file,(long)compat_be32(index+4),SEEK_SET)||fread(s->record,1,n,s->file)!=n||compat_crc(s->record,n)!=compat_be32(index+12))goto bad;
 p=s->record+4;end=s->record+n;
 for(i=0;i<7;i++){
  char *z;if(p>=end||(z=memchr(p,0,(size_t)(end-p)))==NULL)goto bad;
  fields[i]=p;while(p<z){unsigned char c=(unsigned char)*p++;if((c<32&&c!='\n')||c==127)goto bad;}p++;
 }
 if(p!=end)goto bad;
 s->detail.id=id;s->detail.comments=compat_be32((unsigned char*)s->record);
 s->detail.publisher=fields[0];s->detail.tester=fields[1];s->detail.history=fields[2];s->detail.date=fields[3];
 s->detail.author=fields[4];s->detail.preview=fields[5];s->detail.discussion=fields[6];
 s->selected=id;return &s->detail;
bad:s->record[0]=0;s->error="Could not read this report: Compatibility.dat is damaged.";return NULL;
}
#endif
