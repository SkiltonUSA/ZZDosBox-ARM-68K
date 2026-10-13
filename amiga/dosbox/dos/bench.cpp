/* GPL-2.0-or-later. Real DOS kernel, programs supplied by the 68k host. */
#include <stdint.h>
#include <string.h>
#include "adapter.h"
#include "bench.h"
#include "files.h"
#include "dos_wire.h"
extern "C" void db_runtime_init();
extern "C" void db_output(const char *);
extern "C" volatile uint8_t *db_shared();
extern "C" int mesen_test(int verify,MesenResult *out,int (*stop)(),uint32_t (*clock)()){
 db_runtime_init();memset(out,0,sizeof(*out));DosboxResult result{};
 volatile uint8_t *base=db_shared();
 unsigned bytes=ad_get(base+MB_HOST,DB_PROGRAM_SIZE),input=ad_get(base+MB_HOST,DB_INPUT_SIZE);
 if(!bytes||bytes>0xff00||input>DB_CAPACITY)return 4;
 db_files_reset();if(!db_file_put("INPUT.TXT",(const uint8_t *)(base+DB_INPUT),input,1))return 4;
 uint32_t start=clock();int rc=db_run((const uint8_t *)(base+DB_PROGRAM),bytes,1000000,&result,stop);
 out->ticks=clock()-start;db_output(result.text);out->checked=verify?1:0;
 out->cycles=result.slices;out->errors=result.fault;out->actual_pc=result.exit_code;out->expected_pc=result.bytes;
 const uint8_t *data;unsigned size;
 if(db_file_get("OUTPUT.TXT",&data,&size)){
  if(size>DB_CAPACITY)return 4;
  uint32_t hash=2166136261u;for(unsigned i=0;i<size;i++){base[DB_OUTPUT+i]=data[i];hash=(hash^data[i])*16777619u;}
  out->actual_regs=size;out->hash=hash;
 }
 return rc;
}
