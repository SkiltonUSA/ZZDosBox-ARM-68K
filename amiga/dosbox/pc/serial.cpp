/* GPL-2.0-or-later. Upstream 16550 UART, with an owned TCP/modem byte transport.
 * Baud, FIFO and interrupts stay on Core1; sockets stay on the Amiga task. */
#include "dosbox.h"
#include "serialport.h"
#include "serial_wire.h"
#include "setup.h"
#include "bios.h"
extern "C" volatile uint8_t *db_shared();
#ifdef DB_NATIVE
static void serial_fence(){__atomic_thread_fence(__ATOMIC_SEQ_CST);}
#else
static void serial_fence(){__asm__ volatile("dsb sy":::"memory");}
#endif
class ZZSerial : public CSerial {
 Bit8u outgoing=0;
 bool pending=false;
 Bit16u config_divider=12;
 unsigned published_config=~0u;
 void publishConfig(){
  if(LCR&0x80)return; /* DOS writes both divisor bytes while DLAB is set. */
  unsigned word=(config_divider?config_divider:1)|((unsigned)(LCR&63)<<16);
  if(word!=published_config){ad_put(db_shared(),PC_SERIAL_CONFIG,word);serial_fence();published_config=word;}
 }
public:
 ZZSerial(CommandLine*cmd):CSerial(0,cmd){Init_Registers();setRI(false);updateMSR();InstallationSuccessful=true;setEvent(SERIAL_POLLING_EVENT,1);}
 void updatePortConfig(Bit16u divider,Bit8u) override {config_divider=divider;}
 void setBreak(bool active) override {if(active){volatile uint8_t*b=db_shared();ad_put(b,PC_SERIAL_BREAK,ad_get(b,PC_SERIAL_BREAK)+1);serial_fence();}}
 void setRTS(bool) override {}
 void setDTR(bool dtr) override {ad_put(db_shared(),PC_SERIAL_DTR,dtr);serial_fence();}
 void setRTSDTR(bool,bool dtr) override {setDTR(dtr);}
 void updateMSR() override {
  unsigned s=ad_get(db_shared(),PC_SERIAL_STATUS);
  bool physical=ad_get(db_shared(),PC_SERIAL_MODE)==3;
  setDSR((s&(physical?PC_SERIAL_DSR:2))!=0);setCTS((s&(physical?PC_SERIAL_CTS:2))!=0);setCD((s&1)!=0);setRI(physical&&(s&PC_SERIAL_RI));
 }
 void transmitByte(Bit8u value,bool first) override {
  outgoing=value;pending=true;
  setEvent(first?SERIAL_THR_EVENT:SERIAL_TX_EVENT,first?bytetime/10:bytetime);
 }
 void handleUpperEvent(Bit16u type) override {
  volatile uint8_t*b=db_shared();publishConfig();
  if(type==SERIAL_THR_EVENT){ByteTransmitting();setEvent(SERIAL_TX_EVENT,bytetime);return;}
  if(type==SERIAL_TX_EVENT&&pending){
   unsigned w=ad_get(b,PC_SERIAL_TX_WRITE),r=ad_get(b,PC_SERIAL_TX_READ);serial_fence();
   if(w-r>PC_SERIAL_SIZE)E_Exit("Serial TX queue bound");
   if(w-r==PC_SERIAL_SIZE){setEvent(SERIAL_TX_EVENT,1);return;}
   b[PC_SERIAL_TX_DATA+(w&(PC_SERIAL_SIZE-1))]=outgoing;serial_fence();ad_put(b,PC_SERIAL_TX_WRITE,w+1);pending=false;ByteTransmitted();
  }
  if(type==SERIAL_POLLING_EVENT){
   updateMSR();unsigned r=ad_get(b,PC_SERIAL_RX_READ),w=ad_get(b,PC_SERIAL_RX_WRITE);serial_fence();
   if(w-r>PC_SERIAL_SIZE)E_Exit("Serial RX queue bound");
   if(r!=w&&CanReceiveByte()){receiveByte(b[PC_SERIAL_RX_DATA+(r&(PC_SERIAL_SIZE-1))]);serial_fence();ad_put(b,PC_SERIAL_RX_READ,r+1);}
   setEvent(SERIAL_POLLING_EVENT,r!=w?bytetime:1);
  }
 }
};
void db_serial_open(){
 Bit16u ports[4]={};
 if(ad_get(db_shared(),PC_SERIAL_MODE)){
  CommandLine cmd(nullptr,"");serialports[0]=new ZZSerial(&cmd);ports[0]=0x3f8;
 }
 BIOS_SetComPorts(ports);
}
void db_serial_close(){delete serialports[0];serialports[0]=nullptr;}
