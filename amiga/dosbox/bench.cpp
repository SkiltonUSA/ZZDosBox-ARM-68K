/* GPL-2.0-or-later. CPU-only x86 correctness diagnostic, not game FPS. */
#include <stdint.h>
#include <string.h>
#include "adapter.h"
#include "bench.h"
#include "fixture.h"
extern "C" void db_runtime_init();
extern "C" void db_output(const char *);
extern "C" int mesen_test(int verify,MesenResult *out,int (*stop)(),uint32_t (*clock)()){
 db_runtime_init();memset(out,0,sizeof(*out));DosboxResult result{};
 uint32_t start=clock();int rc=db_run(db_fixture,sizeof(db_fixture),100,&result,stop);
 out->ticks=clock()-start;db_output(result.text);out->checked=verify?1:0;
 out->cycles=result.slices;out->hash=result.hash;out->errors=result.fault;
 out->actual_pc=result.exit_code;out->expected_pc=result.bytes;
 if(rc)return rc;
 if(result.hash!=0xf23411c5u||result.exit_code||strcmp(result.text,"ZZDOS x86 arithmetic/memory PASS\r\n")){
  out->mismatch=1;return 1;
 }
 return 0;
}
