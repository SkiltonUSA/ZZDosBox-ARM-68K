/* GPL-3.0-or-later. Included by serial_host.c; exclusive IOExtSer ownership.
 * All device operations are asynchronous. No polling loop waits for CTS/data. */
#ifndef DB_SERIAL_DEVICE_TEST
#include <devices/serial.h>
#include <proto/exec.h>
#endif
#include <stdio.h>
static struct MsgPort *ser_port;
static struct IOExtSer *ser_ctl,*ser_rx,*ser_tx;
static struct db_serial_config ser_config;
static unsigned ser_opened,ser_control,ser_reading,ser_writing,ser_word,ser_break,ser_applied;
static unsigned ser_tx_index,ser_rx_index,ser_status=~0u;
static uint8_t ser_in[256],ser_out[256];
enum {SER_CONTROL_NONE,SER_CONTROL_PARAMS,SER_CONTROL_QUERY,SER_CONTROL_BREAK};
static void ser_start(struct IOExtSer*r,unsigned cmd){r->IOSer.io_Command=cmd;r->IOSer.io_Error=0;r->IOSer.io_Flags=0;r->IOSer.io_Actual=0;SendIO((struct IORequest*)r);}
static int ser_finished(struct IOExtSer*r){if(!CheckIO((struct IORequest*)r))return 0;WaitIO((struct IORequest*)r);return 1;}
static void ser_cancel(struct IOExtSer*r,unsigned active){if(active){if(!CheckIO((struct IORequest*)r))AbortIO((struct IORequest*)r);WaitIO((struct IORequest*)r);}}
static void ser_close(void){
 ser_cancel(ser_rx,ser_reading);ser_cancel(ser_tx,ser_writing);ser_cancel(ser_ctl,ser_control);
 if(ser_opened)CloseDevice((struct IORequest*)ser_ctl);
 if(ser_rx)DeleteIORequest((struct IORequest*)ser_rx);if(ser_tx)DeleteIORequest((struct IORequest*)ser_tx);if(ser_ctl)DeleteIORequest((struct IORequest*)ser_ctl);if(ser_port)DeleteMsgPort(ser_port);
 ser_rx=ser_tx=ser_ctl=NULL;ser_port=NULL;ser_opened=ser_control=ser_reading=ser_writing=ser_applied=0;
}
static int ser_params(unsigned word){
 struct db_serial_config c=ser_config;
 if(!c.baud&&!db_serial_uart_config(word,&c)){printf("SERIAL unsupported guest baud/framing %08lx\n",(unsigned long)word);return 0;}
 ser_ctl->io_Baud=c.baud;ser_ctl->io_ReadLen=ser_ctl->io_WriteLen=c.bits;ser_ctl->io_StopBits=c.stop;
 ser_ctl->io_SerFlags=SERF_XDISABLED|(c.flow?SERF_7WIRE:0)|(c.parity?SERF_PARTY_ON:0)|(c.parity==1?SERF_PARTY_ODD:0);
 ser_ctl->io_ExtFlags=c.parity>=3?SEXTF_MSPON|(c.parity==3?SEXTF_MARK:0):0;
 ser_ctl->io_BrkTime=250000;ser_ctl->io_RBufLen=4096;ser_ctl->io_CtlChar=SER_DEFAULT_CTLCHAR;
 ser_word=word;ser_control=SER_CONTROL_PARAMS;ser_start(ser_ctl,SDCMD_SETPARAMS);
 printf("SERIAL settings baud=%u bits=%u parity=%u stop=%u flow=%u\n",c.baud,c.bits,c.parity,c.stop,c.flow);return 1;
}
static int ser_open(const struct db_serial_config*c){
 if(!c||!db_serial_config_valid(c))return 0;
 ser_config=*c;ser_status=~0u;ser_break=0;ser_port=CreateMsgPort();if(!ser_port)return 0;
 ser_ctl=(struct IOExtSer*)CreateIORequest(ser_port,sizeof(*ser_ctl));
 ser_rx=(struct IOExtSer*)CreateIORequest(ser_port,sizeof(*ser_rx));ser_tx=(struct IOExtSer*)CreateIORequest(ser_port,sizeof(*ser_tx));
 if(!ser_ctl||!ser_rx||!ser_tx)return 0;
 ser_ctl->io_SerFlags=SERF_XDISABLED|(c->flow?SERF_7WIRE:0);
 {LONG error=OpenDevice((STRPTR)c->device,c->unit,(struct IORequest*)ser_ctl,0);if(error){printf("SERIAL cannot open %s unit %u (error %ld); check driver / port in use\n",c->device,c->unit,(long)error);return 0;}}
 ser_opened=1;
 /* Preserve each request's own message/header; share only the opened device. */
 ser_rx->IOSer.io_Device=ser_tx->IOSer.io_Device=ser_ctl->IOSer.io_Device;
 ser_rx->IOSer.io_Unit=ser_tx->IOSer.io_Unit=ser_ctl->IOSer.io_Unit;
 printf("SERIAL physical experimental: %s unit %u\n",c->device,c->unit);
 return ser_params(12u|(3u<<16));
}
static int ser_error(struct IOExtSer*r,const char*op){if(!r->IOSer.io_Error)return 0;printf("SERIAL %s error %d (actual %lu)\n",op,(int)r->IOSer.io_Error,(unsigned long)r->IOSer.io_Actual);return 1;}
static int ser_poll(void){
 unsigned w,r,n,i,word,brk;
 if(ser_writing&&ser_finished(ser_tx)){
  ser_writing=0;if(ser_tx->IOSer.io_Actual>ser_tx->IOSer.io_Length)return 0;
  put(PC_SERIAL_TX_READ,ser_tx_index+ser_tx->IOSer.io_Actual);
  if(ser_error(ser_tx,"write"))return 0;
 }
 if(ser_reading&&ser_finished(ser_rx)){
  ser_reading=0;n=ser_rx->IOSer.io_Actual;if(n>ser_rx->IOSer.io_Length)return 0;
  if(ser_error(ser_rx,"read"))return 0;
  for(i=0;i<n;i++)shared[PC_SERIAL_RX_DATA+((ser_rx_index+i)&(PC_SERIAL_SIZE-1))]=ser_in[i];
  sync_ring(PC_SERIAL_RX_DATA,ser_rx_index,n,1);if(n)put(PC_SERIAL_RX_WRITE,ser_rx_index+n);
 }
 if(ser_control&&ser_finished(ser_ctl)){
  unsigned op=ser_control;ser_control=0;if(ser_error(ser_ctl,"control"))return 0;
  if(op==SER_CONTROL_PARAMS)ser_applied=1;
  if(op==SER_CONTROL_QUERY){
   unsigned s=ser_ctl->io_Status,v=2u|((s&32)?0:1)|((s&16)?0:PC_SERIAL_CTS)|((s&8)?0:PC_SERIAL_DSR)|((s&4)?PC_SERIAL_RI:0);
   if(s&(IO_STATF_OVERRUN|IO_STATF_READBREAK)){printf("SERIAL receive overrun/break status %04x\n",s);return 0;}
   if(v!=ser_status){put(PC_SERIAL_STATUS,v);ser_status=v;}
   if(!ser_reading){w=get(PC_SERIAL_RX_WRITE);r=get(PC_SERIAL_RX_READ);if(w-r>PC_SERIAL_SIZE)return 0;
    n=PC_SERIAL_SIZE-(w-r);if(n>sizeof(ser_in))n=sizeof(ser_in);if(n>ser_ctl->IOSer.io_Actual)n=ser_ctl->IOSer.io_Actual;
    if(n){ser_rx_index=w;ser_rx->IOSer.io_Data=ser_in;ser_rx->IOSer.io_Length=n;ser_reading=1;ser_start(ser_rx,CMD_READ);}
   }
  }
 }
 word=get(PC_SERIAL_CONFIG);brk=get(PC_SERIAL_BREAK);
 if(!ser_applied||(!ser_config.baud&&word!=ser_word)){
  /* Divisor zero can occur while a DOS program writes its two divisor bytes. */
  if(!(word&65535))return 1;
  if(ser_control||ser_reading||ser_writing)return 1;
  return ser_params(word);
 }
 if(!ser_applied)return 1;
 if(brk!=ser_break){if(ser_control||ser_reading||ser_writing)return 1;ser_break=brk;ser_control=SER_CONTROL_BREAK;ser_start(ser_ctl,SDCMD_BREAK);return 1;}
 if(ser_control==SER_CONTROL_PARAMS||ser_control==SER_CONTROL_BREAK)return 1;
 if(!ser_writing){w=get(PC_SERIAL_TX_WRITE);r=get(PC_SERIAL_TX_READ);n=w-r;if(n>PC_SERIAL_SIZE)return 0;if(n>sizeof(ser_out))n=sizeof(ser_out);
  if(n){sync_ring(PC_SERIAL_TX_DATA,r,n,0);for(i=0;i<n;i++)ser_out[i]=shared[PC_SERIAL_TX_DATA+((r+i)&(PC_SERIAL_SIZE-1))];
   ser_tx_index=r;ser_tx->IOSer.io_Data=ser_out;ser_tx->IOSer.io_Length=n;ser_writing=1;ser_start(ser_tx,CMD_WRITE);}
 }
 if(!ser_control){ser_control=SER_CONTROL_QUERY;ser_start(ser_ctl,SDCMD_QUERY);}
 return 1;
}
