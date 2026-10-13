#include <string>
#include <stdio.h>
#include <assert.h>
#include "console_io.h"
#include "files.h"
#include "filetest.h"
#include "exetest.h"
static std::string input,output;static unsigned pos;
static int read(void*){return pos<input.size()?(unsigned char)input[pos++]:-2;}
static int ready(void*){return pos<input.size();}
static int write(void*,const uint8_t *p,unsigned n){output.append((const char*)p,n);return 1;}
int main(){
 db_files_reset();uint8_t data[256];for(unsigned i=0;i<256;i++)data[i]=i*13+7;
 assert(db_file_put("INPUT.TXT",data,256,1));assert(db_file_put("FILETEST.COM",filetest,sizeof(filetest),1));assert(db_file_put("EXETEST.EXE",exetest,sizeof(exetest),1));
 const char text[]="Text from the DOS C drive\r\n";assert(db_file_put("README.TXT",(const uint8_t*)text,sizeof(text)-1,1));
 input="dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rexit\r";pos=0;output.clear();
 DbConsoleIO io={nullptr,read,ready,write};DosboxResult r{};int rc=db_run_io(nullptr,0,20000,&r,nullptr,&io);
 printf("rc=%d fault=%u detail=%s\n%s\n",rc,r.fault,r.text,output.c_str());fflush(stdout);
 assert(rc==0&&!r.fault);assert(output.find("ZZDOS files/handles/seek PASS")!=std::string::npos);assert(output.find("ZZDOS MZ relocation/memory PASS")!=std::string::npos);
 const uint8_t *p;unsigned n;assert(db_file_get("SAVED.TXT",&p,&n)&&std::string((const char*)p,n)=="saved text\r\n");
 puts("PASS upstream shell: DIR, TYPE, redirection, COM and EXE EXEC, return to prompt, EXIT");
}
