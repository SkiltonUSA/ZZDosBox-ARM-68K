/* GPL-3.0-or-later. Byte-order-independent DOSBox IPX/UDP envelope checks. */
#pragma once
#include <stdint.h>
#define ZZ_IPX_MAX 1424u
static inline unsigned zz_ipx_u16(const uint8_t*p){return ((unsigned)p[0]<<8)|p[1];}
static inline int zz_ipx_valid(const uint8_t*p,unsigned n){return p&&n>=30&&n<=ZZ_IPX_MAX&&p[0]==255&&p[1]==255&&zz_ipx_u16(p+2)==n;}
static inline int zz_ipx_registration(const uint8_t*p,unsigned n){return zz_ipx_valid(p,n)&&n==30&&zz_ipx_u16(p+16)==2&&zz_ipx_u16(p+28)==2&&(p[10]|p[11]|p[12]|p[13])&&zz_ipx_u16(p+14)!=0;}
