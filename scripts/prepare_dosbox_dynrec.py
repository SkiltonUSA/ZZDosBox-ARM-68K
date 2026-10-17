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
        # DOSBox-X also falls back for 386 paging: recursive guest page faults
        # are unsafe in dynrec. Check every dispatcher iteration, including
        # immediately after a block that enabled paging (not just on entry).
        s=s.replace('unsigned db_dynrec_dispatches;', 'unsigned db_dynrec_dispatches,db_dynrec_paged_fallbacks;')
        old='Bits CPU_Core_Dynrec_Run(void) {\n\tfor (;;) {'
        assert old in s
        s=s.replace(old,old+'''
#ifdef DB_EXPANDED
        if(db_last_paging!=paging.enabled){
            while(cache.used_pages)cache.used_pages->ClearRelease();
            db_last_paging=paging.enabled;
        }
        if(paging.enabled && !db_paged_jit){db_dynrec_paged_fallbacks++;return CPU_Core_Normal_Run();}
        if(paging.enabled && !db_paged_register_instruction()){
            // Let the paging-safe interpreter consume a short burst. Returning
            // after every memory/branch instruction floods the frontend with
            // dispatches. Preserve the scheduler's allocated cycle total and
            // never extend past an already scheduled event boundary.
            db_dynrec_paged_fallbacks++;
            Bits quantum=CPU_Cycles>32?32:CPU_Cycles;
            CPU_CycleLeft+=CPU_Cycles-quantum;CPU_Cycles=quantum;
            return CPU_Core_Normal_Run();
        }
#else
        if(paging.enabled){db_dynrec_paged_fallbacks++;return CPU_Core_Normal_Run();}
#endif
''')
        helper='''
#ifdef DB_EXPANDED
#include "paged_plan.h"
extern unsigned db_paged_jit;
static bool db_last_paging;
static HostPt db_paged_ram_page(Bitu page){
 if(page>=MEM_TotalPages())return nullptr;
 PageHandler*h=MEM_GetPageHandler(page);
 if(!(h->flags&PFLAG_READABLE)||(h->flags&PFLAG_NOCODE))return nullptr;
 return h->GetHostReadPt(page);
}
static bool db_paged_register_instruction(){
 PhysPt address=SegPhys(cs)+reg_eip;
 // No page-boundary, segment-wrap or device-memory speculative fetches.
 if((address&4095)>4080||(!cpu.code.big&&(reg_eip&65535)>65520))return false;
 HostPt directory=db_paged_ram_page(paging.base.page);if(!directory)return false;
 Bit32u pde=host_readd(directory+(address>>22)*4);if(!(pde&1))return false;
 HostPt table=db_paged_ram_page(pde>>12);if(!table)return false;
 Bit32u pte=host_readd(table+((address>>12)&1023)*4);if(!(pte&1))return false;
 // Match the pinned interpreter's 386 versus 486/Pentium U/S rules.
 bool user_allowed=CPU_ArchitectureType==CPU_ARCHTYPE_386SLOW?((pde|pte)&4)!=0:((pde&pte)&4)!=0;
 if((cpu.cpl&cpu.mpl)==3&&!user_allowed)return false;
 HostPt code=db_paged_ram_page(pte>>12);if(!code)return false;
 // Read directly from validated physical RAM: supervisor-only pages normally
 // leave the upstream TLB unlinked, and forcing that TLB would lose checks.
 code+=address&4095;
 return pc_paged_register_length(host_readb(code),host_readb(code+1),cpu.code.big)!=0;
}
#endif
'''
        s=s.replace('unsigned db_dynrec_dispatches,db_dynrec_paged_fallbacks;',helper+'unsigned db_dynrec_dispatches,db_dynrec_paged_fallbacks;')
        s=s.replace('CreateCacheBlock(chandler,ip_point,32)','CreateCacheBlock(chandler,ip_point,paging.enabled?1:32)')
        s=s.replace('block=LinkBlocks(ret);','if(paging.enabled)break;\n\t\t\tblock=LinkBlocks(ret);')
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
