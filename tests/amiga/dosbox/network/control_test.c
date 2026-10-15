/* Actual control service with fake Exec queues: no target hardware required. */
#include <assert.h>
#include <string.h>
#include "backend.h"
#include "control.h"
static struct MsgPort port;static struct Node sentinel;
static unsigned present,fail,reply_count,deleted;
static struct MsgPort*FindPort(const char*n){assert(!strcmp(n,DN_PORT));return present?&port:NULL;}
static struct MsgPort*CreateMsgPort(void){if(fail)return NULL;memset(&port,0,sizeof(port));port.mp_MsgList.lh_Head=&sentinel;return &port;}
static void AddPort(struct MsgPort*p){assert(p==&port&&!present);present=1;}
static void RemPort(struct MsgPort*p){assert(p==&port&&present);present=0;}
static void DeleteMsgPort(struct MsgPort*p){assert(p==&port&&!present&&!p->mp_MsgList.lh_Head->ln_Succ);deleted++;}
static struct Message*GetMsg(struct MsgPort*p){struct Node*n=p->mp_MsgList.lh_Head;if(!n->ln_Succ)return NULL;p->mp_MsgList.lh_Head=n->ln_Succ;return (struct Message*)n;}
static void ReplyMsg(struct Message*m){assert(m);reply_count++;}
struct zz_net_stats zz_net_statistics(void){struct zz_net_stats s={12,9,1,0};return s;}
#include "control_host.h"
static void enqueue(struct dn_message*m,unsigned n){for(unsigned i=0;i<n;i++){memset(&m[i],0,sizeof(*m));m[i].msg.mn_Length=sizeof(*m);m[i].magic=DN_CONTROL_MAGIC;m[i].op=DN_STATUS;m[i].msg.mn_Node.ln_Succ=i+1<n?&m[i+1].msg.mn_Node:&sentinel;}port.mp_MsgList.lh_Head=&m[0].msg.mn_Node;}
int main(void){struct dn_message m[6];fail=1;assert(!dn_control_open());fail=0;assert(dn_control_open());assert(!dn_control_open());
 enqueue(m,6);assert(!dn_control_poll(0)&&reply_count==4);assert(m[0].tx==12&&m[0].rx==9&&m[0].dropped==1&&!m[0].result);assert(!dn_control_poll(0)&&reply_count==6);
 enqueue(m,1);m[0].op=DN_STOP;assert(dn_control_poll(0)&&!m[0].result);
 enqueue(m,1);m[0].op=DN_STOP;m[0].magic=0;assert(!dn_control_poll(0));
 enqueue(m,1);m[0].op=DN_STOP;m[0].msg.mn_Length--;assert(!dn_control_poll(0));
 enqueue(m,1);m[0].op=99;assert(!dn_control_poll(0)&&m[0].result==1);
 enqueue(m,6);dn_control_close();assert(!dn_port&&!present&&deleted==1);for(unsigned i=0;i<6;i++)assert(m[i].result==2);
 dn_control_close();assert(deleted==1);assert(dn_control_open());dn_control_close();
 puts("PASS network IPC: bounded poll, counters, cooperative stop, malformed messages, pending close and reopen");return 0;
}
