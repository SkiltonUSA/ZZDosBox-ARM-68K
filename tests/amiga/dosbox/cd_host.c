#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "cd_host.h"
int main(int argc,char**argv){unsigned char b[4096];assert(argc==2);assert(db_cd_host_open(1,argv[1])==32);assert(db_cd_host_read(16,1,b)&&!memcmp(b+1,"CD001",5));assert(db_cd_host_read(24,2,b));for(unsigned i=0;i<4096;i++)assert(b[i]==i%251);assert(!db_cd_host_read(32,1,b)&&!db_cd_host_read(31,2,b)&&!db_cd_host_read(0,0,b)&&!db_cd_host_read(0,3,b));db_cd_host_close();assert(!db_cd_host_read(16,1,b));assert(!db_cd_host_open(1,"/nonexistent/zzcd.iso"));assert(!db_cd_host_open(2,argv[1]));puts("PASS read-only CD bounds and missing/invalid sources");}
