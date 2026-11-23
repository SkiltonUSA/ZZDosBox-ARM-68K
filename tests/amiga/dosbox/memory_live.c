#include <proto/exec.h>
#include <stdio.h>
#include <string.h>
#include "memory_ipc.h"
static struct dm_message packet;
static struct MsgPort *reply;
static int call(unsigned op,unsigned address,unsigned length){
 struct MsgPort *server;
 memset(&packet,0,sizeof packet);packet.msg.mn_Length=sizeof packet;
 packet.msg.mn_ReplyPort=reply;packet.magic=DM_MAGIC;
 packet.request.op=op;packet.request.address=address;packet.request.length=length;
 Forbid();server=FindPort(DM_PORT);if(server)PutMsg(server,&packet.msg);Permit();
 if(!server)return 0;
 WaitPort(reply);if(GetMsg(reply)!=&packet.msg)return 0;
 printf("MEMORY op=%u error=%u paused=%u ram=%u count=%u\n",op,
 (ULONG)packet.reply.error,(ULONG)packet.reply.paused,(ULONG)packet.reply.ram_bytes,(ULONG)packet.reply.count);
 return 1;
}
int main(void){
 int ok=1;unsigned char first[32];reply=CreateMsgPort();if(!reply)return 20;
 ok=call(DM_STATUS,0,0)&&packet.reply.error==DM_OK&&packet.reply.ram_bytes==8388608;
 if(ok)ok=call(DM_PAUSE,0,0)&&packet.reply.error==DM_OK&&packet.reply.paused;
 if(ok)ok=call(DM_READ,0x400,32)&&packet.reply.error==DM_OK&&packet.reply.count==32;
 if(ok){memcpy(first,packet.reply.bytes,32);ok=call(DM_READ,0x400,32)&&packet.reply.error==DM_OK&&!memcmp(first,packet.reply.bytes,32);}
 if(ok)ok=call(DM_READ,0xfffffff0u,256)&&packet.reply.error==DM_RANGE;
 /* Always attempt resume after a protocol failure, before ending the test. */
 if(!call(DM_RESUME,0,0)||packet.reply.error!=DM_OK||packet.reply.paused)ok=0;
 DeleteMsgPort(reply);puts(ok?"MEMORY CHECK PASS":"MEMORY CHECK FAIL");return ok?0:20;
}
