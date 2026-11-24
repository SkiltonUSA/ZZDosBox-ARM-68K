/* GPL-2.0-or-later. Resume the real interpreter at instruction boundaries. */
#include <assert.h>
#include <vector>
#include "../../../amiga/dosbox/adapter.cpp"
#include "state/cpu_checkpoint.h"
static const uint8_t program[]={
 0xb8,0xff,0xff,0x05,1,0,0x83,0xd3,0,0x9c,0x5a,
 0xbe,0,0x80,0x89,0x1c,0x46,0xb9,32,0,
 0xd1,0xc0,0x83,0xd3,7,0x88,0x1c,0x46,0xe2,0xf6,
 0xfd,0xbf,0,0x81,0xb9,16,0,0xf3,0xaa,0xfc,0xeb,0xfe
};
static void initialise(){
 memset(ram,0,sizeof(ram));memset(&cpu_regs,0,sizeof(cpu_regs));
 memset(&cpu,0,sizeof(cpu));memset(&Segs,0,sizeof(Segs));memset(&lflags,0,sizeof(lflags));
 zz_dosbox_paging_init();CPU_ArchitectureType=CPU_ARCHTYPE_386SLOW;
 CPU_Core_Normal_Init();cpudecoder=CPU_Core_Normal_Run;
 for(unsigned seg=0;seg<6;seg++)SegSet16(seg,0x1000);
 cpu.stack.mask=0xffff;cpu.stack.notmask=0xffff0000;
 cpu.idt.SetBase(0);cpu.idt.SetLimit(1023);CPU_SetFlags(FLAG_IF,FMASK_ALL);
 reg_sp=0xfffe;reg_eip=0x100;CPU_CycleLeft=0;CPU_Cycles=0;
 memcpy(ram+0x10100,program,sizeof(program));
}
static void step(unsigned count){
 for(unsigned n=0;n<count;n++){CPU_Cycles=1;assert(CPU_Core_Normal_Run()==0);}
}
static std::vector<uint8_t> checkpoint(){
 std::vector<uint8_t> image(db_cpu_checkpoint_bytes);
 assert(db_cpu_checkpoint_save(image.data(),image.size()));return image;
}
// Recalculate the wire CRC so malformed-field tests reach semantic validation.
static void set_word(std::vector<uint8_t> &image,unsigned word,uint32_t value){
 for(unsigned b=0;b<4;b++)image[word*4+b]=(uint8_t)(value>>(8*b));
}
static void repair_crc(std::vector<uint8_t> &image){
 uint32_t crc=~0u;
 for(unsigned i=0;i<image.size();i++)if(i<12||i>=16){
  crc^=image[i];for(unsigned b=0;b<8;b++)crc=(crc>>1)^((crc&1)?0xedb88320u:0);
 }
 set_word(image,3,~crc);
}
static void malformed_fields(){
 auto before=checkpoint();
 const uint32_t fields[][2]={
  {15,0x10000},{22,0x10000},{34,0xffffffff},{35,0xffffffff},{36,2},
  {37,4},{38,4},{39,1},{39,0x80000000},{41,0x10000},{43,0x10000},
  {44,0xffffffff},{45,0},{46,1},{47,1},{50,0},{50,2},{50,0xffffffff},{51,1}
 };
 for(auto &field:fields){
  auto bad=before;set_word(bad,field[0],field[1]);repair_crc(bad);
  if(db_cpu_checkpoint_load(bad.data(),bad.size())){
   fprintf(stderr,"accepted invalid CPU checkpoint word %u = %08x\n",field[0],field[1]);abort();
  }
  assert(checkpoint()==before);
 }
 // DF and direction are a pair, not independent values.
 CPU_SetFlags(FLAG_IF|FLAG_DF,FMASK_ALL);auto reverse=checkpoint();
 assert(db_cpu_checkpoint_load(reverse.data(),reverse.size()));
 auto bad=reverse;set_word(bad,50,1);repair_crc(bad);
 assert(!db_cpu_checkpoint_load(bad.data(),bad.size()));assert(checkpoint()==reverse);
 assert(db_cpu_checkpoint_load(before.data(),before.size()));
 puts("PASS CPU checkpoint: 20 valid-CRC malformed records rejected atomically; reverse string direction restored");
}
static void disk_resume(const char *mode,const char *path){
 initialise();step(240);auto expected=checkpoint();
 std::vector<uint8_t> expected_ram(ram,ram+sizeof(ram));
 if(!strcmp(mode,"save")){
  initialise();step(47);auto saved=checkpoint();FILE *f=fopen(path,"wb");assert(f);
  assert(fwrite(saved.data(),1,saved.size(),f)==saved.size());
  assert(fwrite(ram,1,sizeof(ram),f)==sizeof(ram));assert(fclose(f)==0);
 }else{
  initialise();std::vector<uint8_t> saved(db_cpu_checkpoint_bytes);
  FILE *f=fopen(path,"rb");assert(f);
  assert(fread(saved.data(),1,saved.size(),f)==saved.size());
  assert(fread(ram,1,sizeof(ram),f)==sizeof(ram));assert(fgetc(f)==EOF);assert(fclose(f)==0);
  assert(db_cpu_checkpoint_load(saved.data(),saved.size()));step(240-47);
  assert(checkpoint()==expected&&memcmp(ram,expected_ram.data(),sizeof(ram))==0);
  puts("PASS CPU checkpoint and test RAM resumed in a fresh process");
 }
}
int main(int argc,char **argv){
 if(argc==3){disk_resume(argv[1],argv[2]);return 0;}
 initialise();step(240);auto expected=checkpoint();
 std::vector<uint8_t> expected_ram(ram,ram+sizeof(ram));
 assert(ram[0x18000]==1&&ram[0x18001]==8&&ram[0x18020]==225);
 for(unsigned at=0;at<200;at++){
  initialise();step(at);auto saved=checkpoint();std::vector<uint8_t> memory(ram,ram+sizeof(ram));
  step(240-at);assert(checkpoint()==expected&&memcmp(ram,expected_ram.data(),sizeof(ram))==0);
  initialise();memcpy(ram,memory.data(),sizeof(ram));
  assert(db_cpu_checkpoint_load(saved.data(),saved.size()));step(240-at);
  assert(checkpoint()==expected&&memcmp(ram,expected_ram.data(),sizeof(ram))==0);
 }
 initialise();step(4);auto saved=checkpoint();auto before=checkpoint();
 malformed_fields();
 for(size_t n=0;n<saved.size();n++){
  auto corrupt=saved;corrupt[n]^=0x80;
  assert(!db_cpu_checkpoint_load(corrupt.data(),corrupt.size()));assert(checkpoint()==before);
 }
 for(size_t n=0;n<saved.size();n++)assert(!db_cpu_checkpoint_load(saved.data(),n));
 auto extra=saved;extra.push_back(0);assert(!db_cpu_checkpoint_load(extra.data(),extra.size()));
 assert(!db_cpu_checkpoint_save(extra.data(),extra.size()));
 CPU_ArchitectureType=CPU_ARCHTYPE_486NEWSLOW;assert(!db_cpu_checkpoint_load(saved.data(),saved.size()));
 CPU_ArchitectureType=CPU_ARCHTYPE_386SLOW;
 cpu.pmode=true;assert(!db_cpu_checkpoint_save(saved.data(),saved.size()));cpu.pmode=false;
 paging.enabled=true;assert(!db_cpu_checkpoint_save(saved.data(),saved.size()));paging.enabled=false;
 cpudecoder=CPU_Core_Normal_Trap_Run;assert(!db_cpu_checkpoint_save(saved.data(),saved.size()));cpudecoder=CPU_Core_Normal_Run;
 assert(checkpoint()==before);
 puts("PASS real CPU checkpoint: 200 resume points, pending flags, stack, REP, RAM continuation; corrupt/truncated/mismatched/unsupported states rejected without mutation");
}
