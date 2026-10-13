#!/usr/bin/env python3
"""Stage pinned upstream PC devices and capture native DOSBox frames."""
import argparse,difflib,hashlib,shutil,subprocess
from pathlib import Path
import build_dosbox_shell as shell
ROOT=shell.ROOT;OUT=ROOT/'.context/amiga/dosbox-pc';SRC=ROOT/'amiga/dosbox/pc';UP=shell.UP
HW=['iohandler','pic','timer','keyboard','cmos','vga','vga_attr','vga_crtc','vga_dac','vga_draw','vga_gfx','vga_other','vga_memory','vga_misc','vga_seq','vga_xga']
INTS=['bios','bios_disk','bios_keyboard','int10','int10_char','int10_memory','int10_misc','int10_modes','int10_vesa','int10_pal','int10_put_pixel','int10_video_state','int10_vptable']
def prepare():
 shell.OUT=OUT;sources=shell.prepare();patch=[]
 planes=bytes(sum(((((x*8+bit)//80+4*(y//50))>>plane)&1)<<(7-bit) for bit in range(8)) for plane in range(4) for y in range(200) for x in range(40))
 (OUT/'ega-planes.bin').write_bytes(planes)
 subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/'egatest.S'),'-o','egatest.o'],cwd=OUT,check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary','egatest.o','-o','EGATEST.COM'],cwd=OUT,check=True)
 for name in ['keytest','cgatest','boottest','hddtest','badmem']:
  subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/(name+'.S')),'-o',name+'.o'],cwd=OUT,check=True)
  subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',name+'.o','-o',name.upper()+'.COM'],cwd=OUT,check=True)
 (OUT/'BOOTTEST.IMG').write_bytes((OUT/'BOOTTEST.COM').read_bytes()+bytes([0x39,0x5a])+bytes(1440*1024-514))
 for path in ['src/hardware','src/ints','src/gui']:shutil.copytree(UP/path,OUT/path,dirs_exist_ok=True)
 def change(rel,fn):
  p=OUT/rel;old=p.read_text();new=fn(old);p.write_text(new);patch.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 # Upstream banked software TLB: retain full guest address coverage without
 # a 20 MiB always-resident translation table on 32-bit ARM.
 change('include/paging.h',lambda s:s.replace('#define USE_FULL_TLB','/* USE_FULL_TLB disabled for the bounded XACP allocation. */'))
 def cold_tlb(s):
  a=s.index('static INLINE tlb_entry *get_tlb_entry(PhysPt address) {')
  b=s.index('static INLINE HostPt get_tlb_read(',a)
  return s[:a]+'''tlb_entry *PAGING_GetHighTLBEntry(PhysPt address);
static INLINE tlb_entry *get_tlb_entry(PhysPt address) {
 Bitu index=address>>12;
 if(index>=TLB_SIZE)return PAGING_GetHighTLBEntry(address);
 return &paging.tlbh[index];
}

'''+s[b:]
 change('include/paging.h',cold_tlb)
 change('src/cpu/paging.cpp',lambda s:s+'''
#if !defined(USE_FULL_TLB)
/* Keep rare high-bank allocation out of every interpreter memory access. */
__attribute__((noinline)) tlb_entry *PAGING_GetHighTLBEntry(PhysPt address) {
 Bitu bank=(address>>BANK_SHIFT)-1;
 if(bank>=TLB_BANKS)E_Exit("Invalid high TLB bank");
 if(!paging.tlbh_banks[bank])PAGING_InitTLBBank(&paging.tlbh_banks[bank]);
 return &paging.tlbh_banks[bank][(address>>12)&BANK_MASK];
}
#endif
''')
 for n in HW+INTS:
  rel=('src/hardware/' if n in HW else 'src/ints/')+n+'.cpp'
  change(rel,lambda s:s.replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(').replace('sect->AddDestroyFunction(', 'if(sect)sect->AddDestroyFunction('))
 change('src/hardware/iohandler.cpp',lambda s:s.replace('../src/cpu/lazyflags.h','../cpu/lazyflags.h'))
 # Memoize only the rounded float product, preserving upstream comparisons,
 # subtraction and integer conversion. No timer quantisation or event batching.
 def pic_cache(s):
  s=s.replace('struct PICEntry {\n\tfloat index;', 'struct PICEntry {\n\tfloat index;\n\tfloat cycle_index;\n\tBit32s cycle_max;\n\tBits cycle_floor, cycle_due;\n\tbool cycle_valid, cycle_integral;')
  anchor='static void AddEntry(PICEntry * entry) {'
  assert anchor in s
  helper='''#ifndef DB_PIC_EVENT_CACHE
#define DB_PIC_EVENT_CACHE 0
#endif
static inline float PIC_EventCycles(PICEntry *entry) {
#if DB_PIC_EVENT_CACHE
 if (!entry->cycle_valid || entry->cycle_max != CPU_CycleMax) {
  entry->cycle_index = entry->index * CPU_CycleMax;
  entry->cycle_max = CPU_CycleMax;
  entry->cycle_valid = true;
#if DB_PIC_EVENT_CACHE == 2
  // In this range integers are exactly representable as float. If a future
  // nonnegative deadline is reduced by a nonnegative integer, its fractional
  // bits remain representable (the magnitude only decreases). Consequently
  // trunc(product-index) == floor(product)-index, with no rounding change.
  entry->cycle_integral = entry->cycle_index >= 0.0f && entry->cycle_index <= 65536.0f;
  if (entry->cycle_integral) {
   entry->cycle_floor = (Bits)entry->cycle_index;
   entry->cycle_due = entry->cycle_floor + (entry->cycle_index != (float)entry->cycle_floor);
  }
#endif
 }
#ifdef DB_NATIVE
 // Native tests check the real queue's invalidation sites on every lookup.
 if (entry->cycle_index != entry->index * CPU_CycleMax)
  E_Exit("Stale PIC deadline cache");
#endif
 return entry->cycle_index;
#else
 return entry->index * CPU_CycleMax;
#endif
}

static inline bool PIC_EventDue(PICEntry *entry, Bits index) {
 float cycles = PIC_EventCycles(entry);
#if DB_PIC_EVENT_CACHE == 2
 if (entry->cycle_integral && index >= 0 && index <= 65536) {
  bool result = index >= entry->cycle_due;
#ifdef DB_NATIVE
  if (result != (cycles <= index)) E_Exit("PIC due comparison changed");
#endif
  return result;
 }
#endif
 return cycles <= index;
}
static inline Bits PIC_EventBudget(PICEntry *entry, Bits index) {
 float cycles = PIC_EventCycles(entry);
#if DB_PIC_EVENT_CACHE == 2
 if (entry->cycle_integral && index >= 0 && index <= entry->cycle_floor) {
  Bits result = entry->cycle_floor - index;
#ifdef DB_NATIVE
  if (result != (Bits)(cycles-index)) E_Exit("PIC cycle budget changed");
#endif
  return result;
 }
#endif
 return (Bits)(cycles-index);
}
#ifdef DB_NATIVE
// Exercise the actual compiled helpers, including cached CPU-budget changes.
void zz_pic_deadline_case(float when, Bits index) {
 PICEntry entry = {};
 entry.index = when;
 const Bit32s budgets[] = {1,500,1000,3000,6000,9999,10000,3000};
 for (unsigned i=0;i<sizeof(budgets)/sizeof(*budgets);i++) {
  CPU_CycleMax = budgets[i];
  float expected = when * CPU_CycleMax;
  for (unsigned repeat=0;repeat<2;repeat++) {
   if (PIC_EventDue(&entry,index) != (expected<=index) ||
       PIC_EventBudget(&entry,index) != (Bits)(expected-index))
    E_Exit("PIC deadline differential mismatch");
  }
 }
}
#endif

'''
  s=s.replace(anchor,helper+anchor)
  assert s.count('PICEntry * entry=pic_queue.free_entry;')==1
  s=s.replace('PICEntry * entry=pic_queue.free_entry;', 'PICEntry * entry=pic_queue.free_entry;\n\tentry->cycle_valid=false;')
  assert s.count('entry->index -= 1.0;')==1
  s=s.replace('entry->index -= 1.0;', 'entry->index -= 1.0;\n\t\tentry->cycle_valid=false;')
  assert s.count('pic_queue.next_entry->index*CPU_CycleMax')==2
  s=s.replace('pic_queue.next_entry->index*CPU_CycleMax<=index_nd','PIC_EventDue(pic_queue.next_entry,index_nd)')
  return s.replace('(Bits)(pic_queue.next_entry->index*CPU_CycleMax-index_nd)','PIC_EventBudget(pic_queue.next_entry,index_nd)')
 change('src/hardware/pic.cpp',pic_cache)
 change('src/hardware/vga.cpp',lambda s:s[:s.index('void SVGA_Setup_Driver(void) {')]+'void SVGA_Setup_Driver(void) { memset(&svga,0,sizeof(svga));vga.vmemsize=vga.vmemwrap=256*1024; }\n')
 change('src/hardware/vga_memory.cpp',lambda s:s+'\nvoid zz_vga_memory_close(){VGA_Memory_ShutDown(nullptr);}\n')
 change('src/ints/bios_keyboard.cpp',lambda s:s.replace('#include "SDL.h"','#define SDL_VERSION_ATLEAST(a,b,c) 0'))
 def bios(s):
  s=s.replace('#include "serialport.h"','#include "dos_inc.h"')
  a=s.index('static void BIOS_HostTimeSync() {');b=s.index('static Bitu INT8_Handler',a)
  return s[:a]+'''static void BIOS_HostTimeSync(){dos.date.day=1;dos.date.month=1;dos.date.year=1980;mem_writed(BIOS_TIMER,0);}
'''+s[b:]
 change('src/ints/bios.cpp',bios)
 def disks(s):
  s=s.replace('#include "../dos/drives.h"','#include "drive.h"\nextern Bit8u db_image_sector(bool,Bit32u,void *,bool);')
  a=s.index('void swapInNextDisk(');b=s.index('Bit8u imageDisk::Read_Sector',a)
  s=s[:a]+s[b:]
  a=s.index('Bit8u imageDisk::Read_AbsoluteSector');b=s.index('imageDisk::imageDisk(',a)
  s=s[:a]+'''Bit8u imageDisk::Read_AbsoluteSector(Bit32u sector,void *data){return db_image_sector(hardDrive,sector,data,false);}
Bit8u imageDisk::Write_Sector(Bit32u head,Bit32u cylinder,Bit32u sector,void*data){
 if(head>=heads||cylinder>=cylinders||!sector)return 4;
 return Write_AbsoluteSector((cylinder*heads+head)*sectors+sector-1,data);
}
Bit8u imageDisk::Write_AbsoluteSector(Bit32u sector,void*data){return db_image_sector(hardDrive,sector,data,true);}
'''+s[b:]
  s=s.replace('Bit32u sectnum;\n\n\tsectnum =', 'Bit32u sectnum;\n if(head>=heads||cylinder>=cylinders||!sector)return 4;\n\tsectnum =')
  s=s.replace('fseek(diskimg,0,SEEK_SET);','/* Read-only image is preloaded inside the owned ARM allocation. */')
  # Validate the starting CHS, then permit a multi-sector request to cross
  # tracks as upstream does. Each linear sector remains image-bounded.
  s=s.replace('for(i=0;i<reg_al;i++) {', 'if(!reg_al||!(reg_cl&63)||(reg_cl&63)>imageDiskList[drivenum]->sectors){reg_ah=4;CALLBACK_SCF(true);return CBRET_NONE;}\n\t\tfor(i=0;i<reg_al;i++) {')
  s=s.replace('if(last_status != 0x00) {\n            CALLBACK_SCF(true);','if(last_status != 0x00) {\n            reg_ah=last_status;CALLBACK_SCF(true);')
  s=s.replace('MAPPER_AddHandler(swapInNextDisk,MK_f4,MMOD1,"swapimg","Swap Image");','')
  s=s.replace('mem_writeb(BIOS_HARDDISK_COUNT,2);','mem_writeb(BIOS_HARDDISK_COUNT,0);')
  # Formatting an immutable image must fail, rather than pretend success.
  a=s.index('case 0x05: /* Format track */');b=s.index('case 0x08:',a)
  s=s[:a]+'case 0x05: reg_ah=3;CALLBACK_SCF(true);break;\n\t'+s[b:]
  return s
 change('src/ints/bios_disk.cpp',disks)
 sources += [OUT/'src/hardware'/f'{n}.cpp' for n in HW]+[OUT/'src/ints'/f'{n}.cpp' for n in INTS]
 frontend=(shell.SRC/'frontend.cpp').read_text();a=frontend.index('static Bitu bios_video()');b=frontend.index('void zz_shell_run()',a);frontend=frontend[:a]+frontend[b:]
 a=frontend.index(' Bitu cb=');b=frontend.index(' first_shell=new',a);frontend=frontend[:a]+frontend[b:];(OUT/'frontend.cpp').write_text(frontend)
 remote=(shell.SRC/'remote_drive.cpp').read_text().replace('__asm__ volatile("dsb sy" ::: "memory");','__atomic_thread_fence(__ATOMIC_SEQ_CST);')
 remote=remote.replace('while(ad_get(s,FS_REPLY)!=sequence)db_shell_poll();', 'while(ad_get(s,FS_REPLY)!=sequence){db_shell_poll();db_native_fs_service();}')
 remote='extern "C" void db_native_fs_service();\n'+remote;(OUT/'native_drive.cpp').write_text(remote)
 (OUT/'pc-integration.patch').write_text(''.join(patch))
 return sources

def main():
 p=argparse.ArgumentParser();p.add_argument('--prepare-only',action='store_true');p.add_argument('--sanitize',action='store_true');p.add_argument('--pic-event-cache',choices=['on','off','cycles'],default='off');p.add_argument('--game',choices=['keen','vette','elite','fixture','boot','hdd-install','hdd','pic-queue'],default='fixture');args=p.parse_args();sources=prepare()
 if args.prepare_only:return
 flags=['-std=c++17','-O2','-g','-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format','-Wno-deprecated-declarations','-DZZ_CONTROL=0x180000','-DDB_NATIVE',*['-I'+str(x) for x in [SRC,shell.SRC,ROOT/'amiga/dosbox/dos',ROOT/'amiga/dosbox/console',OUT,OUT/'include',OUT/'src/cpu',OUT/'src/ints',ROOT/'amiga/dosbox',ROOT/'amiga/mesen_bench',ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']]]
 if args.sanitize:flags+=['-fsanitize=address','-fno-omit-frame-pointer']
 flags+=['-DDB_PIC_EVENT_CACHE='+str({'off':0,'on':1,'cycles':2}[args.pic_event_cache])]
 sources += [SRC/'adapter.cpp',OUT/'frontend.cpp',OUT/'native_drive.cpp',SRC/'devices.cpp',SRC/'boot.cpp',SRC/'video.cpp',SRC/'native.cpp']
 sources += [ROOT/'tests/amiga/dosbox/pic_queue.cpp']
 headers=b''.join(x.read_bytes() for d in [OUT/'include',SRC,shell.SRC,ROOT/'amiga/dosbox/dos'] for x in sorted(d.glob('*.h')))
 objects=[]
 for i,s in enumerate(sources):
  o=OUT/(f'{i}-'+s.stem+'.native.o');stamp=o.with_suffix('.sha256');objects.append(str(o));digest=hashlib.sha256(s.read_bytes()+headers+str(flags).encode()).hexdigest()
  if not stamp.exists() or stamp.read_text()!=digest:subprocess.run(['clang++',*flags,'-c',str(s),'-o',str(o)],check=True);stamp.write_text(digest)
 for s in [shell.SRC/'fs_host.c',SRC/'native_fs.c']:
  o=OUT/(s.stem+'.native.o');objects.append(str(o));cf=[f for f in flags if f not in ['-std=c++17','-fno-rtti','-fno-exceptions']]
  subprocess.run(['clang','-std=c11','-I'+str(ROOT/'tests/amiga/dosbox/fs_mock'),*cf,'-c',str(s),'-o',str(o)],check=True)
 subprocess.run(['clang++',*flags,*objects,'-Wl,-dead_strip','-o',str(OUT/'pc-test')],check=True)
 subprocess.run([str(OUT/'pc-test'),args.game],check=True,cwd=ROOT)
if __name__=='__main__':main()
