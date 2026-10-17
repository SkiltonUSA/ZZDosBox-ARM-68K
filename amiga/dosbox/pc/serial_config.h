/* GPL-3.0-or-later. Physical Amiga serial-device settings. */
#pragma once
#include <string.h>
struct db_serial_config {char device[64];unsigned unit,baud,bits,parity,stop,flow;};
/* baud zero follows the guest UART. Other values override physical framing. */
static inline void db_serial_defaults(struct db_serial_config*c){memset(c,0,sizeof(*c));strcpy(c->device,"serial.device");c->bits=8;c->stop=1;}
static inline int db_serial_config_valid(const struct db_serial_config*c){
 unsigned i;if(!c->device[0]||c->unit>255||c->flow>1||c->parity>4||(c->bits!=7&&c->bits!=8)||(c->stop!=1&&c->stop!=2)||(c->baud&&(c->baud<50||c->baud>115200)))return 0;
 for(i=0;i<sizeof(c->device);i++){unsigned ch=(unsigned char)c->device[i];if(!ch)return 1;if(!((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch=='.'||ch=='_'||ch=='-'||ch==':'||ch=='/'))return 0;}return 0;
}
/* Return false for unsupported 5/6-bit and 1.5-stop-bit guest framing. */
static inline int db_serial_uart_config(unsigned word,struct db_serial_config*c){
 unsigned divisor=word&65535,lcr=(word>>16)&63;
 if(!divisor)return 0;
 c->baud=115200/divisor;c->bits=5+(lcr&3);c->stop=(lcr&4)?2:1;
 c->parity=!(lcr&8)?0:(lcr&32)?((lcr&16)?4:3):((lcr&16)?2:1);
 return db_serial_config_valid(c);
}
