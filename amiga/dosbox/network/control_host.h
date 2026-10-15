/* GPL-3.0-or-later. Bounded IPC, polled by the runtime's owning host task. */
#include "control.h"
static struct MsgPort *dn_port;
static int dn_control_open(void){
 if(FindPort(DN_PORT))return 0;
 dn_port=CreateMsgPort();if(!dn_port)return 0;
 dn_port->mp_Node.ln_Name=DN_PORT;AddPort(dn_port);return 1;
}
static int dn_control_poll(int closing){struct dn_message*m;unsigned n=0;int stop=0;
 if(!dn_port)return 0;
 while(n++<4&&(m=(struct dn_message*)GetMsg(dn_port))){
  if(m->msg.mn_Length==sizeof(*m)&&m->magic==DN_CONTROL_MAGIC){struct zz_net_stats s=zz_net_statistics();
   m->result=closing?2:0;m->tx=s.sent;m->rx=s.received;m->dropped=s.dropped;m->errors=s.errors;
   if(!closing&&m->op==DN_STOP)stop=1;else if(m->op!=DN_STATUS)m->result=1;
  }ReplyMsg(&m->msg);
 }return stop;
}
static void dn_control_close(void){if(dn_port){RemPort(dn_port);while(dn_port->mp_MsgList.lh_Head->ln_Succ)dn_control_poll(1);DeleteMsgPort(dn_port);dn_port=NULL;}}
