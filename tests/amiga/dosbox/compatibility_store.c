#include "compatibility_store.h"
#include <assert.h>
static struct compat_store store;
int main(int argc,char **argv){
 assert(argc==3);
 if(!strcmp(argv[2],"bad-index")){assert(!compat_store_open(&store,argv[1]));assert(store.error&&!store.file&&!store.index);}
 else {
  assert(compat_store_open(&store,argv[1]));unsigned id=compat_be32(store.index);
  if(!strcmp(argv[2],"bad-record")){assert(!compat_store_find(&store,id));assert(store.error&&!store.selected);}
  else {assert(compat_store_find(&store,id));assert(!compat_store_find(&store,0xffffffff));assert(compat_store_find(&store,id));}
 }
 compat_store_close(&store);compat_store_close(&store);return 0;
}
