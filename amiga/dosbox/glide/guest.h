/* GPL-2.0-or-later. Guest address boundary; never use a guest pointer as a host pointer. */
#ifndef GRX_GUEST_H
#define GRX_GUEST_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
 int (*read)(void *,uint32_t,void *,unsigned);
 int (*write)(void *,uint32_t,const void *,unsigned);
 void *opaque;
 uint32_t data_base; /* Guest near pointers are DS-relative. */
} grx_guest_io;
typedef struct { unsigned id,count; uint32_t args[16]; } grx_guest_call;
int grx_guest_decode(grx_guest_call *,unsigned,uint32_t,const grx_guest_io *);
uint32_t grx_guest_diagnostic(const grx_guest_call *,const grx_guest_io *);
#ifdef __cplusplus
}
#endif
#endif
