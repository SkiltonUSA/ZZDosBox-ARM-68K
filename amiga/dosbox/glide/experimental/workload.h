#ifndef GRX_WORKLOAD_H
#define GRX_WORKLOAD_H
#include "translator.h"
#define GRX_WORKLOAD_COLOR 0x06610000u
#define GRX_WORKLOAD_TEXTURE 0x066c0000u
#define GRX_WORKLOAD_PACKETS 65u
/* Profiles 1/3=320x240, 2/4=640x480. Profiles 3/4 pair drawing packets.
 * All profiles generate the same 65 packets: clear + 320 textured triangles. */
unsigned grx_workload_width(unsigned profile);
unsigned grx_workload_height(unsigned profile);
unsigned grx_workload_submissions(unsigned profile);
int grx_workload_packet(grx_packet *,unsigned profile,unsigned batch);
unsigned grx_workload_verify(const volatile uint16_t *,unsigned profile,unsigned *checked);
#endif
