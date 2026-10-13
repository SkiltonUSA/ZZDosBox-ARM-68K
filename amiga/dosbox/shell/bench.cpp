/* GPL-2.0-or-later. Real DOS kernel, programs supplied by the 68k host. */
#include <stdint.h>
#include <string.h>
#include "adapter.h"
#include "console_io.h"
#include "stream.h"
extern "C" const DbConsoleIO *db_stream(int (*)());
#include "bench.h"
#include "files.h"
#include "dos_wire.h"
extern "C" void db_runtime_init();
extern "C" void db_output(const char *);
extern "C" volatile uint8_t *db_shared();
extern "C" int mesen_test(int verify,MesenResult *out,int (*stop)(),uint32_t (*clock)()){
 db_runtime_init();memset(out,0,sizeof(*out));DosboxResult result{};
 uint32_t start=clock();int rc=db_run_io(nullptr,0,1000000,&result,stop,db_stream(stop));
 out->ticks=clock()-start;db_output(result.text);out->checked=verify?1:0;
 out->cycles=result.slices;out->errors=result.fault;out->actual_pc=result.exit_code;out->expected_pc=result.bytes;
 return rc;
}
