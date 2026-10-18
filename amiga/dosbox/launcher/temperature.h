/* GPL-3.0-or-later. Read-only Zynq sensor, shared convention with ZZTemperature.
 * MNT ZZTop: manufacturer 0x6d6e, product 4/3, register +0xe0 in 0.1 C.
 * No XACP commands, ARM worker or card register writes. */
#include <devices/timer.h>
#include <libraries/configvars.h>
#include <proto/expansion.h>
struct ExpansionBase *ExpansionBase;
static volatile UBYTE *temperature_registers;
static Object *temperature_text;
static struct MsgPort *temperature_port;
static struct timerequest *temperature_timer;
static char temperature_caption[40];

static void temperature_sample(void){
 unsigned raw=temperature_registers?*(volatile UWORD *)(temperature_registers+0xe0):0;
 if(raw>0&&raw<=1500)snprintf(temperature_caption,sizeof(temperature_caption),"ZZ9000: %u.%u C",raw/10,raw%10);
 else strcpy(temperature_caption,"ZZ9000: n/a");
 set(temperature_text,MUIA_Text_Contents,(ULONG)temperature_caption);
}
static void temperature_arm(void){
 temperature_timer->tr_node.io_Command=TR_ADDREQUEST;
 temperature_timer->tr_time.tv_secs=2;temperature_timer->tr_time.tv_micro=0;
 SendIO((struct IORequest*)temperature_timer);
}
static void temperature_close(void){
 if(temperature_timer){
  if(!CheckIO((struct IORequest*)temperature_timer))AbortIO((struct IORequest*)temperature_timer);
  WaitIO((struct IORequest*)temperature_timer);CloseDevice((struct IORequest*)temperature_timer);
  DeleteIORequest((struct IORequest*)temperature_timer);temperature_timer=NULL;
 }
 if(temperature_port){DeleteMsgPort(temperature_port);temperature_port=NULL;}
}
static void temperature_init(void){
 ExpansionBase=(struct ExpansionBase*)OpenLibrary("expansion.library",37);
 if(ExpansionBase){
  struct ConfigDev *board=FindConfigDev(NULL,0x6d6e,4);
  if(!board)board=FindConfigDev(NULL,0x6d6e,3);
  if(board&&board->cd_BoardAddr&&board->cd_BoardSize>0xe1)temperature_registers=(volatile UBYTE*)board->cd_BoardAddr;
  CloseLibrary((struct Library*)ExpansionBase);ExpansionBase=NULL;
 }
 if(!temperature_registers){temperature_sample();return;}
 temperature_port=CreateMsgPort();
 if(temperature_port)temperature_timer=(struct timerequest*)CreateIORequest(temperature_port,sizeof(*temperature_timer));
 if(!temperature_timer||OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)temperature_timer,0)){
  if(temperature_timer){DeleteIORequest((struct IORequest*)temperature_timer);temperature_timer=NULL;}
  temperature_close();return; /* Keep n/a rather than display a stale sample. */
 }
 temperature_sample();temperature_arm();
}
static ULONG temperature_signal(void){return temperature_port?1ul<<temperature_port->mp_SigBit:0;}
static void temperature_poll(void){
 if(temperature_timer&&CheckIO((struct IORequest*)temperature_timer)){
  WaitIO((struct IORequest*)temperature_timer);temperature_sample();temperature_arm();
 }
}
