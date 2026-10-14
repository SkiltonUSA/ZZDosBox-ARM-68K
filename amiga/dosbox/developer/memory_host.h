/* Included by host.c; host stays responsive while ARM handles a request. */
#include "memory_ipc.h"
static struct MsgPort *memory_port;
static struct dm_message *memory_pending;
static ULONG memory_next;
static int memory_stop_requested;
static void memory_host_close(void){
 struct dm_message *m;
 if(!memory_port)return;
 Forbid();RemPort(memory_port);Permit();
 if(memory_pending){memory_pending->reply.error=DM_OFFLINE;ReplyMsg(&memory_pending->msg);memory_pending=NULL;}
 while((m=(struct dm_message*)GetMsg(memory_port))){if(m->msg.mn_Length==sizeof(*m))m->reply.error=DM_OFFLINE;ReplyMsg(&m->msg);}
 DeleteMsgPort(memory_port);memory_port=NULL;
}
static int memory_host_open(void){
 Forbid();if(FindPort(DM_PORT)){Permit();return 0;}
 memory_port=CreateMsgPort();if(memory_port){memory_port->mp_Node.ln_Name=DM_PORT;AddPort(memory_port);}Permit();
 memory_next=0;memory_pending=NULL;memory_stop_requested=0;publish(DM_SEQUENCE,0);publish(DM_ACK,0);return memory_port!=NULL;
}
static void memory_host_service(int stopping){
 struct dm_message *m;unsigned i;
 if(!memory_port)return;
 if(stopping){memory_host_close();return;}
 if(memory_pending&&receive(DM_ACK)==memory_next){
  ULONG *words=(ULONG*)&memory_pending->reply;
  syncio->pull(shared+DM_RESPONSE,sizeof(struct dm_reply),NULL);
  for(i=0;i<8;i++)words[i]=ad_get(shared,DM_RESPONSE+i*4);
  for(i=0;i<DM_BYTES;i++)memory_pending->reply.bytes[i]=shared[DM_RESPONSE+32+i];
  if(memory_pending->reply.paused&&!pc_display_locked()&&!pc_display_pending())pc_display_console(1);
  ReplyMsg(&memory_pending->msg);memory_pending=NULL;
 }
 if(memory_pending)return;
 m=(struct dm_message*)GetMsg(memory_port);if(!m)return;
 if(m->msg.mn_Length!=sizeof(*m)){ReplyMsg(&m->msg);return;}
 if(m->magic!=DM_MAGIC){m->reply.error=DM_BAD_REQUEST;ReplyMsg(&m->msg);return;}
 if(m->request.op==DM_STOP){memory_stop_requested=1;memset(&m->reply,0,sizeof(m->reply));ReplyMsg(&m->msg);return;}
 memory_pending=m;
 for(i=0;i<8;i++)ad_put(shared,DM_COMMAND+i*4,((ULONG*)&m->request)[i]);
 syncio->push(shared+DM_COMMAND,sizeof(m->request),NULL);
 if(!++memory_next)++memory_next;
 publish(DM_SEQUENCE,memory_next);
}
