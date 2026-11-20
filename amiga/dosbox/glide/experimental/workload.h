#ifndef GRX_WORKLOAD_H
#define GRX_WORKLOAD_H
#include "translator.h"
#define GRX_WORKLOAD_COLOR 0x06610000u
#define GRX_WORKLOAD_TEXTURE 0x066c0000u
#define GRX_WORKLOAD_PACKETS 65u
/* Profiles 1=320x240 and 2=640x480. Each frame: clear + 320 textured triangles. */
unsigned grx_workload_width(unsigned profile);
unsigned grx_workload_height(unsigned profile);
int grx_workload_packet(grx_packet *,unsigned profile,unsigned batch);
unsigned grx_workload_verify(const volatile uint16_t *,unsigned profile,unsigned *checked);
#endif
