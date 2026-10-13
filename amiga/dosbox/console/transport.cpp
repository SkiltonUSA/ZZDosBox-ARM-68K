/* GPL-2.0-or-later. Core1 console transport. */
#include "console_io.h"
#include "stream.h"
extern "C" volatile uint8_t *db_shared();
static int (*stop_requested)();
static void fence(){__asm__ volatile("dsb sy" ::: "memory");}
static int ready(void*){
 volatile uint8_t *s=db_shared();uint32_t w=ad_get(s,DB_RX_WRITE),r=ad_get(s,DB_RX_READ);fence();
 return (uint32_t)(w-r)>0;
}
static int read(void*){
 volatile uint8_t *s=db_shared();uint32_t w=ad_get(s,DB_RX_WRITE),r=ad_get(s,DB_RX_READ);fence();
 if((uint32_t)(w-r)>DB_RX_SIZE)return -3;
 if(w==r)return ad_get(s,DB_RX_EOF)?-2:-1;
 int c=s[DB_RX_DATA+(r&(DB_RX_SIZE-1))];fence();ad_put(s,DB_RX_READ,r+1);fence();return c;
}
static int write(void*,const uint8_t *p,unsigned n){
 volatile uint8_t *s=db_shared();
 while(n){
  if(stop_requested&&stop_requested())return -1;
  uint32_t w=ad_get(s,DB_TX_WRITE),r=ad_get(s,DB_TX_READ);fence();
  uint32_t used=w-r;if(used>DB_TX_SIZE)return 0;if(used==DB_TX_SIZE)continue;
  unsigned take=DB_TX_SIZE-used;if(take>n)take=n;
  for(unsigned i=0;i<take;i++)s[DB_TX_DATA+((w+i)&(DB_TX_SIZE-1))]=p[i];
  fence();ad_put(s,DB_TX_WRITE,w+take);fence();p+=take;n-=take;
 }
 return 1;
}
extern "C" const DbConsoleIO *db_stream(int (*stop)()){
 static const DbConsoleIO io={nullptr,read,ready,write};stop_requested=stop;return &io;
}
