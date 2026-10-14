/* GPL-3.0-or-later. Automated physical integration check; owned test guest only. */
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>
#include <string.h>
#include "memory_ipc.h"
static struct dm_message packet;
static struct MsgPort *reply;
static int call(unsigned op,uint32_t addr,unsigned len,unsigned width,uint32_t value,unsigned compare,uint32_t expected,unsigned error){
 struct MsgPort *server;
 memset(&packet,0,sizeof(packet));packet.magic=DM_MAGIC;packet.msg.mn_Length=sizeof(packet);packet.msg.mn_ReplyPort=reply;
 packet.request=(struct dm_request){op,addr,len,width,value,compare,0,expected};
 Forbid();server=FindPort(DM_PORT);if(server)PutMsg(server,&packet.msg);Permit();
 if(!server){puts("FAIL: guest memory server unavailable");return 0;}
 WaitPort(reply);if(GetMsg(reply)!=&packet.msg)return 0;
 printf("MEM op=%u error=%lu paused=%lu count=%lu\n",op,(unsigned long)packet.reply.error,(unsigned long)packet.reply.paused,(unsigned long)packet.reply.count);fflush(stdout);
 return packet.reply.error==error;
}
int main(int argc,char **argv){
 int ok=1,patch=argc>1&&!strcmp(argv[1],"patch");uint32_t address=0;
 reply=CreateMsgPort();if(!reply)return 20;
 if(patch){
  unsigned attempt;for(attempt=0;attempt<100;attempt++){
   if(!call(DM_READ,0x500,8,0,0,0,0,DM_OK)){ok=0;break;}
   if(!memcmp(packet.reply.bytes+4,"MEM1",4)){
    address=((uint32_t)packet.reply.bytes[0]|((uint32_t)packet.reply.bytes[1]<<8))*16u+packet.reply.bytes[2]+((uint32_t)packet.reply.bytes[3]<<8);break;
   }
   Delay(2);
  }
  if(!address)ok=0;
  if(ok){Delay(25);ok=call(DM_PAUSE,0,0,0,0,0,0,DM_OK)&&call(DM_WRITE,address,0,2,0x2222,0,0x1111,DM_OK)&&call(DM_RESUME,0,0,0,0,0,0,DM_OK);}
 }else{
#define CHECK(...) do{if(ok)ok=call(__VA_ARGS__);}while(0)
  CHECK(DM_STATUS,0,0,0,0,0,0,DM_OK);
  CHECK(DM_WRITE,0x180001,0,4,0x12345678,0,0,DM_RUNNING);
  CHECK(DM_PAUSE,0,0,0,0,0,0,DM_OK);
  CHECK(DM_WRITE,0x180001,0,4,0x12345678,0,0,DM_OK);
  CHECK(DM_READ,0x180001,4,0,0,0,0,DM_OK);if(ok)ok=!memcmp(packet.reply.bytes,"\x78\x56\x34\x12",4);
  CHECK(DM_SCAN,0x180000,16,4,0x12345678,DM_EQUAL,0,DM_OK);if(ok)ok=packet.reply.count==1;
  CHECK(DM_WRITE,0x180001,0,2,0x5679,0,0x5678,DM_OK);
  CHECK(DM_REFINE,0,0,0,0,DM_CHANGED,0,DM_OK);if(ok)ok=packet.reply.count==1;
  CHECK(DM_RESULTS,0,0,0,0,0,0,DM_OK);if(ok)ok=!memcmp(packet.reply.bytes,"\0\x18\0\1\x12\x34\x56\x79",8);
  CHECK(DM_READ,0x9ffff,2,0,0,0,0,DM_RANGE);
  CHECK(DM_WRITE,0x180001,0,4,0,0,0,DM_BAD_REQUEST);
  CHECK(DM_WRITE,0x180001,0,4,0,0,0x12345679,DM_OK);
  CHECK(DM_RESUME,0,0,0,0,0,0,DM_OK);
  CHECK(DM_STOP,0,0,0,0,0,0,DM_OK);
 }
 if(!ok)call(DM_STOP,0,0,0,0,0,0,DM_OK);
 puts(ok?(patch?"MEMORY EXTERNAL PATCH REQUEST PASS":"MEMORY PROTOCOL PASS"):"MEMORY PROTOCOL FAIL");DeleteMsgPort(reply);return ok?0:20;
}
