/* GPL-2.0-or-later. Real DOS console API and callback lifecycle tests. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "console_io.h"
#include "files.h"
#include "io_test.h"
#include "echo.h"
#include "filetest.h"
struct State{std::string input,output;unsigned at=0;bool start=false,cancel=false;};
static State s;
static int read(void*){if(!s.start||s.at==s.input.size())return -1;return (uint8_t)s.input[s.at++];}
static int ready(void*){return s.start&&s.at<s.input.size();}
static int write(void*,const uint8_t *p,unsigned n){s.output.append((const char*)p,n);if(s.output.find("READY\r\n")!=std::string::npos)s.start=true;return 1;}
static unsigned stop_count;
static int stop_later(){return ++stop_count>10;}
static int stop(){return s.cancel&&s.output.size()>0;}
static DbConsoleIO io={nullptr,read,ready,write};
int main(){
 for(unsigned repeat=0;repeat<3;repeat++){
  s={"ZQAB\bXC\r",""};db_files_reset();DosboxResult r{};
  int rc=db_run_io(io_test,sizeof(io_test),10000,&r,stop,&io);printf("rc=%d exit=%u fault=%u bytes=%zu detail=%.159s\n",rc,r.exit_code,r.fault,s.output.size(),r.text);fflush(stdout);assert(rc==0&&r.exit_code==0&&!r.fault);
  std::string expected="READY\r\nQAB\b \bXC\r";expected+=std::string(5000,'~');expected+="\r\nCONSOLE PASS\r\n";
  assert(s.output==expected&&s.at==s.input.size()&&r.bytes==expected.size());
  s={"hello\rexit\r","",0,true,false};db_files_reset();
  assert(db_run_io(echo,sizeof(echo),100,&r,stop,&io)==0);
  assert(s.output.find("You typed: hello\r\n")!=std::string::npos&&s.output.find("Goodbye.")!=std::string::npos);
  s={"","",0,false,true};db_files_reset();
  assert(db_run_io(echo,sizeof(echo),100,&r,stop,&io)==2&&r.fault==3);
 }
 // Cancel AH=01h while it has DOS echo enabled, then start a no-echo run.
 const uint8_t blocking[]={0xb4,1,0xcd,0x21,0xb8,0,0x4c,0xcd,0x21};
 s={};db_files_reset();DosboxResult cancelled{};stop_count=0;
 assert(db_run_io(blocking,sizeof(blocking),100,&cancelled,stop_later,&io)==2);
 s={"ZQAB\bXC\r",""};db_files_reset();
 assert(db_run_io(io_test,sizeof(io_test),10000,&cancelled,stop,&io)==0);
 assert(s.output.find("READY\r\nQAB") == 0);
 // No streaming callback remains after teardown. Old bounded API still works.
 db_files_reset();uint8_t input[256];for(unsigned i=0;i<256;i++)input[i]=i*13+7;
 assert(db_file_put("INPUT.TXT",input,256,1));DosboxResult r{};
 assert(db_run(filetest,sizeof(filetest),100,&r,nullptr)==0);
 assert(!strcmp(r.text,"ZZDOS files/handles/seek PASS\r\n"));
 puts("PASS native console: status, nonblocking read, echo/no-echo, line editing, 5032-byte output, repeat, input-wait cancellation, legacy API");
}
