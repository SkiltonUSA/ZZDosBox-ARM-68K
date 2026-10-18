#!/usr/bin/env python3
"""Stage pinned upstream PC devices and capture native DOSBox frames."""
import argparse,difflib,hashlib,shutil,subprocess,re,json
from pathlib import Path
import build_dosbox_shell as shell
ROOT=shell.ROOT;OUT=ROOT/'.context/amiga/dosbox-pc';SRC=ROOT/'amiga/dosbox/pc';UP=shell.UP
HW=['iohandler','pic','timer','keyboard','cmos','mpu401','vga','vga_attr','vga_crtc','vga_dac','vga_draw','vga_gfx','vga_other','vga_memory','vga_misc','vga_seq','vga_xga','vga_s3']
INTS=['mouse','bios','bios_disk','bios_keyboard','int10','int10_char','int10_memory','int10_misc','int10_modes','int10_vesa','int10_pal','int10_put_pixel','int10_video_state','int10_vptable']
def prepare():
 shell.OUT=OUT;sources=shell.prepare();patch=[]
 planes=bytes(sum(((((x*8+bit)//80+4*(y//50))>>plane)&1)<<(7-bit) for bit in range(8)) for plane in range(4) for y in range(200) for x in range(40))
 (OUT/'ega-planes.bin').write_bytes(planes)
 subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/'egatest.S'),'-o','egatest.o'],cwd=OUT,check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary','egatest.o','-o','EGATEST.COM'],cwd=OUT,check=True)
 for name in ['keytest','cgatest','boottest','pagingtest','hddtest','badmem','miditest','welcome','mousetst','smctest','pitmath','emstest','sbtest','opltest','sb16test','beeptest','datetest','x87test','cputest','exttest','joytest','cmstest','tandtest','gustest','serialtest','filetest','temptest']:
  subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/(name+'.S')),'-o',name+'.o'],cwd=OUT,check=True)
  subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x7c00' if name=='pagingtest' else '0x100','--oformat=binary',name+'.o','-o','RDTEST.COM' if name=='filetest' else name.upper()+'.COM'],cwd=OUT,check=True)
 for name,adapter,mode,bpp in [('S3V101',3,0x101,8),('S3V103',3,0x103,8),('S3V105',3,0x105,8),('S3V110',3,0x110,15),('S3V111',3,0x111,16),('S3V114',3,0x114,16),('S3V10F',3,0x10f,32),('TANTEST',4,9,4),('JRTTEST',5,9,4),('HERCTEST',6,0,1)]:
  subprocess.run(['clang','--target=i386-none-elf',f'-DTEST_ADAPTER={adapter}',f'-DTEST_MODE={mode}',f'-DTEST_BPP={bpp}','-c',str(SRC/'adaptertest.S'),'-o',str(OUT/(name+'.o'))],check=True)
  subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(OUT/(name+'.o')),'-o',str(OUT/(name+'.COM'))],check=True)
 (OUT/'ZZHELLO.COM').write_bytes((OUT/'WELCOME.COM').read_bytes())
 (OUT/'welcome-build.json').write_text(json.dumps({'source_sha256':hashlib.sha256((SRC/'welcome.S').read_bytes()).hexdigest(),'bytes':(OUT/'ZZHELLO.COM').stat().st_size,'sha256':hashlib.sha256((OUT/'ZZHELLO.COM').read_bytes()).hexdigest()},indent=2)+'\n')
 (OUT/'BOOTTEST.IMG').write_bytes((OUT/'BOOTTEST.COM').read_bytes()+bytes([0x39,0x5a])+bytes(1440*1024-514))
 for path in ['src/hardware','src/ints','src/gui']:shutil.copytree(UP/path,OUT/path,dirs_exist_ok=True)
 def change(rel,fn):
  p=OUT/rel;old=p.read_text();new=fn(old);p.write_text(new);patch.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 # Upstream banked software TLB: retain full guest address coverage without
 # a 20 MiB always-resident translation table on 32-bit ARM.
 change('include/paging.h',lambda s:s.replace('#define USE_FULL_TLB','/* USE_FULL_TLB disabled for the bounded XACP allocation. */'))
 # Windows uses sparse high linear addresses. A 256 MiB bank costs 1.25 MiB
 # on ARM and exhausts the 2 MiB heap after only two banks. Keep the full
 # 32-bit address space, but allocate 16 MiB banks (80 KiB each on ARM).
 change('include/paging.h',lambda s:s.replace('#define TLB_SIZE\t\t65536','#define TLB_SIZE\t\t4096').replace('#define BANK_SHIFT\t\t28','#define BANK_SHIFT\t\t24').replace('#define BANK_MASK\t\t0xffff','#define BANK_MASK\t\t0xfff'))
 # Optional native exception trace for protected-mode installation research.
 change('src/cpu/cpu.cpp',lambda s:s.replace('void CPU_Exception(Bitu which,Bitu error ) {', 'void CPU_Exception(Bitu which,Bitu error ) {\n#ifdef DB_NATIVE\n extern void db_native_exception(Bitu,Bitu);db_native_exception(which,error);\n#endif'))
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
 def mpu(text):
  text=text.replace('const char* s_mpu = section->Get_string("mpu401");','const char* s_mpu = db_midi_mode==1?"intelligent":db_midi_mode==2?"uart":"none";')
  text=text.replace('if(strcasecmp(section->Get_string("mpu401"),"intelligent")) return;', 'if(!mpu.intelligent)return;')
  text=text.replace('delete test;', 'PIC_RemoveEvents(MPU401_Event);PIC_RemoveEvents(MPU401_EOIHandler);PIC_RemoveEvents(MPU401_ResetDone);delete test;test=nullptr;')
  return 'extern unsigned db_midi_mode;\n'+text
 change('src/hardware/mpu401.cpp',mpu)
 # Keep INT33 callbacks in BIOS callback storage: booted DOS owns low RAM.
 # Preserve the nonzero segment/offset expected by older DOS applications.
 change('src/ints/mouse.cpp',lambda s:s.replace('RealMake(DOS_GetMemory(0x1)-1,0x10)', 'RealMake(CB_SEG+1,CB_SOFFSET+(call_int33*CB_SIZE)-0x10)')+"\nextern \"C\" bool db_mouse_relative(){return useps2callback;}\n")
 change('src/hardware/iohandler.cpp',lambda s:s.replace('../src/cpu/lazyflags.h','../cpu/lazyflags.h'))
 change('src/hardware/vga.cpp',lambda s:s[:s.index('void SVGA_Setup_Driver(void) {')]+'''
#include "adapters.h"
void SVGA_Setup_Driver(void) {
 memset(&svga,0,sizeof(svga));vga.vmemsize=vga.vmemwrap=256*1024;
 if(svgaCard==SVGA_S3Trio){vga.vmemsize=PC_S3_VRAM_BYTES;SVGA_Setup_S3Trio();}
}
''')
 # Advertise only modes inside both the private capture and video RAM bounds.
 # S3 register/VBE discovery works without emulating a PCI bus.
 def s3(text):
  text=text.replace('return VideoModeMemSize(mode) < vga.vmemsize;','''for(unsigned i=0;ModeList_VGA[i].mode!=0xffff;i++){
   const VideoModeBlock&m=ModeList_VGA[i];
   if(m.mode==mode)return m.swidth<=PC_VIDEO_MAX_WIDTH&&m.sheight<=PC_VIDEO_MAX_HEIGHT&&VideoModeMemSize(mode)<=vga.vmemsize;
  }
  return false;''').replace('PCI_AddSVGAS3_Device();','/* No emulated PCI bus: BIOS/register detection only. */')
  return '#include "adapters.h"\n'+text.replace('#include "mem.h"','#include "mem.h"\n#include "int10.h"')
 change('src/hardware/vga_s3.cpp',s3)
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
 # This port has its own controls; do not advertise unsupported upstream menus.
 def welcome(text):
  messages={
   'SHELL_STARTUP_BEGIN':"DosBox ZZ9000 project                         0.11.0-dev\nAmigaOS - 32-bit | DOSBox / libretro\nhttps://github.com/SkiltonUSA/ZZDosBox-ARM-68K\n\nAlt+Return: window/fullscreen   Ctrl+- / Ctrl++: CPU speed\nF10: close session             HELP: DOS commands\n\n",
   'SHELL_STARTUP_END':'', 'SHELL_STARTUP_CGA':'', 'SHELL_STARTUP_HERC':'', 'SHELL_STARTUP_DEBUG':''}
  messages['SHELL_STARTUP_SUB']=messages['SHELL_STARTUP_BEGIN']
  for key,value in messages.items():
   text=re.sub(r'MSG_Add\("'+key+r'",.*?\);',lambda m:'MSG_Add("'+key+'",'+json.dumps(value)+');',text,flags=re.S)
  return text
 change('shell_messages.cpp',welcome)
 # Keep DOSBox's ISO9660 and MSCDEX implementation; replace platform CD I/O.
 for name in ['dos_mscdex','drive_iso']:
  target=OUT/'src/dos'/f'{name}.cpp';original=(UP/'src/dos'/f'{name}.cpp').read_text();target.write_text(original)
  change('src/dos/'+name+'.cpp',lambda text:text.replace('#include "cdrom.h"','#include "cd_interface.h"').replace('#include "drives.h"','#include "cd_iso.h"'))
 change('src/dos/drive_iso.cpp',lambda text:'extern \"C\" char *strtok_r(char *,const char *,char **);\n'+text.replace('for(char* name = strtok(isoPath, "/"); NULL != name; name = strtok(NULL, "/"))', 'char *token_context=nullptr;\n\tfor(char* name = strtok_r(isoPath, "/", &token_context); NULL != name; name = strtok_r(NULL, "/", &token_context))'))
 def mscdex(text):
  a=text.index('switch (CDROM_GetMountType');b=text.index('\n\tif (!cdrom[numDrives]->SetDevice',a)
  text=text[:a]+'cdrom[numDrives] = new CDROM_Interface_Image((Bit8u)numDrives);\n'+text[b:]
  text=text.replace('(1<<2)', '(0<<2)').replace('(1<<4)', '(0<<4)').replace('(1<<8)', '(0<<8)')
  return text.replace('sec->AddDestroyFunction(&MSCDEX_ShutDown);','if(sec)sec->AddDestroyFunction(&MSCDEX_ShutDown);')
 change('src/dos/dos_mscdex.cpp',mscdex)
 sources += [OUT/'src/dos/dos_mscdex.cpp',OUT/'src/dos/drive_iso.cpp']
 subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/'cddriver.S'),'-o','cddriver.o'],cwd=OUT,check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0','--oformat=binary','cddriver.o','-o','ZZDOSCD.SYS'],cwd=OUT,check=True)
 sources += [OUT/'src/hardware'/f'{n}.cpp' for n in HW]+[OUT/'src/ints'/f'{n}.cpp' for n in INTS]
 # Paging executes a single instruction through the reentrant full decoder.
 sources.append(OUT/'src/cpu/core_full.cpp')
 frontend=(shell.SRC/'frontend.cpp').read_text();a=frontend.index('static Bitu bios_video()');b=frontend.index('void zz_shell_run()',a);frontend=frontend[:a]+frontend[b:]
 a=frontend.index(' Bitu cb=');b=frontend.index(' first_shell=new',a);frontend=frontend[:a]+frontend[b:];(OUT/'frontend.cpp').write_text(frontend)
 remote=(shell.SRC/'remote_drive.cpp').read_text().replace('__asm__ volatile("dsb sy" ::: "memory");','__atomic_thread_fence(__ATOMIC_SEQ_CST);')
 remote=remote.replace('while(ad_get(s,FS_REPLY)!=sequence)db_shell_poll();', 'while(ad_get(s,FS_REPLY)!=sequence){db_shell_poll();db_native_fs_service();}')
 remote='extern "C" void db_native_fs_service();\n'+remote;(OUT/'native_drive.cpp').write_text(remote)
 (OUT/'pc-integration.patch').write_text(''.join(patch))
 return sources

