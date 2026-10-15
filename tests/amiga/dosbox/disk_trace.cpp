/* Replay only sector numbers/operations, using the real bounded cache. */
#include "disk_cache.h"
#include <cstdio>
#include <cassert>
static DbDiskCache cache;
int main(int argc,char **argv){
 assert(argc==2);FILE *f=fopen(argv[1],"r");assert(f);
 unsigned sector,writing,requests=0,reads=0,writes=0;uint8_t bytes[512]={0};
 auto transfer=[&](unsigned s,unsigned n,void *data,bool w){
  assert(s<524288&&n<=8);requests++;if(w)writes++;else{reads+=n;memset(data,0,n*512);}return true;
 };
 while(fscanf(f,"%u %u",&sector,&writing)==2)assert(cache.access(sector,bytes,writing,524288,transfer));
 assert(feof(f));fclose(f);
 printf("{\"kib\":%u,\"hits\":%u,\"misses\":%u,\"requests\":%u,\"sectors_read\":%u,\"writes\":%u}\n",cache.sets*cache.ways*4,cache.hits,cache.misses,requests,reads,writes);
}
