/* ASan/UBSan: exercise the actual asynchronous device transport with Exec stubs. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../amiga/dosbox/pc/serial_config.h"
#include "../../../amiga/dosbox/pc/serial_wire.h"
#define DB_SERIAL_DEVICE_TEST
#define CMD_READ 2
#define CMD_WRITE 3
#define SDCMD_QUERY 9
#define SDCMD_BREAK 10
#define SDCMD_SETPARAMS 11
#define SERF_XDISABLED 128
#define SERF_7WIRE 4
#define SERF_PARTY_ON 1
#define SERF_PARTY_ODD 2
#define SEXTF_MSPON 2
#define SEXTF_MARK 1
#define SER_DEFAULT_CTLCHAR 0x11130000
#define IO_STATF_OVERRUN 256
#define IO_STATF_READBREAK 1024
typedef long LONG;typedef char *STRPTR;
struct MsgPort {int unused;};struct IORequest;
struct IOStdReq {unsigned io_Command,io_Flags,io_Actual,io_Length;int io_Error;void *io_Data,*io_Device,*io_Unit;};
struct IOExtSer {struct IOStdReq IOSer;unsigned io_Baud,io_ReadLen,io_WriteLen,io_StopBits,io_SerFlags,io_ExtFlags,io_BrkTime,io_RBufLen,io_CtlChar,io_Status;int pending,done;};
static unsigned allocated,opens,closes,waits,aborts,param_calls,breaks,fail_open,fail_alloc,fail_params,hold_write,short_write;
static unsigned incoming_n,outgoing_n,modem_status=0x38;static uint8_t incoming[512],outgoing[512];
static struct MsgPort*CreateMsgPort(void){return calloc(1,sizeof(struct MsgPort));}
static void DeleteMsgPort(struct MsgPort*p){free(p);}
static void *CreateIORequest(struct MsgPort*p,unsigned n){(void)p;if(fail_alloc&&allocated==fail_alloc-1)return NULL;allocated++;return calloc(1,n);}
static void DeleteIORequest(struct IORequest*p){struct IOExtSer*r=(void*)p;assert(!r->pending);allocated--;free(p);}
static LONG OpenDevice(STRPTR name,unsigned unit,struct IORequest*p,unsigned flags){struct IOExtSer*r=(void*)p;(void)name;(void)unit;assert(!flags&&!(r->io_SerFlags&32));if(fail_open)return 1;opens++;r->IOSer.io_Device=(void*)1;return 0;}
static void CloseDevice(struct IORequest*p){assert(((struct IOExtSer*)p)->IOSer.io_Device);closes++;}
static void SendIO(struct IORequest*p){struct IOExtSer*r=(void*)p;assert(!r->pending);r->pending=1;r->done=0;}
static int CheckIO(struct IORequest*p){struct IOExtSer*r=(void*)p;assert(r->pending);if(r->done)return 1;
 if(r->IOSer.io_Command==CMD_WRITE&&hold_write)return 0;
 r->done=1;
 switch(r->IOSer.io_Command){
 case SDCMD_SETPARAMS:param_calls++;r->IOSer.io_Error=fail_params?5:0;break;
 case SDCMD_QUERY:r->IOSer.io_Actual=incoming_n;r->io_Status=modem_status;break;
 case SDCMD_BREAK:breaks++;break;
 case CMD_WRITE:{unsigned n=r->IOSer.io_Length;if(short_write&&n>short_write)n=short_write;assert(outgoing_n+n<=sizeof(outgoing));memcpy(outgoing+outgoing_n,r->IOSer.io_Data,n);outgoing_n+=n;r->IOSer.io_Actual=n;break;}
 case CMD_READ:assert(r->IOSer.io_Length<=incoming_n);r->IOSer.io_Actual=r->IOSer.io_Length;memcpy(r->IOSer.io_Data,incoming,r->IOSer.io_Actual);incoming_n-=r->IOSer.io_Actual;memmove(incoming,incoming+r->IOSer.io_Actual,incoming_n);break;
 default:assert(0);
 }return 1;
}
static void WaitIO(struct IORequest*p){struct IOExtSer*r=(void*)p;assert(r->pending&&r->done);r->pending=r->done=0;waits++;}
static void AbortIO(struct IORequest*p){struct IOExtSer*r=(void*)p;assert(r->pending&&!r->done);r->done=1;r->IOSer.io_Error=-2;aborts++;}
static uint8_t memory[0x1c0000];static volatile uint8_t*shared=memory;
static unsigned get(unsigned at){return ad_get(shared,at);}static void put(unsigned at,uint32_t v){ad_put(shared,at,v);}
static void sync_ring(unsigned data,unsigned index,unsigned count,int push){(void)data;(void)index;(void)push;assert(count<=256);}
#include "../../../amiga/dosbox/pc/serial_device.h"
static void reset(void){memset(memory,0,sizeof(memory));incoming_n=outgoing_n=param_calls=breaks=0;fail_open=fail_alloc=fail_params=hold_write=short_write=0;put(PC_SERIAL_CONFIG,12u|(3u<<16));}
static void poll_n(unsigned n){while(n--)assert(ser_poll());}
int main(void){struct db_serial_config c;db_serial_defaults(&c);assert(db_serial_config_valid(&c));
 assert(db_serial_uart_config(12u|(3u<<16),&c)&&c.baud==9600&&c.bits==8&&!c.parity&&c.stop==1);
 assert(db_serial_uart_config(48u|(26u<<16),&c)&&c.baud==2400&&c.bits==7&&c.parity==2);
 assert(!db_serial_uart_config(12u,&c));db_serial_defaults(&c);
 strcpy(c.device,"foo.device;quit");assert(!db_serial_config_valid(&c));db_serial_defaults(&c);
 for(unsigned failure=1;failure<=3;failure++){reset();fail_alloc=failure;assert(!ser_open(&c));ser_close();assert(!allocated);}
 reset();fail_open=1;assert(!ser_open(&c));ser_close();assert(!allocated);
 reset();fail_params=1;assert(ser_open(&c));assert(!ser_poll());ser_close();assert(!allocated&&opens==closes);
 reset();assert(ser_open(&c));poll_n(3);assert(param_calls==1&&get(PC_SERIAL_STATUS)==2);
 /* No bytes discarded when guest ring is full; subsequently wrap both rings. */
 put(PC_SERIAL_RX_READ,0xfffffff8u);put(PC_SERIAL_RX_WRITE,0xfffffff8u+PC_SERIAL_SIZE);memcpy(incoming,"0123456789abcdef",16);incoming_n=16;poll_n(3);assert(incoming_n==16&&!ser_reading);
 put(PC_SERIAL_RX_WRITE,0xfffffff8u);poll_n(4);assert(!incoming_n&&get(PC_SERIAL_RX_WRITE)==8);
 for(unsigned i=0;i<16;i++)assert(memory[PC_SERIAL_RX_DATA+((0xfffffff8u+i)&4095)]=="0123456789abcdef"[i]);
 put(PC_SERIAL_TX_READ,0xfffffff8u);put(PC_SERIAL_TX_WRITE,8);for(unsigned i=0;i<16;i++)memory[PC_SERIAL_TX_DATA+((0xfffffff8u+i)&4095)]=(uint8_t)i;
 hold_write=1;poll_n(2);assert(get(PC_SERIAL_TX_READ)==0xfffffff8u);
 put(PC_SERIAL_CONFIG,6u|(3u<<16));poll_n(2);assert(param_calls==1); /* No SETPARAMS with pending write. */
 hold_write=0;short_write=5;poll_n(10);assert(outgoing_n==16&&get(PC_SERIAL_TX_READ)==8&&param_calls==2);
 for(unsigned i=0;i<16;i++)assert(outgoing[i]==i);
 modem_status=4;poll_n(3);assert(get(PC_SERIAL_STATUS)==31);
 put(PC_SERIAL_BREAK,1);poll_n(3);assert(breaks==1);ser_close();assert(!allocated&&opens==closes);
 /* Fixed physical baud ignores guest changes; cancellation drains outstanding messages. */
 reset();c.baud=19200;assert(ser_open(&c));poll_n(3);put(PC_SERIAL_CONFIG,6u|(3u<<16));poll_n(3);assert(param_calls==1);
 memory[PC_SERIAL_TX_DATA]=42;put(PC_SERIAL_TX_WRITE,1);hold_write=1;poll_n(2);ser_close();assert(aborts&&opens==closes&&!allocated&&waits);
 reset();db_serial_defaults(&c);assert(ser_open(&c));poll_n(2);put(PC_SERIAL_TX_WRITE,4097);assert(!ser_poll());ser_close();assert(!allocated);
 puts("PASS physical serial: async lifecycle, partial writes, backpressure/wrap, framing, status, break, blocked CTS cancellation, failure cleanup");
}
