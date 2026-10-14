#include <assert.h>
#include "../../../amiga/dosbox/launcher/developer.h"
int main(void){
 char text[192];unsigned frame=0;
 assert(dl_dev_frame("1",&frame)&&frame==1);
 assert(dl_dev_frame("10000000",&frame)&&frame==10000000);
 const char *bad[]={"","0","-1","+1","1;Execute bad","1\n","10000001","4294967297","999999999999999999999999"};
 for(unsigned i=0;i<sizeof(bad)/sizeof(*bad);i++)assert(!dl_dev_frame(bad[i],&frame));
 for(unsigned trace=0;trace<2;trace++)for(unsigned point=0;point<3;point++)
 for(unsigned profile=0;profile<2;profile++)for(unsigned handoff=0;handoff<3;handoff++)for(unsigned poll=0;poll<4;poll++){
  assert(dl_dev_tools(text,sizeof(text),trace,point,"123",profile,handoff,poll));
  assert(!!strstr(text,"--trace-video")==trace);
  assert(!!strstr(text,"--profile")==profile);
  assert(!!strstr(text,"--break-before 123")== (point==1));
  assert(!!strstr(text,"--break-after 123")== (point==2));
  assert(!!strstr(text,"--serial-video")== (handoff==1));
  assert(!!strstr(text,"--pipeline-video")== (handoff==2));
  assert(!!strstr(text,"--poll-ms")== !!poll);
  assert(!strcmp(dl_dev_runtime("ZZDosBox-OpenGL",trace,point),trace||point?"ZZDosBox-OpenGL-Trace":"ZZDosBox-OpenGL"));
  assert(!strcmp(dl_dev_runtime("ZZDosBox-Adaptive",trace,point),trace||point?"ZZDosBox-Trace":"ZZDosBox-Adaptive"));
 }
 assert(dl_dev_tools(text,sizeof(text),0,0,"",0,0,0)&&!*text);
 assert(!dl_dev_tools(text,2,1,1,"1",1,2,3));
 assert(!dl_dev_tools(text,sizeof(text),2,0,"1",0,0,0));
 assert(!dl_dev_tools(text,sizeof(text),0,3,"1",0,0,0));
 assert(!dl_dev_tools(text,sizeof(text),0,0,"1",2,0,0));
 assert(!dl_dev_tools(text,sizeof(text),0,0,"1",0,3,0));
 assert(!dl_dev_tools(text,sizeof(text),0,0,"1",0,0,4));
 puts("PASS Developer defaults, all control combinations, dispatch and input bounds");
}
