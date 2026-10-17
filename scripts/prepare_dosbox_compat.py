#!/usr/bin/env python3
"""Adapt pinned DOSBox EMS/XMS services to the owned guest RAM allocation."""
import difflib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
UP=ROOT/'.context/research/dosbox-libretro'
def prepare(out):
 patches=[];sources=[]
 p=out/'shell_messages.cpp';p.write_text(p.read_text().replace('0.11.0-dev','0.20.1-dev'))
 # Add the environment only in the compatibility runtime, after shell creation.
 p=out/'frontend.cpp';text=p.read_text();text=text.replace('first_shell=new DOS_Shell();first_shell->Run();','first_shell=new DOS_Shell();if(db_sound_card)first_shell->SetEnv("BLASTER",db_sound_card==2?"A220 I7 D1 H5 T6":"A220 I7 D1 T4");first_shell->Run();');text=text.replace('first_shell->Run();','if(db_extra_sound==1){first_shell->SetEnv("ULTRASND","240,3,3,5,5");first_shell->SetEnv("ULTRADIR","C:\\\\ULTRASND");}first_shell->Run();');p.write_text('#include "audio.h"\n'+text)
 def unit(rel,edit):
  old=(UP/rel).read_text();new=edit(old);p=out/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(new);sources.append(p)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 def ems(s):
  a=s.index('Bitu GetEMSType(');b=s.index('class EMS:',a)
  s=s[:a]+'extern void CALLBACK_DeAllocate(Bitu);\nBitu GetEMSType(Section_prop*) { return db_ems_enabled ? (db_vcpi_enabled ? 1 : 2) : 0; }\n\n'+s[b:]
  s=s.replace('#include "dma.h"','#include "dma.h"\n#include "compat.h"')
  s=s.replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(')
  s=s.replace('delete test;', 'delete test;test=nullptr;')
  s=s.replace('/* Release memory allocated to system handle */','CALLBACK_DeAllocate(call_int67);\n\t\tems_baseseg=0;\n\t\t/* Release all surviving application handles as well as the system handle. */')
  a=s.index('\t\tif (emm_handles[EMM_SYSTEM_HANDLE].pages != NULL_HANDLE) {',s.index('~EMS()'))
  b=s.index('\n\t\t/* Clear handle',a)
  s=s[:a]+'''        for(unsigned i=0;i<EMM_MAX_HANDLES;i++)
            if(emm_handles[i].pages!=NULL_HANDLE) MEM_ReleasePages(emm_handles[i].mem);
'''+s[b:]
  return s
 def xms(s):
  return s.replace('#include "xms.h"','#include "xms.h"\n#include "compat.h"').replace('section->Get_bool("xms")','db_xms_enabled').replace('section->Get_bool("umb")','db_umb_enabled').replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(').replace('delete test;', 'delete test;test=nullptr;')
 unit('src/hardware/dma.cpp',lambda s:s.replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(').replace('delete test;', 'delete test;test=nullptr;'));unit('src/ints/ems.cpp',ems);unit('src/ints/xms.cpp',xms)
 old=(UP/'src/hardware/memory.cpp').read_text()
 code=old[old.index('Bitu MEM_FreeLargest('):old.index('/* \n\tA20 line handling')]
 code=code.replace('while (pages && (!memory.mhandles[index]))','while (pages && index<memory.pages && (!memory.mhandles[index]))')
 prefix=old[:old.index('#include')]+'''#include "mem.h"
#include "compat.h"
#include <string.h>
#include "capabilities.h"
#define XMS_START 0x110u
static struct { Bitu pages; MemHandle mhandles[DB_GUEST_PAGES]; } memory;
void db_memory_pool_reset() {
 memory.pages=MEM_TotalPages();
 if(memory.pages>DB_GUEST_PAGES)E_Exit("Extended memory pool bound");
 memset(memory.mhandles,0,sizeof(memory.mhandles));
}
'''
 p=out/'extended_allocator.cpp';p.write_text(prefix+code);sources.append(p)
 (out/'compat-integration.patch').write_text(''.join(patches))
 return sources
