/* GPL-3.0-or-later. Guest-memory tools inside the Developer tab. */
#include "../developer/memory_ipc.h"
#include "../developer/disasm.h"
#include <stdlib.h>
#include <errno.h>
static Object *mem_enabled,*mem_address,*mem_width,*mem_value,*mem_watch,*mem_hex,*mem_state;
static Object *mem_start,*mem_length,*mem_compare,*mem_matches,*mem_bits,*mem_asm;
static struct MsgPort *mem_replies;
static struct dm_message mem_packet;
static unsigned mem_busy,mem_view,mem_total,mem_page,mem_valid,mem_undo_valid,mem_paused;
static uint32_t mem_base,mem_size,mem_addresses[32],mem_values[32];
static uint8_t mem_snapshot[DM_BYTES];
static struct dm_request mem_undo;
static const char *mem_tabs[]={"Monitor","Scanner","Disassembler",NULL};
static const char *mem_widths[]={"8-bit","16-bit","32-bit",NULL};
static const char *mem_conditions[]={"Equal","Unknown initial","Changed","Unchanged","Increased","Decreased",NULL};
static const char *mem_modes[]={"16-bit x86","32-bit x86",NULL};
enum {ID_MEM_PAUSE=300,ID_MEM_RESUME,ID_MEM_REFRESH,ID_MEM_WRITE,ID_MEM_UNDO,ID_MEM_SCAN,ID_MEM_REFINE,ID_MEM_PREV,ID_MEM_NEXT,ID_MEM_SELECT,ID_MEM_DISASM,ID_MEM_STOP};
static void mem_text(Object *o,uint32_t n){char b[24];snprintf(b,sizeof(b),"%08lx",(unsigned long)n);set(o,MUIA_String_Contents,(ULONG)b);}
static int mem_number(Object *o,unsigned base,uint32_t *out){
 const char *s=(const char*)val(o,MUIA_String_Contents);char *end;unsigned long n;
 if(!s||!*s||*s=='-'||*s=='+'||*s==' '){note("Enter an unsigned number.");return 0;}
 if(!base)base=s[0]=='0'&&(s[1]=='x'||s[1]=='X')?16:10;
 errno=0;n=strtoul(s,&end,base);if(errno||*end||n>0xfffffffful){note("Invalid number.");return 0;}*out=n;return 1;
}
static int mem_send(struct dm_request q,unsigned view){
 struct MsgPort *server;
 if(mem_busy){note("Memory request pending.");return 0;}
 if(!mem_replies){note("Memory reply port unavailable.");return 0;}
 memset(&mem_packet,0,sizeof(mem_packet));mem_packet.msg.mn_Length=sizeof(mem_packet);
 mem_packet.msg.mn_ReplyPort=mem_replies;mem_packet.magic=DM_MAGIC;mem_packet.request=q;
 Forbid();server=FindPort(DM_PORT);if(server)PutMsg(server,&mem_packet.msg);Permit();
 if(!server){note("Launch with Memory tools enabled first.");return 0;}
 mem_busy=1;mem_view=view;return 1;
}
static void mem_read_page(unsigned view){
 struct dm_request q={0};q.op=DM_READ;q.length=DM_BYTES;
 if(!mem_number(mem_address,16,&q.address))return;
 /* Allow the final partial page up to a RAM boundary or legacy hole. */
 if(q.address<0xa0000u&&q.length>0xa0000u-q.address)q.length=0xa0000u-q.address;
 if(mem_size&&q.address<mem_size&&q.length>mem_size-q.address)q.length=mem_size-q.address;
 mem_send(q,view);
}
static void mem_render_hex(void){
 unsigned row,j;char line[100];DoMethod(mem_hex,MUIM_List_Clear);
 for(row=0;row<mem_valid;row+=16){unsigned used=snprintf(line,sizeof(line),"%08lx  ",(unsigned long)(mem_base+row));
  for(j=0;j<16;j++)used+=snprintf(line+used,sizeof(line)-used,row+j<mem_valid?"%02x ":"   ",mem_snapshot[row+j<mem_valid?row+j:0]);
  line[used++]=' ';for(j=0;j<16&&row+j<mem_valid;j++){unsigned c=mem_snapshot[row+j];line[used++]=c>=32&&c<127?c:'.';}line[used]=0;
  DoMethod(mem_hex,MUIM_List_InsertSingle,(ULONG)line,MUIV_List_Insert_Bottom);
 }
}
static void mem_render_asm(void){
 unsigned pos=0,lines=0;char text[256],line[320];DoMethod(mem_asm,MUIM_List_Clear);
 while(pos<mem_valid&&lines++<32){
  unsigned n=dm_disassemble(text,sizeof(text),mem_snapshot+pos,mem_valid-pos,mem_base+pos,val(mem_bits,MUIA_Cycle_Active)?32:16);
  if(!n)break;
  snprintf(line,sizeof(line),"%08lx  %s",(unsigned long)(mem_base+pos),text);
  DoMethod(mem_asm,MUIM_List_InsertSingle,(ULONG)line,MUIV_List_Insert_Bottom);pos+=n;
 }
}
static void mem_results(void){struct dm_request q={0};q.op=DM_RESULTS;q.index=mem_page;mem_send(q,0);}
static void mem_poll(void){
 struct Message *message;struct dm_reply *r;struct dm_request q;char text[192];unsigned i;
 if(!mem_replies||!(message=GetMsg(mem_replies)))return;
 if(message!=&mem_packet.msg)return;
 mem_busy=0;r=&mem_packet.reply;q=mem_packet.request;mem_paused=r->paused;mem_size=r->ram_bytes;
 if(r->error){
  static const char *errors[]={"","Invalid or stale request; refresh first.","Pause before editing or scanning.","Outside guest RAM or invalid range.","Over 4096 matches; narrow the scan range.","Start a new scan first.","Emulator closed."};
  note(r->error<=DM_OFFLINE?errors[r->error]:"Memory protocol error.");return;
 }
 snprintf(text,sizeof(text),"%s | RAM %lu MB | CS:EIP %04lx:%08lx | ESP %08lx",r->paused?"Paused":"Running",(unsigned long)(r->ram_bytes>>20),(unsigned long)r->cs,(unsigned long)r->eip,(unsigned long)r->esp);
 set(mem_state,MUIA_Text_Contents,(ULONG)text);
 if(q.op==DM_READ){mem_base=q.address;mem_valid=r->count;memcpy(mem_snapshot,r->bytes,DM_BYTES);if(mem_view)mem_render_asm();else mem_render_hex();}
 if(q.op==DM_WRITE){unsigned undoing=mem_view==2;mem_undo=q;mem_undo.value=q.expected;mem_undo.expected=q.value;mem_undo_valid=!undoing;mem_read_page(0);note(undoing?"Edit undone.":"Guest RAM updated.");}
 if(q.op==DM_SCAN||q.op==DM_REFINE){mem_total=r->count;mem_page=0;mem_results();}
 if(q.op==DM_RESULTS){
  DoMethod(mem_matches,MUIM_List_Clear);
  for(i=0;i<r->count&&i<32;i++){
   const uint8_t *b=r->bytes+i*8;uint32_t a=0,v=0;unsigned j;for(j=0;j<4;j++){a=(a<<8)|b[j];v=(v<<8)|b[j+4];}
   mem_addresses[i]=a;mem_values[i]=v;
   snprintf(text,sizeof(text),"%08lx   %lu (0x%lx)",(unsigned long)a,(unsigned long)v,(unsigned long)v);
   DoMethod(mem_matches,MUIM_List_InsertSingle,(ULONG)text,MUIV_List_Insert_Bottom);
  }
  snprintf(text,sizeof(text),"%lu matches; showing %lu-%lu",(unsigned long)mem_total,(unsigned long)(r->count?mem_page+1:0),(unsigned long)(mem_page+r->count));note(text);
 }
}
static void mem_action(unsigned id){
 struct dm_request q={0};uint32_t v;unsigned width=1u<<val(mem_width,MUIA_Cycle_Active);
 if(id==ID_MEM_REFRESH){mem_read_page(0);return;}
 if(id==ID_MEM_DISASM){mem_read_page(1);return;}
 if(id==ID_MEM_SELECT){LONG i=(LONG)val(mem_matches,MUIA_List_Active);if(i>=0&&i<32&&(unsigned)i+mem_page<mem_total){char text[24];mem_text(mem_address,mem_addresses[i]);snprintf(text,sizeof(text),"%lu",(unsigned long)mem_values[i]);set(mem_value,MUIA_String_Contents,(ULONG)text);}return;}
 if(id==ID_MEM_PAUSE)q.op=DM_PAUSE;
 else if(id==ID_MEM_RESUME)q.op=DM_RESUME;
 else if(id==ID_MEM_STOP)q.op=DM_STOP;
 else if(id==ID_MEM_WRITE){
  if(!mem_number(mem_address,16,&q.address)||!mem_number(mem_value,0,&q.value))return;
  if(q.address<mem_base||q.address-mem_base>mem_valid||width>mem_valid-(q.address-mem_base)){note("Refresh this address before editing.");return;}
  q.op=DM_WRITE;q.width=width;for(v=0;v<width;v++)q.expected|=(uint32_t)mem_snapshot[q.address-mem_base+v]<<(8*v);
 }else if(id==ID_MEM_UNDO){if(!mem_undo_valid){note("No edit to undo.");return;}q=mem_undo;}
 else if(id==ID_MEM_SCAN||id==ID_MEM_REFINE){
  q.op=id==ID_MEM_SCAN?DM_SCAN:DM_REFINE;q.width=width;q.compare=val(mem_compare,MUIA_Cycle_Active);
  if(!mem_number(mem_start,16,&q.address)||!mem_number(mem_length,0,&q.length)||!mem_number(mem_value,0,&q.value))return;
 }else if(id==ID_MEM_PREV||id==ID_MEM_NEXT){
  if(id==ID_MEM_PREV)mem_page=mem_page>=32?mem_page-32:0;
  else if(mem_page+32<mem_total)mem_page+=32;
  mem_results();return;
 }else return;
 mem_send(q,id==ID_MEM_UNDO?2:0);
}
static Object *mem_button(const char *s,unsigned id){Object *o=ui_button(s);set(o,MUIA_UserData,id);return o;}
static Object *mem_list(Object **list){return ListviewObject,MUIA_Listview_List,*list=ListObject,InputListFrame,MUIA_Font,MUIV_Font_Fixed,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End;}
static Object *memory_page(void){
 return VGroup,
 Child,HGroup,Child,mem_button("Pause",ID_MEM_PAUSE),Child,mem_button("Resume",ID_MEM_RESUME),Child,mem_button("Stop",ID_MEM_STOP),End,
 Child,mem_state=label("Guest physical RAM; video/ROM excluded."),
 Child,ColGroup(2),Child,fieldlabel("Address (hex)"),Child,mem_address=str(11),Child,fieldlabel("Value (decimal / 0x)"),Child,mem_value=str(16),Child,fieldlabel("Width"),Child,mem_width=cycle(mem_widths),End,
 Child,RegisterGroup(mem_tabs),MUIA_Font,ui_font(),
 Child,VGroup,Child,mem_list(&mem_hex),Child,HGroup,Child,mem_button("Refresh",ID_MEM_REFRESH),Child,mem_watch=CheckMark(FALSE),Child,label("Monitor"),Child,mem_button("Write",ID_MEM_WRITE),Child,mem_button("Undo",ID_MEM_UNDO),End,End,
 Child,VGroup,Child,ColGroup(2),Child,fieldlabel("Start (hex)"),Child,mem_start=str(11),Child,fieldlabel("Bytes (max 65536)"),Child,mem_length=str(11),Child,fieldlabel("Comparison"),Child,mem_compare=cycle(mem_conditions),End,
 Child,mem_list(&mem_matches),Child,HGroup,Child,mem_button("New scan",ID_MEM_SCAN),Child,mem_button("Refine",ID_MEM_REFINE),Child,mem_button("Previous",ID_MEM_PREV),Child,mem_button("Next",ID_MEM_NEXT),End,End,
 Child,VGroup,Child,mem_bits=cycle(mem_modes),Child,mem_list(&mem_asm),Child,mem_button("Disassemble",ID_MEM_DISASM),End,End,End;
}
static void memory_ui_init(void){
 mem_replies=CreateMsgPort();mem_text(mem_address,0x10000);mem_text(mem_start,0x10000);
 set(mem_length,MUIA_String_Contents,(ULONG)"4096");set(mem_value,MUIA_String_Contents,(ULONG)"0");
 notify(mem_matches,MUIA_List_Active,MUIV_EveryTime,ID_MEM_SELECT);
}
/* A worker waits for DOS; MUI and the memory reply port stay on the UI task. */
struct mem_job {struct Message done;const char *command;BPTR log;LONG result;};
static struct mem_job mem_job;
static void memory_player_worker(void){
 mem_job.result=SystemTags((STRPTR)mem_job.command,SYS_Input,0,SYS_Output,mem_job.log,SYS_Asynch,FALSE,NP_StackSize,65536,TAG_DONE);
 /* Return into DOS teardown before the parent can unload this entry code. */
 Forbid();
 PutMsg(mem_job.done.mn_ReplyPort,&mem_job.done);
}
static LONG player_run(const char *command,BPTR log){
 struct MsgPort *done;struct Process *process;ULONG signals,id,ticks=0;int finished=0,close_requested=0;
 if(mem_busy)return 20;
 if(!loading_splash())return 0;
 if(!val(mem_enabled,MUIA_Selected)&&!network_requested){
  LONG rc;set(win,MUIA_Window_Open,FALSE);rc=SystemTags((STRPTR)command,SYS_Input,0,SYS_Output,log,SYS_Asynch,FALSE,NP_StackSize,65536,TAG_DONE);set(win,MUIA_Window_Open,TRUE);return rc;
 }
 if(network_requested&&!network_live_begin())return 20;
 done=CreateMsgPort();if(!done){network_live_end();return 20;}
 memset(&mem_job,0,sizeof(mem_job));mem_job.done.mn_ReplyPort=done;mem_job.command=command;mem_job.log=log;
 mem_valid=mem_total=mem_undo_valid=mem_paused=mem_size=0;
 process=CreateNewProcTags(NP_Entry,(ULONG)memory_player_worker,NP_Name,(ULONG)"ZZDosBox session",NP_StackSize,65536,NP_Input,0,NP_Output,0,NP_CloseInput,FALSE,NP_CloseOutput,FALSE,TAG_DONE);
 if(!process){DeleteMsgPort(done);network_live_end();return 20;}
 navigation_show(network_running?PAGE_NETWORK:PAGE_DEVELOPER);set(navigation_view,MUIA_Disabled,TRUE);note(network_running?"Network DOS session running.":"Developer session running.");
 while(!finished||mem_busy||network_busy){
  mem_poll();network_poll();if(GetMsg(done))finished=1;
  id=DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);
  if(network_running&&id)network_live_event(id);
  else if(id>=ID_MEM_PAUSE&&id<=ID_MEM_STOP)mem_action(id);
  else if(id==(ULONG)MUIV_Application_ReturnID_Quit)close_requested=1;
  else if(id)note("Stop the session before changing launch settings.");
  if(close_requested&&!finished&&!mem_busy&&!network_running){mem_action(ID_MEM_STOP);close_requested=0;}
  if(!network_running&&!finished&&!mem_busy&&val(mem_watch,MUIA_Selected)&&++ticks>=12){ticks=0;mem_read_page(0);}
  if(!finished||mem_busy||network_busy)Delay(1);
 }
 set(navigation_view,MUIA_Disabled,FALSE);DeleteMsgPort(done);network_live_end();set(mem_state,MUIA_Text_Contents,(ULONG)"Session ended.");return mem_job.result;
}
