/* Native-only installation research harness; never part of the ARM payload.
 * Input lines: numeric KBD_KEYS value, pressed (0/1). No supplied media in Git. */
#include <chrono>
#include <thread>
static bool interactive;
static unsigned interactive_keys;
static auto interactive_start=std::chrono::steady_clock::now();
void db_native_exception(Bitu which,Bitu error){
 static unsigned count;
 if(!interactive||!getenv("ZZDOS_TRACE")||count++>=200)return;
 fprintf(stderr,"tick=%u exception=%u error=%x at=%04x:%08x base=%08x cr0=%08x ax=%08x bx=%08x cx=%08x dx=%08x ss:sp=%04x:%08x bytes=",PIC_Ticks,(unsigned)which,(unsigned)error,SegValue(cs),reg_eip,SegPhys(cs),cpu.cr0,reg_eax,reg_ebx,reg_ecx,reg_edx,SegValue(ss),reg_esp);
 for(unsigned i=0;i<16;i++)fprintf(stderr,"%02x",mem_readb(SegPhys(cs)+reg_eip+i));
 fprintf(stderr,"\n");fflush(stderr);
 if(SegValue(cs)==0x53&&which==13){
  fprintf(stderr,"ds=%x/%x es=%x/%x flags=%x gdt=%x/%x\n",SegValue(ds),SegPhys(ds),SegValue(es),SegPhys(es),reg_flags,(unsigned)cpu.gdt.GetBase(),(unsigned)cpu.gdt.GetLimit());
  for(unsigned sel: {0x14u,0xa4u,0x3b7u}){Descriptor d;bool ok=cpu.gdt.GetDescriptor(sel,d);fprintf(stderr,"descriptor=%x ok=%u base=%x limit=%x type=%x present=%u dpl=%u\n",sel,ok,(unsigned)d.GetBase(),(unsigned)d.GetLimit(),(unsigned)d.Type(),d.saved.seg.p,d.DPL());}
  std::ofstream f(".context/amiga/wfw311/fault-ram.bin",std::ios::binary);f.write((char*)MemBase,db_memory_mb*1048576u);
 }
}
static void interactive_tick(){
 if(!interactive)return;
 if(PIC_Ticks%20==0){
  std::ifstream keys(".context/amiga/wfw311/keys.txt");unsigned key,down,n=0;
  while(keys>>key>>down){if(n++==interactive_keys){if(key>0&&key<KBD_LAST&&down<2)KEYBOARD_AddKey((KBD_KEYS)key,down);interactive_keys++;break;}}
 }
 if(PIC_Ticks%500==0){
  std::string text;
  for(unsigned i=0;i<2000;i++){unsigned c=mem_readb(0xb8000+i*2);text+=c>=32&&c<127?(char)c:' ';if(i%80==79)text+='\n';}
  {std::ofstream f(".context/amiga/wfw311/screen.txt");f<<"ticks="<<PIC_Ticks<<" frames="<<db_frames<<"\n"<<text;}
  if(db_width&&db_height){std::ofstream f(".context/amiga/wfw311/screen.tmp",std::ios::binary);f<<"P6\n"<<db_width<<" "<<db_height<<"\n255\n";for(unsigned i=0;i<db_width*db_height;i++){char rgb[]={(char)(db_pixels[i]>>16),(char)(db_pixels[i]>>8),(char)db_pixels[i]};f.write(rgb,3);}f.close();std::filesystem::rename(".context/amiga/wfw311/screen.tmp",".context/amiga/wfw311/screen.ppm");}
  if(std::filesystem::exists(".context/amiga/wfw311/stop"))limit_ms=PIC_Ticks;
 }
 if(PIC_Ticks%20==0&&!getenv("ZZDOS_FAST"))std::this_thread::sleep_until(interactive_start+std::chrono::milliseconds(PIC_Ticks));
}
