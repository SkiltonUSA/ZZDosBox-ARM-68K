/* GPL-2.0-or-later. Bounded native integration test; no real Amiga claims. */
#include <stdio.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <cstdlib>
#include "console_io.h"
#include "pc.h"
#include "dosbox.h"
#include "pic.h"
#include "keyboard.h"
#include "mem.h"
#include "cd_host.h"
#include "pc_wire.h"
#include "serial_wire.h"
#include "fs_wire.h"
#include "mouse_input.h"
#include "cpu.h"
#include "capabilities.h"
#ifdef DB_COMPAT
#include "compat.h"
#endif
#include "midi_wire.h"
extern "C" {
#include "fs_host.h"
}
#ifdef DB_COMPAT
#include "audio_wire.h"
#include "audio.h"
static unsigned audio_frames,audio_nonzero;
#endif
alignas(64) static uint8_t shared[0x1c0000];
#ifdef DB_COMPAT
void db_native_audio_service(){
 unsigned r=ad_get(shared,PC_AUDIO_READ),w=ad_get(shared,PC_AUDIO_WRITE);assert(w-r<=PC_AUDIO_SLOTS);
 while(r!=w){unsigned at=PC_AUDIO_DATA+(r&(PC_AUDIO_SLOTS-1))*PC_AUDIO_STRIDE;for(unsigned i=0;i<PC_AUDIO_STRIDE;i++)audio_nonzero+=shared[at+i]!=0;audio_frames+=PC_AUDIO_FRAMES;r++;}ad_put(shared,PC_AUDIO_READ,r);
}
#endif
extern "C" volatile uint8_t *db_shared(){return shared;}
extern "C" void db_native_fs_service(){
 unsigned op=ad_get(shared,FS_CMD+4);db_fs_service();
 if(getenv("ZZ_FS_TRACE")&&(op==FS_OPEN||op==FS_STAT||op==FS_CREATE||op==FS_TESTDIR))
  fprintf(stderr,"FS op=%u dir=%s name=%s err=%u size=%u date=%u time=%u\n",op,(const char*)shared+FS_DIR,(const char*)shared+FS_NAME,ad_get(shared,FS_REPLY+4),ad_get(shared,FS_REPLY+12),ad_get(shared,FS_REPLY+20),ad_get(shared,FS_REPLY+24));
}
static std::string midi_bytes;
extern "C" void db_native_midi_service(){unsigned r=ad_get(shared,PC_MIDI_READ),w=ad_get(shared,PC_MIDI_WRITE);assert(w-r<=PC_MIDI_SIZE);while(r!=w)midi_bytes+=(char)shared[PC_MIDI_DATA+(r++&(PC_MIDI_SIZE-1))];ad_put(shared,PC_MIDI_READ,r);}
static void sync_range(volatile void*,size_t,void*){}static void barrier(void*){}
static bool mouse_mode;static unsigned mouse_stage;
static bool boot_mode,cd_boot,welcome_mode;static unsigned welcome_border,welcome_title;
static std::string input,output,boot_text;static unsigned pos;static unsigned limit_ms=8000;
static int read(void*){return pos<input.size()?(uint8_t)input[pos++]:-1;}
static int ready(void*){return pos<input.size();}
static int write(void*,const uint8_t*p,unsigned n){output.append((const char*)p,n);return 1;}
static int stop(){if((boot_mode||welcome_mode)&&PIC_Ticks>=limit_ms&&boot_text.empty()){welcome_border=mem_readw((machine==MCH_HERC?0xb0000:0xb8000)+162);welcome_title=mem_readb((machine==MCH_HERC?0xb0000:0xb8000)+3*160+13);for(unsigned i=0;i<80*25;i++)boot_text+=(char)mem_readb((machine==MCH_HERC?0xb0000:0xb8000)+i*2);}return PIC_Ticks>=limit_ms;}
#include "../../../tests/amiga/dosbox/native_interactive.inc"
#ifdef DB_EXPANDED
#include "joystick_input.h"
#include "serial_wire.h"
#endif
void db_pc_tick(){
#ifdef DB_EXPANDED
 db_joystick_tick();
 if(ad_get(shared,PC_SERIAL_MODE)){
  unsigned r=ad_get(shared,PC_SERIAL_TX_READ),w=ad_get(shared,PC_SERIAL_TX_WRITE),rx=ad_get(shared,PC_SERIAL_RX_WRITE);
  assert(w-r<=PC_SERIAL_SIZE);
  while(r!=w){shared[PC_SERIAL_RX_DATA+(rx++&(PC_SERIAL_SIZE-1))]=shared[PC_SERIAL_TX_DATA+(r++&(PC_SERIAL_SIZE-1))];}
  ad_put(shared,PC_SERIAL_TX_READ,r);ad_put(shared,PC_SERIAL_RX_WRITE,rx);
 }
#endif
 if(mouse_mode){unsigned stage=mem_readb(0x500);if(stage&&stage<5&&stage!=mouse_stage){
  unsigned w=ad_get(shared,PC_MOUSE_WRITE),p=PC_MOUSE_DATA+(w&(PC_MOUSE_SIZE-1))*PC_MOUSE_STRIDE;
  ad_put(shared,p,16384);ad_put(shared,p+4,49151);ad_put(shared,p+8,stage==1||stage==4?1:stage==2?6:0);ad_put(shared,p+12,stage!=3);
  ad_put(shared,PC_MOUSE_WRITE,w+1);mouse_stage=stage;
 }}
 db_mouse_tick(db_shared());
 interactive_tick();
#ifdef DB_COMPAT
 db_native_audio_service();
#endif
 db_native_midi_service();
 // One scheduled Enter key for menu progression; actual PC keyboard/IRQ path.
 if(boot_mode&&!cd_boot&&!interactive&&strncmp(db_boot_name,"SYS",3)){const KBD_KEYS keys[]={KBD_v,KBD_e,KBD_r,KBD_enter,KBD_m,KBD_e,KBD_m,KBD_enter};unsigned t=PIC_Ticks;if(t>=5000&&t<5800){unsigned i=(t-5000)/100;if((t-5000)%100==0)KEYBOARD_AddKey(keys[i],true);if((t-5000)%100==50)KEYBOARD_AddKey(keys[i],false);}}
 if(!interactive&&PIC_Ticks==4000)KEYBOARD_AddKey(KBD_enter,true);
 if(!interactive&&PIC_Ticks==4100)KEYBOARD_AddKey(KBD_enter,false);
}
int main(int argc,char**argv){
 assert(argc==2);if(const char*m=getenv("ZZ_TEST_MONO")){db_monochrome=strtoul(m,nullptr,10);assert(db_monochrome<4);}if(const char*m=getenv("ZZ_TEST_MACHINE")){db_machine_option=strtoul(m,nullptr,10);assert(db_machine_option<7);}std::string game=argv[1],root=".context/amiga/dosbox-pc/game-"+(game=="hdd-install"?"hdd":game);
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
 unsigned adapter_w=0,adapter_h=0,adapter_bpp=0;
 if(game=="adapters"){
  char fixture[20];const char*spec=getenv("ZZ_ADAPTER_TEST");if(!spec)spec="3 S3V101 640 480 8";
  assert(sscanf(spec,"%u %19s %u %u %u",&db_machine_option,fixture,&adapter_w,&adapter_h,&adapter_bpp)==5);
  assert(db_machine_option>=3&&db_machine_option<=6);db_memory_mb=8;
  std::string name=std::string(fixture)+".COM";
  std::filesystem::copy_file(".context/amiga/dosbox-pc/"+name,root+"/"+name,std::filesystem::copy_options::overwrite_existing);
  input=name+"\rexit\r";
 }
 if(game=="compatgame"){
  const char*dir=getenv("ZZ_GAME_DIR"),*command=getenv("ZZ_GAME_COMMAND");assert(dir&&command);
  // Every game gets a clean private test copy, including nested assets.
  // Reusing one directory left unrelated games' configuration/data behind.
  assert(std::filesystem::is_directory(dir));
  std::filesystem::remove_all(root);std::filesystem::create_directories(root);
  for(auto &p:std::filesystem::directory_iterator(dir))
   std::filesystem::copy(p.path(),root+"/"+p.path().filename().string(),std::filesystem::copy_options::recursive|std::filesystem::copy_options::overwrite_existing);
  std::filesystem::copy_file(".context/amiga/dosbox-pc/LoadFix.com",root+"/LOADFIX.COM",std::filesystem::copy_options::skip_existing);
  interactive=getenv("ZZ_GAME_INTERACTIVE")!=nullptr;
  interactive_start=std::chrono::steady_clock::now();
#ifdef DB_COMPAT
  if(const char*card=getenv("ZZ_GAME_SOUND")){assert(!strcmp(card,"off")||!strcmp(card,"sb16")||!strcmp(card,"sbpro2"));db_sound_card=!strcmp(card,"sb16")?2:!strcmp(card,"sbpro2");}
#endif
  db_memory_mb=8;limit_ms=getenv("ZZ_GAME_MS")?strtoul(getenv("ZZ_GAME_MS"),nullptr,10):20000;assert(limit_ms<=120000);input=std::string(command)+"\rexit\r";
 }
 if(game=="sb16"||game=="beep"||game=="dates"||game=="x87"){
  const char *name=game=="x87"?"X87TEST.COM":game=="sb16"?"SB16TEST.COM":game=="beep"?"BEEPTEST.COM":"DATETEST.COM";
  std::filesystem::copy_file(std::string(".context/amiga/dosbox-pc/")+name,root+"/"+name,std::filesystem::copy_options::overwrite_existing);input=std::string(name)+"\rexit\r";
#ifdef DB_COMPAT
  db_sound_card=game=="sb16"?2:0;db_pc_speaker=game=="beep";
#endif
 }
 if(game=="opl"){
  std::filesystem::copy_file(".context/amiga/dosbox-pc/OPLTEST.COM",root+"/OPLTEST.COM",std::filesystem::copy_options::overwrite_existing);
  input="opltest\rexit\r";
 }
 if(game=="sb"){
  std::filesystem::copy_file(".context/amiga/dosbox-pc/SBTEST.COM",root+"/SBTEST.COM",std::filesystem::copy_options::overwrite_existing);
  input="sbtest\rexit\r";
 }
 if(game=="serial"){
  ad_put(shared,PC_SERIAL_MODE,1);ad_put(shared,PC_SERIAL_STATUS,3);
  for(unsigned off:{PC_SERIAL_TX_WRITE,PC_SERIAL_TX_READ,PC_SERIAL_RX_WRITE,PC_SERIAL_RX_READ})ad_put(shared,off,0xfffffff8);
  std::filesystem::copy_file(".context/amiga/dosbox-pc/SERIALTEST.COM",root+"/SERIAL.COM",std::filesystem::copy_options::overwrite_existing);input="serial\rexit\r";
 }
 if(game=="gus"||game=="cms"||game=="tandy"||game=="joystick"){
  std::string name=game=="gus"?"GUSTEST.COM":game=="cms"?"CMSTEST.COM":game=="tandy"?"TANDTEST.COM":"JOYTEST.COM";
#ifdef DB_COMPAT
  db_sound_card=0;db_extra_sound=game=="gus"?1:game=="cms"?2:game=="tandy"?3:0;
#endif
  if(game=="joystick"){ad_put(shared,PC_JOYSTICK_ENABLED,1);ad_put(shared,PC_JOYSTICK_STATE,1|8|16);}
  std::filesystem::copy_file(".context/amiga/dosbox-pc/"+name,root+"/"+name,std::filesystem::copy_options::overwrite_existing);input=name+"\rexit\r";
 }
 if(game=="cpu"||game=="extended"){
  const char*name=game=="cpu"?"CPUTEST.COM":"EXTTEST.COM";
  if(const char*model=getenv("ZZ_TEST_CPU")){db_cpu_model=strtoul(model,nullptr,10);assert(db_cpu_model<DB_CPU_COUNT);}
  if(game=="extended"){
   db_memory_mb=32;
#ifdef DB_COMPAT
   db_vcpi_enabled=db_umb_enabled=true;db_sound_card=0;
#endif
  }
  std::filesystem::copy_file(std::string(".context/amiga/dosbox-pc/")+name,root+"/"+name,std::filesystem::copy_options::overwrite_existing);input=std::string(name)+"\rexit\r";
 }
 if(game=="ems"){
  db_memory_mb=8;
  std::filesystem::copy_file(".context/amiga/dosbox-pc/EMSTEST.COM",root+"/EMSTEST.COM",std::filesystem::copy_options::overwrite_existing);
  input="emstest\rexit\r";
 }
#ifdef DB_COMPAT
 ad_put(shared,PC_AUDIO_WRITE,0xfffffffeu);ad_put(shared,PC_AUDIO_READ,0xfffffffeu);
#endif
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
 if(game=="welcome"){assert(rc==2&&welcome_border==0x1bc9&&welcome_title==0x1e&&boot_text.find("github.com")==std::string::npos&&boot_text.find("Ctrl+F5          Capture screenshot")!=std::string::npos&&boot_text.find("DosBox ZZ9000 project")!=std::string::npos&&boot_text.find("DOS 5.00")!=std::string::npos&&boot_text.find("Return to Workbench")!=std::string::npos);puts("PASS welcome yellow ASCII, capture shortcut, no URL, DOS version, border and prompt return");}
 if(game=="cd"){assert(rc==0&&output.find("ZZCD data read PASS")!=std::string::npos);std::ifstream f(root+"/CDLARGE.BIN",std::ios::binary);std::string bytes((std::istreambuf_iterator<char>(f)),{});assert(bytes.size()==5003);for(unsigned i=0;i<bytes.size();i++)assert((unsigned char)bytes[i]==i%251);assert(std::filesystem::exists(root+"/CDNEST.TXT"));puts("PASS ISO9660 D: read/copy/nested/cross-sector and write protection");}
 if(game=="cdboot"){printf("BOOT TEXT: %s\n",boot_text.c_str());assert(rc==2&&boot_text.find("ZZCD data read PASS")!=std::string::npos);puts("PASS real DOS6.22 ZZDOSCD.SYS + Microsoft MSCDEX read D:");}
 if(game=="midi"){
  const unsigned char tail[]={0x91,60,64,62,65,0xf8,0x81,60,0,0xf0,0x7d,1,2,3,0xf7};
  assert(rc==0&&output.find("MPU401 intelligent/UART PASS")!=std::string::npos);
  assert(midi_bytes.size()==5000+sizeof(tail));for(unsigned i=0;i<5000;i++)assert((unsigned char)midi_bytes[i]==0xf8);
  assert(!memcmp(midi_bytes.data()+5000,tail,sizeof(tail)));puts("PASS MPU-401 version/UART, MIDI running status/SysEx bytes, queue wrap/backpressure");
 }
 if(game=="mouse"){printf("Mouse stage=%u host=%u\n",mem_readb(0x500),mouse_stage);fflush(stdout);assert(rc==0&&mem_readb(0x500)==5&&output.find("DOS mouse + PS2 IRQ12 PASS")!=std::string::npos);puts("PASS mouse queue wrap, scaled position, 3 buttons, focus release, BIOS PS2 callback");}
 if(game=="adapters"){
  assert(rc==0&&result.exit_code==0&&output.find("ADAPTER PASS")!=std::string::npos&&output.find("ADAPTER FAIL")==std::string::npos);
  assert(db_frames>=10&&db_width==adapter_w&&db_height==adapter_h&&db_bpp==adapter_bpp);
  unsigned nonzero=0;for(unsigned i=0;i<db_width*db_height;i++)nonzero+=db_pixels[i]!=0;
  unsigned wanted=db_machine_option==6?0xaaaaaa:db_machine_option==4||db_machine_option==5?0xaa00aa:adapter_bpp==15||adapter_bpp==16?0x00ff00:0xffffff;
  if(db_machine_option==6){static const unsigned expect[]={0xaaaaaa,0xaa7f00,0x22aa22,0xaaa18f};wanted=expect[db_monochrome];}
  for(unsigned i=0;i<db_width*db_height;i++)assert(db_pixels[i]==wanted);
  printf("PASS adapter=%u geometry=%ux%u bpp=%u nonzero=%u pixel=%08x\n",db_machine_option,db_width,db_height,db_bpp,nonzero,db_pixels[db_width*db_height/2]);
 }
#ifdef DB_COMPAT
 if(game=="opl"){db_native_audio_service();printf("OPL frames=%u nonzero_bytes=%u\n",audio_frames,audio_nonzero);assert(rc==0&&audio_nonzero>100&&audio_frames>=16000);}
 if(game=="sb"){db_native_audio_service();printf("PCM frames=%u nonzero_bytes=%u\n",audio_frames,audio_nonzero);assert(rc==0&&audio_nonzero>0&&output.find("SBPro2 DMA1 IRQ7 PASS")!=std::string::npos);}
#endif
 if(game=="ems")assert(rc==0&&output.find("EMS/XMS remap execution and shared allocation PASS")!=std::string::npos);
 if(game=="fixture"){
  const uint32_t colors[]={0x000000,0x0000ad,0x00aa00,0x00aaad,0xad0000,0xad00ad,0xad5500,0xadaaad,0x525552,0x5255ff,0x52ff52,0x52ffff,0xff5552,0xff55ff,0xffff52,0xffffff};
  assert(rc==0&&db_frames>=10&&db_width==320&&db_height==400);
  assert(output.find("nested-pass")!=std::string::npos);
  assert(!std::filesystem::exists(root+"/ZZTEMP"));
  assert(output.find("EGA/PIT PASS")!=std::string::npos);
  for(unsigned y=0;y<400;y++)for(unsigned x=0;x<320;x++)assert(db_pixels[y*320+x]==colors[x/80+(y/100)*4]);
  puts("PASS EGA 16-colour planes, 128000 pixels, PIT/BIOS IRQ timing and DOS return");
 }
#ifdef DB_COMPAT
 if(game=="sb16"){assert(output.find("SB16 DMA5 IRQ7 PASS")!=std::string::npos);assert(audio_nonzero>100&&audio_frames>=1024);}
 if(game=="beep"){assert(output.find("PC speaker tone complete")!=std::string::npos);assert(audio_nonzero>100&&audio_frames>=16000);}
 if(game=="gus"||game=="cms"||game=="tandy"){
  assert(rc==0&&result.exit_code==0&&output.find("Extra sound PASS")!=std::string::npos);
#ifdef DB_COMPAT
  assert(audio_frames>=16000&&audio_nonzero>1000);printf("PASS extra sound PCM frames=%u nonzero=%u\n",audio_frames,audio_nonzero);
#endif
 }
 if(game=="serial")assert(rc==0&&result.exit_code==0&&output.find("COM1 UART PASS")!=std::string::npos);
 if(game=="joystick")assert(rc==0&&result.exit_code==0&&output.find("Gameport PASS")!=std::string::npos);
 if(game=="cpu"){
  const char*names[]={"CPU 386 PASS","CPU 486 PASS","CPU Pentium PASS"};
  assert(rc==0&&result.exit_code==0&&output.find(names[db_cpu_model])!=std::string::npos);
 }
 if(game=="extended")assert(rc==0&&result.exit_code==0&&output.find("Extended RAM VCPI UMB PASS")!=std::string::npos);
 if(game=="x87")assert(output.find("x87 arithmetic rounding state PASS")!=std::string::npos);
 if(game=="dates")assert(output.find("File date open/find match PASS")!=std::string::npos);
 if(game=="sb16"||game=="beep")fprintf(stderr,"PCM frames=%u nonzero_bytes=%u\n",audio_frames,audio_nonzero);
#endif
 return rc==1?1:0;
}
