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
#include "cd_host.h"
#include "pc_wire.h"
#include "mouse_input.h"
#include "cpu.h"
#include "midi_wire.h"
extern "C" {
#include "fs_host.h"
}
alignas(64) static uint8_t shared[0x190000];
extern "C" volatile uint8_t *db_shared(){return shared;}
extern "C" void db_native_fs_service(){db_fs_service();}
static std::string midi_bytes;
extern "C" void db_native_midi_service(){unsigned r=ad_get(shared,PC_MIDI_READ),w=ad_get(shared,PC_MIDI_WRITE);assert(w-r<=PC_MIDI_SIZE);while(r!=w)midi_bytes+=(char)shared[PC_MIDI_DATA+(r++&(PC_MIDI_SIZE-1))];ad_put(shared,PC_MIDI_READ,r);}
static void sync_range(volatile void*,size_t,void*){}static void barrier(void*){}
static bool mouse_mode;static unsigned mouse_stage;
static bool boot_mode,cd_boot,welcome_mode;static unsigned welcome_border;
static std::string input,output,boot_text;static unsigned pos;static unsigned limit_ms=8000;
static int read(void*){return pos<input.size()?(uint8_t)input[pos++]:-1;}
static int ready(void*){return pos<input.size();}
static int write(void*,const uint8_t*p,unsigned n){output.append((const char*)p,n);return 1;}
static int stop(){if((boot_mode||welcome_mode)&&PIC_Ticks>=limit_ms&&boot_text.empty()){welcome_border=mem_readw(0xb8000+162);for(unsigned i=0;i<80*25;i++)boot_text+=(char)mem_readb(0xb8000+i*2);}return PIC_Ticks>=limit_ms;}
#include "../../../tests/amiga/dosbox/native_interactive.inc"
void db_pc_tick(){
 if(mouse_mode){unsigned stage=mem_readb(0x500);if(stage&&stage<5&&stage!=mouse_stage){
  unsigned w=ad_get(shared,PC_MOUSE_WRITE),p=PC_MOUSE_DATA+(w&(PC_MOUSE_SIZE-1))*PC_MOUSE_STRIDE;
  ad_put(shared,p,16384);ad_put(shared,p+4,49151);ad_put(shared,p+8,stage==1||stage==4?1:stage==2?6:0);ad_put(shared,p+12,stage!=3);
  ad_put(shared,PC_MOUSE_WRITE,w+1);mouse_stage=stage;
 }}
 db_mouse_tick(db_shared());
 interactive_tick();
 db_native_midi_service();
 // One scheduled Enter key for menu progression; actual PC keyboard/IRQ path.
 if(boot_mode&&!cd_boot&&!interactive&&strncmp(db_boot_name,"SYS",3)){const KBD_KEYS keys[]={KBD_v,KBD_e,KBD_r,KBD_enter,KBD_m,KBD_e,KBD_m,KBD_enter};unsigned t=PIC_Ticks;if(t>=5000&&t<5800){unsigned i=(t-5000)/100;if((t-5000)%100==0)KEYBOARD_AddKey(keys[i],true);if((t-5000)%100==50)KEYBOARD_AddKey(keys[i],false);}}
 if(!interactive&&PIC_Ticks==4000)KEYBOARD_AddKey(KBD_enter,true);
 if(!interactive&&PIC_Ticks==4100)KEYBOARD_AddKey(KBD_enter,false);
}
int main(int argc,char**argv){
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
 if(game=="cd"||game=="cdboot"){
  unsigned sectors=db_cd_host_open(1,".context/amiga/dosbox-startup-cd/TEST.ISO");assert(sectors==32);ad_put(shared,PC_VIDEO+944,sectors);
  if(game=="cd"){input="dir d:\\\rtype d:\\hello.txt\rcopy d:\\large.bin c:\\CDLARGE.BIN\rcopy d:\\subdir\\nested.txt c:\\CDNEST.TXT\recho bad > d:\\DENIED.TXT\rexit\r";}
  else{boot_mode=cd_boot=true;db_memory_mb=8;limit_ms=14000;strcpy(db_hdd_name,"HDD.IMG");}
 }
 if(game=="midi"){
  std::filesystem::copy_file(".context/amiga/dosbox-pc/MIDITEST.COM",root+"/MIDITEST.COM",std::filesystem::copy_options::overwrite_existing);
  ad_put(shared,PC_MIDI_MODE,1);ad_put(shared,PC_MIDI_WRITE,0xffffff00);ad_put(shared,PC_MIDI_READ,0xffffff00);input="miditest\rexit\r";
 }
 if(game=="mouse"){
  mouse_mode=true;std::filesystem::copy_file(".context/amiga/dosbox-pc/MOUSETST.COM",root+"/MOUSETST.COM",std::filesystem::copy_options::overwrite_existing);
  ad_put(shared,PC_MOUSE_WRITE,0xfffffffe);ad_put(shared,PC_MOUSE_READ,0xfffffffe);input="mousetst\rexit\r";
 }
 if(game=="welcome"){
  welcome_mode=true;db_vga_console=true;limit_ms=1000;
  std::filesystem::copy_file(".context/amiga/dosbox-pc/ZZHELLO.COM",root+"/ZZHELLO.COM",std::filesystem::copy_options::overwrite_existing);input="zzhello\r";
 }
 if(game=="wfw"){
  interactive=boot_mode=true;db_memory_mb=8;limit_ms=3600000;strcpy(db_hdd_name,"HDD.IMG");input="";
  unsigned sectors=db_cd_host_open(1,".context/amiga/wfw311/INSTALL.ISO");assert(sectors);ad_put(shared,PC_VIDEO+944,sectors);
  interactive_start=std::chrono::steady_clock::now();
 }
 ad_io sync{sync_range,sync_range,barrier,barrier,nullptr};assert(db_fs_open(shared,&sync,root.c_str()));
 DbConsoleIO io{nullptr,read,ready,write};DosboxResult result{};int rc=db_run_io(nullptr,0,interactive?0xffffffffu:20000000,&result,stop,&io);db_native_midi_service();db_fs_close();db_cd_host_close();
 printf("PC rc=%d fault=%u detail=%s slices=%u ticks=%u frames=%u size=%ux%u bpp=%u\n%s\n",rc,result.fault,result.text,result.slices,(unsigned)PIC_Ticks,db_frames,db_width,db_height,db_bpp,output.c_str());
 if(db_frames){std::ofstream f(".context/amiga/dosbox-pc/"+game+".ppm",std::ios::binary);f<<"P6\n"<<db_width<<" "<<db_height<<"\n255\n";for(unsigned i=0;i<db_width*db_height;i++){char rgb[]={(char)(db_pixels[i]>>16),(char)(db_pixels[i]>>8),(char)db_pixels[i]};f.write(rgb,3);}}
 if(game=="boot"){const std::string &text=boot_text;printf("BOOT TEXT: %s\n",text.c_str());assert(rc==2&&text.find("A:\\>")!=std::string::npos&&text.find("MS-DOS Version 6.22")!=std::string::npos&&text.find("8,192K")!=std::string::npos);}
 if(game=="hdd"||game=="hdd-install"){
  printf("BOOT TEXT: %s\n",boot_text.c_str());
  assert(rc==2&&boot_text.find(game=="hdd"?"C:\\>":"SYS FINISHED")!=std::string::npos);
  if(game=="hdd-install")assert(boot_text.find("System transferred")!=std::string::npos);
  else assert(boot_text.find("MS-DOS Version 6.22")!=std::string::npos);
 }
 if(game=="welcome"){assert(rc==2&&welcome_border==0x1bc9&&boot_text.find("DosBox ZZ9000 project")!=std::string::npos&&boot_text.find("DOS 5.00")!=std::string::npos&&boot_text.find("Return to Workbench")!=std::string::npos);puts("PASS welcome DOS version, coloured CP437 frame, BIOS cursor and prompt return");}
 if(game=="cd"){assert(rc==0&&output.find("ZZCD data read PASS")!=std::string::npos);std::ifstream f(root+"/CDLARGE.BIN",std::ios::binary);std::string bytes((std::istreambuf_iterator<char>(f)),{});assert(bytes.size()==5003);for(unsigned i=0;i<bytes.size();i++)assert((unsigned char)bytes[i]==i%251);assert(std::filesystem::exists(root+"/CDNEST.TXT"));puts("PASS ISO9660 D: read/copy/nested/cross-sector and write protection");}
 if(game=="cdboot"){printf("BOOT TEXT: %s\n",boot_text.c_str());assert(rc==2&&boot_text.find("ZZCD data read PASS")!=std::string::npos);puts("PASS real DOS6.22 ZZDOSCD.SYS + Microsoft MSCDEX read D:");}
 if(game=="midi"){
  const unsigned char tail[]={0x91,60,64,62,65,0xf8,0x81,60,0,0xf0,0x7d,1,2,3,0xf7};
  assert(rc==0&&output.find("MPU401 intelligent/UART PASS")!=std::string::npos);
  assert(midi_bytes.size()==5000+sizeof(tail));for(unsigned i=0;i<5000;i++)assert((unsigned char)midi_bytes[i]==0xf8);
  assert(!memcmp(midi_bytes.data()+5000,tail,sizeof(tail)));puts("PASS MPU-401 version/UART, MIDI running status/SysEx bytes, queue wrap/backpressure");
 }
 if(game=="mouse"){printf("Mouse stage=%u host=%u\n",mem_readb(0x500),mouse_stage);fflush(stdout);assert(rc==0&&mem_readb(0x500)==5&&output.find("DOS mouse + PS2 IRQ12 PASS")!=std::string::npos);puts("PASS mouse queue wrap, scaled position, 3 buttons, focus release, BIOS PS2 callback");}
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
