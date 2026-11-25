/* GPL-2.0-or-later. Test actual x86 instruction continuation, not just fields. */
#include <assert.h>
#include <vector>
#include "state_fpu_fixture.h"
using Bytes=std::vector<uint8_t>;
static Bytes fp(){Bytes x(db_fpu_checkpoint_bytes);assert(db_fpu_checkpoint_save(x.data(),x.size()));return x;}
static Bytes cpu_image(){Bytes x(db_cpu_checkpoint_bytes);assert(db_cpu_checkpoint_save(x.data(),x.size()));return x;}
static Bytes processor(){Bytes x(db_processor_checkpoint_bytes);assert(db_processor_checkpoint_save(x.data(),x.size()));return x;}
static void results(){
 const int32_t expected[]={2,-2,1,-3,2,-2,1,-2};
 for(unsigned n=0;n<8;n++)assert((int32_t)mem_readd(0x18100+n*4)==expected[n]);
}
static void put(Bytes &x,unsigned word,uint32_t v){for(unsigned b=0;b<4;b++)x[word*4+b]=(uint8_t)(v>>(b*8));}
static void crc(Bytes &x){
 uint32_t c=~0u;for(unsigned n=0;n<x.size();n++)if(n<12||n>=16){c^=x[n];for(unsigned b=0;b<8;b++)c=(c>>1)^((c&1)?0xedb88320u:0);}
 put(x,3,~c);
}
static void rejected(Bytes bad,const Bytes &before){
 assert(!db_fpu_checkpoint_load(bad.data(),bad.size()));assert(fp()==before);
}
static void boundaries(){
 initialise();step(300);results();auto expected=fp(),expected_cpu=cpu_image();Bytes memory(ram,ram+sizeof(ram));
 for(unsigned at=0;at<200;at++){
  initialise();step(at);auto saved=processor();Bytes saved_ram(ram,ram+sizeof(ram));
  initialise();memcpy(ram,saved_ram.data(),sizeof(ram));
  assert(db_processor_checkpoint_load(saved.data(),saved.size()));step(300-at);
  assert(fp()==expected&&cpu_image()==expected_cpu&&!memcmp(ram,memory.data(),sizeof(ram)));results();
 }
 puts("PASS x87 checkpoint: 200 interpreter continuations, four rounding modes, FXCH/arithmetic/sqrt, FSAVE/FRSTOR and CPU/RAM parity");
}
static void processor_transaction(){
 initialise();step(5);Bytes saved(db_processor_checkpoint_bytes);
 assert(db_processor_checkpoint_save(saved.data(),saved.size()));
 step(10);auto before_cpu=cpu_image(),before_fpu=fp();
 for(unsigned n=0;n<saved.size();n++){
  auto bad=saved;bad[n]^=0x80;
  assert(!db_processor_checkpoint_load(bad.data(),bad.size()));
  assert(cpu_image()==before_cpu&&fp()==before_fpu);
 }
 for(unsigned n=0;n<saved.size();n++)assert(!db_processor_checkpoint_load(saved.data(),n));
 auto extra=saved;extra.push_back(0);assert(!db_processor_checkpoint_load(extra.data(),extra.size()));
 assert(!db_processor_checkpoint_load(nullptr,saved.size()));
 for(unsigned offset:{0u,unsigned(db_cpu_checkpoint_bytes)}){
  Bytes component(saved.begin()+offset,saved.begin()+offset+(offset?db_fpu_checkpoint_bytes:db_cpu_checkpoint_bytes));
  put(component,offset?7:44,0xffffffffu);crc(component);
  auto bad=saved;memcpy(bad.data()+offset,component.data(),component.size());
  assert(!db_processor_checkpoint_load(bad.data(),bad.size()));
  assert(cpu_image()==before_cpu&&fp()==before_fpu);
 }
 Bytes destination(saved.size(),0xa5),untouched=destination;
 fpu.top=9;
 assert(!db_processor_checkpoint_save(destination.data(),destination.size()));assert(destination==untouched);
 assert(db_fpu_checkpoint_load(before_fpu.data(),before_fpu.size()));
 cpu.pmode=true;assert(!db_processor_checkpoint_load(saved.data(),saved.size()));cpu.pmode=false;
 assert(cpu_image()==before_cpu&&fp()==before_fpu);
 assert(db_processor_checkpoint_load(saved.data(),saved.size()));
 Bytes after(saved.size());assert(db_processor_checkpoint_save(after.data(),after.size()));assert(after==saved);
 puts("PASS processor transaction: corrupt CPU or x87 leaves both unchanged; complete restoration");
}
static void bits_and_failures(){
 initialise();
 // Raw payloads, including signed zero, subnormal, infinity and NaN, must not
 // be converted through decimal or floating-point arithmetic by the serializer.
 const uint64_t bits[]={0,0x8000000000000000ull,1,0x7ff0000000000000ull,
  0xfff0000000000000ull,0x7ff8000000001234ull,0x7ff0000000004321ull,
  0x3ff8000000000000ull,0xbff8000000000000ull};
 for(unsigned n=0;n<9;n++){memcpy(&fpu.regs[n],&bits[n],8);fpu.tags[n]=FPU_Tag(n%4);}
 fpu.top=5;fpu.sw=0xa541;FPU_SetCW(0x0b7f);auto saved=fp();
 initialise();assert(db_fpu_checkpoint_load(saved.data(),saved.size()));assert(fp()==saved);
 assert(fpu.top==5&&fpu.sw==0xa541&&fpu.round==ROUND_Up&&fpu.cw_mask_all==(fpu.cw|0x3f));
 for(unsigned n=0;n<9;n++)assert(!memcmp(&fpu.regs[n],&bits[n],8));
 for(unsigned n=0;n<saved.size();n++){auto bad=saved;bad[n]^=0x80;rejected(bad,saved);}
 for(unsigned n=0;n<saved.size();n++)assert(!db_fpu_checkpoint_load(saved.data(),n));
 auto extra=saved;extra.push_back(0);rejected(extra,saved);
 const unsigned fields[]={0,1,2,4,5,6,7,10,13,16,19,22,25,28,31,34};
 for(unsigned word:fields){auto bad=saved;put(bad,word,0xffffffff);crc(bad);rejected(bad,saved);}
 assert(!db_fpu_checkpoint_load(nullptr,saved.size()));assert(!db_fpu_checkpoint_save(nullptr,saved.size()));
 assert(!db_fpu_checkpoint_save(extra.data(),extra.size()));assert(fp()==saved);
 puts("PASS x87 checkpoint: bit-exact nine registers, independent TOP/status, derived rounding, all-byte corruption/length and 16 valid-CRC malformed records rejected atomically");
}
static void disk(const char *mode,const char *path){
 initialise();step(300);auto expected=fp(),expected_cpu=cpu_image();Bytes memory(ram,ram+sizeof(ram));
 if(!strcmp(mode,"save")){
  initialise();step(5);auto c=cpu_image(),f=fp();FILE *file=fopen(path,"wb");assert(file);
  for(auto *x:{&c,&f})assert(fwrite(x->data(),1,x->size(),file)==x->size());
  assert(fwrite(ram,1,sizeof(ram),file)==sizeof(ram));assert(fclose(file)==0);
 }else{
  initialise();Bytes c(db_cpu_checkpoint_bytes),f(db_fpu_checkpoint_bytes);FILE *file=fopen(path,"rb");assert(file);
  for(auto *x:{&c,&f})assert(fread(x->data(),1,x->size(),file)==x->size());
  assert(fread(ram,1,sizeof(ram),file)==sizeof(ram));assert(fgetc(file)==EOF);assert(fclose(file)==0);
  c.insert(c.end(),f.begin(),f.end());assert(db_processor_checkpoint_load(c.data(),c.size()));step(295);
  assert(fp()==expected&&cpu_image()==expected_cpu&&!memcmp(ram,memory.data(),sizeof(ram)));results();
  puts("PASS x87 checkpoint: CPU/FPU/test RAM fresh-process continuation");
 }
}
int main(int argc,char **argv){
 if(argc==3){disk(argv[1],argv[2]);return 0;}
 boundaries();bits_and_failures();processor_transaction();
}
