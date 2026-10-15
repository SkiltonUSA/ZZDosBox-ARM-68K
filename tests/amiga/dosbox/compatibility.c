#include <assert.h>
#include <stdio.h>
#include "compatibility.h"
#include "compatibility_store.h"
static struct compat_store database;
#define compat_find_details(id) compat_store_find(&database,id)
int main(int argc,char **argv){
 assert(argc==2);assert(compat_store_open(&database,argv[1]));
 unsigned i,bucket,matched,total=0,broken=0,doom=0;
 assert(COMPAT_COUNT>3000);
 assert(!compat_find_details(0)&&!compat_find_details(0xffffffffu));
 for(i=0;i<COMPAT_STORE_COUNT;i++){
  unsigned id=compat_be32(database.index+i*16);
  const struct compat_game_details *d=compat_find_details(id);assert(d&&d->id==id);
  assert(compat_find_details(id)==d);
  assert(*d->history);
  assert((d->comments!=0)==(*d->discussion!=0));
  {const char *cursor=d->history;struct compat_version_row row;unsigned rows=0;int result;
   while((result=compat_next_version(&cursor,&row))>0){assert(*row.version&&*row.status&&*row.rating);rows++;}
   assert(result==0&&rows>0&&rows<=COMPAT_MAX_VERSIONS);
  }
 }
 for(bucket=1;bucket<=27;bucket++){
  matched=0;
  for(i=0;i<COMPAT_COUNT;i++)if(compat_matches(&compat_entries[i],bucket,""))matched++;
  assert(matched);total+=matched;
 }
 assert(total==COMPAT_COUNT);
 for(i=0;i<COMPAT_COUNT;i++){
  const struct compat_entry *e=&compat_entries[i];
  assert(e->id&&e->bucket>=1&&e->bucket<=27);
  assert(compat_find_details(e->id));
  assert(compat_matches(e,0,e->title));
  assert(!compat_matches(e,28,""));
  assert(!compat_matches(e,0,"zzzzzz-no-such-title-zzzzzz"));
  if(e->percent==255){assert(!strcmp(e->status,"broken"));broken++;}
  else assert(e->percent<=100);
  if(!strcmp(e->title,"Doom")){
   assert(compat_matches(e,4,"dOoM"));assert(!compat_matches(e,3,"doom"));doom++;
  }
 }
 {struct compat_version_row row;const char *p="0.74 broken|0.73 supported 100%";
  assert(compat_next_version(&p,&row)==1&&!strcmp(row.rating,"-"));
  assert(compat_next_version(&p,&row)==1&&!strcmp(row.rating,"100%"));assert(compat_next_version(&p,&row)==0);
  p="bad";assert(compat_next_version(&p,&row)==-1&&!strcmp(p,"bad"));
  p="0.74 supported 100% unexpected";assert(compat_next_version(&p,&row)==-1);
 }
 compat_store_close(&database);assert(!database.file&&!database.index);
 assert(broken&&doom);puts("Compatibility dataset, letter partition, title search and missing-rating checks passed.");return 0;
}
