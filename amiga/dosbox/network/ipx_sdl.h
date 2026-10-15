/* GPL-3.0-or-later. Minimal source-level UDP adapter; no SDL runtime. */
#pragma once
#include <stdint.h>
typedef uint8_t Uint8;typedef uint16_t Uint16;typedef uint32_t Uint32;
struct IPaddress {Uint32 host;Uint16 port;};
typedef int UDPsocket;typedef int SDLNet_SocketSet;
struct UDPpacket {int channel;Uint8*data;int len,maxlen,status;IPaddress address;};
static inline void SDLNet_Write16(Uint16 n,void*v){Uint8*p=(Uint8*)v;p[0]=n>>8;p[1]=n;}
static inline void SDLNet_Write32(Uint32 n,void*v){Uint8*p=(Uint8*)v;p[0]=n>>24;p[1]=n>>16;p[2]=n>>8;p[3]=n;}
static inline Uint16 SDLNet_Read16(const void*v){const Uint8*p=(const Uint8*)v;return ((Uint16)p[0]<<8)|p[1];}
int SDLNet_ResolveHost(IPaddress*,const char*,Uint16);
UDPsocket SDLNet_UDP_Open(Uint16);
void SDLNet_UDP_Close(UDPsocket);
int SDLNet_UDP_Bind(UDPsocket,int,const IPaddress*);
int SDLNet_UDP_Send(UDPsocket,int,UDPpacket*);
int SDLNet_UDP_Recv(UDPsocket,UDPpacket*);
static inline const char*SDLNet_GetError(){return "IPX transport unavailable";}
