/* Offline DOSBox directory lookup. No emulator settings are changed. */
#ifndef ZZ_COMPATIBILITY_H
#define ZZ_COMPATIBILITY_H
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../compatibility/data.h"
enum {COMPAT_MAX_VERSIONS=32};
struct compat_version_row {char version[24],status[24],rating[12];};
/* Consume one bounded history row. A malformed row leaves the cursor intact. */
static int compat_next_version(const char **cursor,struct compat_version_row *row){
 const char *p=*cursor;char line[96],extra;size_t n=0;int fields;
 if(!*p)return 0;
 while(p[n]&&p[n]!='|')n++;
 if(n>=sizeof(line))return -1;
 memcpy(line,p,n);line[n]=0;row->rating[0]=0;
 fields=sscanf(line,"%23s %23s %11s %c",row->version,row->status,row->rating,&extra);
 if(fields<2||fields>3)return -1;
 if(fields==2)strcpy(row->rating,"-");
 *cursor=p+n+(p[n]=='|');return 1;
}
static int compat_matches(const struct compat_entry *entry,unsigned bucket,const char *query){
 const unsigned char *start=(const unsigned char*)entry->title,*a,*b;
 if(bucket&&entry->bucket!=bucket)return 0;
 if(!*query)return 1;
 for(;*start;start++){
  a=start;b=(const unsigned char*)query;
  while(*a&&*b&&tolower(*a)==tolower(*b)){a++;b++;}
  if(!*b)return 1;
 }
 return 0;
}
#endif
