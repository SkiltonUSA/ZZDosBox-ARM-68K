"""Stage the optional ARMv7 JIT; the pinned upstream checkout stays untouched."""
import difflib

def prepare(out):
    patches=[]
    def change(name, fn):
        p=out/name;old=p.read_text();new=fn(old);assert new!=old,name
        p.write_text(new)
        patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+name,tofile='b/'+name))
    change('include/config.h',lambda s:s+'\n#define C_DYNREC 1\n#define C_TARGETCPU ARMV7LE\n')
    def core(s):
        s='#include "dynrec_plan.h"\n'+s
        s=s.replace('(1024*1024*8)','DB_JIT_CACHE').replace('(128*1024)','(8*1024*DB_JIT_MIB)').replace('#define CACHE_PAGES\t\t(512)','#define CACHE_PAGES\t\t(128)')
        s=s.replace('Bits CPU_Core_Dynrec_Run(void)', 'unsigned db_dynrec_dispatches;\nBits CPU_Core_Dynrec_Run(void)')
        s=s.replace('BlockReturn ret=core_dynrec.runcode(block->cache.start);','db_dynrec_dispatches++;\n\t\tBlockReturn ret=core_dynrec.runcode(block->cache.start);')
        return s
    change('src/cpu/core_dynrec.cpp',core)
    def arm(s):
        a=s.index('static void cache_block_closing(')
        return s[:a]+'''extern "C" void db_dynrec_commit(const void *,unsigned);
static void cache_block_closing(const Bit8u *start,Bitu size) {
 db_dynrec_commit(start,size);
}
'''
    change('src/cpu/core_dynrec/risc_armv4le-common.h',arm)
    def cache(s):
        s='extern "C" void *db_dynrec_alloc(unsigned);\nextern "C" void db_dynrec_commit(const void *,unsigned);\n'+s
        a=s.index('#if defined (WIN32)',s.index('static void cache_init'))
        b=s.index('\n\t\t\tif(!cache_code_start_ptr)',a)
        s=s[:a]+'cache_code_start_ptr=(Bit8u*)db_dynrec_alloc(CACHE_TOTAL+CACHE_MAXSIZE+PAGESIZE_TEMP*2-1);'+s[b:]
        # The entry trampoline also needs publication before its first call.
        s=s.replace('\t\tdyn_run_code();','\t\tdyn_run_code();\n\t\tdb_dynrec_commit(cache_code_link_blocks,PAGESIZE_TEMP);')
        # Restore wrapped guest page handlers before devices disappear.
        a=s.index('static void cache_close(void) {')
        s=s[:a]+'''static void cache_close(void) {
 while(cache.used_pages)cache.used_pages->ClearRelease();
 while(cache.free_pages){
  CodePageHandlerDynRec *p=cache.free_pages;cache.free_pages=p->next;
  free(p->invalidation_map);delete p;
 }
 free(cache_blocks);cache_blocks=NULL;
 cache_code_start_ptr=cache_code=cache_code_link_blocks=NULL;
 memset(&cache,0,sizeof(cache));memset(link_blocks,0,sizeof(link_blocks));
 cache_initialized=false;
}
'''
        return s
    change('src/cpu/dyn_cache.h',cache)
    (out/'dynrec-integration.patch').write_text(''.join(patches))
    return [out/'src/cpu/core_dynrec.cpp']