def main():
 p=argparse.ArgumentParser();p.add_argument('--prepare-only',action='store_true');p.add_argument('--sanitize',action='store_true');p.add_argument('--game',choices=['keen','vette','elite','fixture','boot','hdd-install','hdd','cd','cdboot','midi','welcome','wfw','mouse','adapters','ems','sb','opl','sb16','beep','dates','x87','cpu','extended','joystick','gus','cms','tandy','serial','tempfile','compatgame'],default='fixture');p.add_argument('--compat',action='store_true');p.add_argument('--x87',action='store_true');p.add_argument('--expanded',action='store_true');args=p.parse_args();sources=prepare()
 if args.expanded and not (args.compat and args.x87):p.error('--expanded requires --compat --x87')
 if args.compat:
  from prepare_dosbox_compat import prepare as compat_prepare
  sources+=compat_prepare(OUT)+[SRC/'compat.cpp']
  from prepare_dosbox_audio import prepare as audio_prepare
  sources+=audio_prepare(OUT)+[SRC/'audio.cpp']
 if args.x87:
  from prepare_dosbox_x87 import prepare as x87_prepare
  sources+=x87_prepare(OUT)
 if args.expanded:
  from prepare_dosbox_expanded import prepare as expanded_prepare
  sources+=expanded_prepare(OUT)+[SRC/'serial.cpp']
 if args.prepare_only:return
 flags=['-std=c++17','-O2','-g','-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format','-Wno-deprecated-declarations','-DZZ_CONTROL=0x180000','-DDB_NATIVE','-DZZ_PC_CD',*['-I'+str(x) for x in [SRC,shell.SRC,ROOT/'amiga/dosbox/dos',ROOT/'amiga/dosbox/console',OUT,OUT/'include',OUT/'src/cpu',OUT/'src/ints',ROOT/'amiga/dosbox',ROOT/'amiga/common/arm',ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']]]
 if args.expanded:flags+=['-DDB_EXPANDED']
 if args.compat:flags+=['-DDB_COMPAT']
 if args.sanitize:flags+=['-fsanitize=address','-fno-omit-frame-pointer']
 sources += [SRC/'adapter.cpp',OUT/'frontend.cpp',OUT/'native_drive.cpp',SRC/'devices.cpp',SRC/'cd.cpp',SRC/'midi.cpp',SRC/'boot.cpp',SRC/'video.cpp',SRC/'native.cpp']
 headers=b''.join(x.read_bytes() for d in [OUT/'include',SRC,shell.SRC,ROOT/'amiga/dosbox/dos'] for x in sorted(d.glob('*.h')))
 objects=[]
 for i,s in enumerate(sources):
  o=OUT/(f'{i}-'+s.stem+'.native.o');stamp=o.with_suffix('.sha256');objects.append(str(o));extra=(ROOT/'tests/amiga/dosbox/native_interactive.inc').read_bytes() if s==SRC/'native.cpp' else b'';digest=hashlib.sha256(s.read_bytes()+extra+headers+str(flags).encode()).hexdigest()
  if not stamp.exists() or stamp.read_text()!=digest:subprocess.run(['clang++',*flags,'-c',str(s),'-o',str(o)],check=True);stamp.write_text(digest)
 for s in [shell.SRC/'fs_host.c',SRC/'native_fs.c',SRC/'cd_host.c']:
  o=OUT/(s.stem+'.native.o');objects.append(str(o));cf=[f for f in flags if f not in ['-std=c++17','-fno-rtti','-fno-exceptions']]
  subprocess.run(['clang','-std=c11','-I'+str(ROOT/'tests/amiga/dosbox/fs_mock'),*cf,'-c',str(s),'-o',str(o)],check=True)
 subprocess.run(['clang++',*flags,*objects,'-Wl,-dead_strip','-o',str(OUT/'pc-test')],check=True)
 subprocess.run([str(OUT/'pc-test'),args.game],check=True,cwd=ROOT)
if __name__=='__main__':main()
