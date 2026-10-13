#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "adapter.h"
#include "files.h"
#include "fixture.h"
#include "filetest.h"
#include "exetest.h"
static void test(const uint8_t *p,unsigned n,const char *expected){
 db_files_reset();uint8_t input[256];for(unsigned i=0;i<256;i++)input[i]=(i*13+7)&255;
 assert(db_file_put("INPUT.TXT",input,sizeof(input),1));
 DosboxResult r{};int rc=db_run(p,n,100,&r,nullptr);printf("rc=%d exit=%u fault=%u text=%s\n",rc,r.exit_code,r.fault,r.text);fflush(stdout);assert(!rc&&!strcmp(expected,r.text));
 if(p==filetest){const uint8_t *out;unsigned size;assert(db_file_get("OUTPUT.TXT",&out,&size)&&size==256&&!memcmp(out,input,256));}
}
int main(){for(unsigned repeat=0;repeat<3;repeat++){
 test(db_fixture,sizeof(db_fixture),"ZZDOS x86 arithmetic/memory PASS\r\n");
 test(filetest,sizeof(filetest),"ZZDOS files/handles/seek PASS\r\n");
 test(exetest,sizeof(exetest),"ZZDOS MZ relocation/memory PASS\r\n");
}
 db_files_reset();uint8_t byte=1;
 assert(!db_file_put("../BAD",&byte,1,0));assert(!db_file_put("SYS:BAD",&byte,1,0));assert(!db_file_put("BAD.",&byte,1,0));assert(!db_file_put("123456789.TXT",&byte,1,0));
 assert(!db_file_put("BIG.BIN",&byte,65537,0));
 assert(db_file_put("INPUT.TXT",&byte,1,1));DosboxResult r{};
 assert(db_run(filetest,sizeof(filetest),100,&r,nullptr)==1&&r.exit_code==1&&!r.fault);
 puts("PASS real DOS kernel: COM and MZ EXEC, relocation, allocation, file read/write/reopen/seek/EOF, missing files, repeated teardown");}
