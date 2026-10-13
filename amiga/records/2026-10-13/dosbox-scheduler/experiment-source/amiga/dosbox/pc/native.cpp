/* GPL-2.0-or-later. Bounded native integration test; no real Amiga claims. */
#include <stdio.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include "console_io.h"
#include "pc.h"
#include "dosbox.h"
#include "pic.h"
#include "keyboard.h"
#include "mem.h"
extern "C" {
#include "fs_host.h"
}
alignas(64) static uint8_t shared[0x190000];
extern "C" volatile uint8_t *db_shared(){return shared;}
extern "C" void db_native_fs_service(){db_fs_service();}
static void sync_range(volatile void*,size_t,void*){}static void barrier(void*){}
static bool boot_mode;
static std::string input,output,boot_text;static unsigned pos;static unsigned limit_ms=8000;
static int read(void*){return pos<input.size()?(uint8_t)input[pos++]:-1;}
static int ready(void*){return pos<input.size();}
static int write(void*,const uint8_t*p,unsigned n){output.append((const char*)p,n);return 1;}
static int stop(){if(boot_mode&&PIC_Ticks>=limit_ms&&boot_text.empty())for(unsigned i=0;i<80*25;i++)boot_text+=(char)mem_readb(0xb8000+i*2);return PIC_Ticks>=limit_ms;}
void db_pc_tick(){
 // One scheduled Enter key for menu progression; actual PC keyboard/IRQ path.
 if(boot_mode&&strncmp(db_boot_name,"SYS",3)){const KBD_KEYS keys[]={KBD_v,KBD_e,KBD_r,KBD_enter,KBD_m,KBD_e,KBD_m,KBD_enter};unsigned t=PIC_Ticks;if(t>=5000&&t<5800){unsigned i=(t-5000)/100;if((t-5000)%100==0)KEYBOARD_AddKey(keys[i],true);if((t-5000)%100==50)KEYBOARD_AddKey(keys[i],false);}}
 if(PIC_Ticks==4000)KEYBOARD_AddKey(KBD_enter,true);
 if(PIC_Ticks==4100)KEYBOARD_AddKey(KBD_enter,false);
}
int main(int argc,char**argv){
 if(argc==2&&!strcmp(argv[1],"pic-queue")){extern int test_pic_queue();return test_pic_queue();}
 assert(argc==2);std::string game=argv[1],root=".context/amiga/dosbox-pc/game-"+(game=="hdd-install"?"hdd":game);
 std::filesystem::create_directories(root);
 std::string from=game=="keen"?"Games/KEEN6":game=="vette"?"Games/VETTE":game=="elite"?"Games/ELITE":"CDrive";
 if(game!="boot"&&game!="hdd"&&game!="hdd-install")for(auto &p:std::filesystem::directory_iterator(".context/amiga/dosbox-shell/"+from))std::filesystem::copy_file(p.path(),root+"/"+p.path().filename().string(),std::filesystem::copy_options::overwrite_existing);
 if(game=="vette"||game=="elite")std::filesystem::copy_file(".context/amiga/dosbox-pc/LoadFix.com",root+"/LOADFIX.COM",std::filesystem::copy_options::overwrite_existing);
 if(game=="fixture")std::filesystem::copy_file(".context/amiga/dosbox-pc/EGATEST.COM",root+"/EGATEST.COM",std::filesystem::copy_options::overwrite_existing);
 input=game=="keen"?"keen6c\r":game=="vette"?"loadfix vette.exe\r":game=="elite"?"loadfix elite.exe\r":"md ZZTEMP\rmd ZZTEMP\\SAVE\rcd ZZTEMP\\SAVE\recho nested-pass > STATE.TXT\rtype STATE.TXT\rcd \\\rcd ZZTEMP\rdir /b\rcd \\\rdel ZZTEMP\\SAVE\\STATE.TXT\rrd ZZTEMP\\SAVE\rrd ZZTEMP\rfiletest\rexetest\regatest\rexit\r";
 if(game=="boot"){boot_mode=true;db_memory_mb=8;extern char db_boot_name[13];strcpy(db_boot_name,"BOOT.IMG");std::filesystem::copy_file(".context/amiga/msdos622/alternatives/Dos6.22.img",root+"/BOOT.IMG",std::filesystem::copy_options::overwrite_existing);}
 if(game=="hdd"||game=="hdd-install"){
  boot_mode=true;db_memory_mb=8;limit_ms=12000;strcpy(db_hdd_name,"HDD.IMG");
  if(game=="hdd-install"){
   strcpy(db_boot_name,"SYSBOOT.IMG");
   for(const auto *name:{"HDD.IMG","SYSBOOT.IMG"})std::filesystem::copy_file(std::string(".context/amiga/dosbox-hdd/")+name,root+"/"+name,std::filesystem::copy_options::overwrite_existing);
  }
 }
 ad_io sync{sync_range,sync_range,barrier,barrier,nullptr};assert(db_fs_open(shared,&sync,root.c_str()));
 DbConsoleIO io{nullptr,read,ready,write};DosboxResult result{};int rc=db_run_io(nullptr,0,20000000,&result,stop,&io);db_fs_close();
 printf("PC rc=%d fault=%u detail=%s slices=%u ticks=%u frames=%u size=%ux%u bpp=%u\n%s\n",rc,result.fault,result.text,result.slices,(unsigned)PIC_Ticks,db_frames,db_width,db_height,db_bpp,output.c_str());
 if(db_frames){std::ofstream f(".context/amiga/dosbox-pc/"+game+".ppm",std::ios::binary);f<<"P6\n"<<db_width<<" "<<db_height<<"\n255\n";for(unsigned i=0;i<db_width*db_height;i++){char rgb[]={(char)(db_pixels[i]>>16),(char)(db_pixels[i]>>8),(char)db_pixels[i]};f.write(rgb,3);}}
 if(game=="boot"){const std::string &text=boot_text;printf("BOOT TEXT: %s\n",text.c_str());assert(rc==2&&text.find("A:\\>")!=std::string::npos&&text.find("MS-DOS Version 6.22")!=std::string::npos&&text.find("8,192K")!=std::string::npos);}
 if(game=="hdd"||game=="hdd-install"){
  printf("BOOT TEXT: %s\n",boot_text.c_str());
  assert(rc==2&&boot_text.find(game=="hdd"?"C:\\>":"SYS FINISHED")!=std::string::npos);
  if(game=="hdd-install")assert(boot_text.find("System transferred")!=std::string::npos);
  else assert(boot_text.find("MS-DOS Version 6.22")!=std::string::npos);
 }
 if(game=="fixture"){
  const uint32_t colors[]={0x000000,0x0000ad,0x00aa00,0x00aaad,0xad0000,0xad00ad,0xad5500,0xadaaad,0x525552,0x5255ff,0x52ff52,0x52ffff,0xff5552,0xff55ff,0xffff52,0xffffff};
  assert(rc==0&&db_frames>=10&&db_width==320&&db_height==400);
  assert(output.find("nested-pass")!=std::string::npos);
  assert(!std::filesystem::exists(root+"/ZZTEMP"));
  assert(output.find("EGA/PIT PASS")!=std::string::npos);
  for(unsigned y=0;y<400;y++)for(unsigned x=0;x<320;x++)assert(db_pixels[y*320+x]==colors[x/80+(y/100)*4]);
  puts("PASS EGA 16-colour planes, 128000 pixels, PIT/BIOS IRQ timing and DOS return");
 }
 return rc==1?1:0;
}
