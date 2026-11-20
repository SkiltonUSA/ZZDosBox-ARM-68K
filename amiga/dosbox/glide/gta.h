/* Experimental GTA ECTS subset. No hardware addresses supplied by the guest. */
#ifndef GRX_GTA_H
#define GRX_GTA_H
#include "draw_stream.h"
#define GRX_GTA_TEXTURE_BYTES (2u*1024*1024)
#define GRX_GTA_PIXELS (640u*480)
typedef struct {
 void *context;
 /* Upload/read only after all previous submit calls retired. */
 int (*upload)(void *,uint32_t,const void *,unsigned);
 int (*readback)(void *,uint32_t,void *,unsigned);
 grx_draw_submit submit;
 int (*present)(void *,uint32_t,unsigned,unsigned);
} grx_gta_backend;
typedef struct {unsigned width,height,offset,size,options,resident,downloaded;} grx_gta_texture;
typedef struct {
 grx_gta_backend backend;grx_draw_stream stream;
 unsigned initialized,opened,failed,locked,selected,texture_count,used,resident_end,frames;
 unsigned colour,alpha,alpha_ref,alpha_func;
 uint32_t lfb_address;
 grx_gta_texture textures[1024];
 unsigned char texture[GRX_GTA_TEXTURE_BYTES];
 unsigned char lfb[GRX_GTA_PIXELS*2],before[GRX_GTA_PIXELS*2];
} grx_gta;
/* Allocate this state outside the stack. Host provides reserved guest LFB RAM. */
void grx_gta_init(grx_gta *,const grx_gta_backend *,uint32_t lfb_address);
/* Unsupported/invalid calls are terminal; never silently render a partial frame. */
grx_result grx_gta_call(grx_gta *,const grx_guest_call *,const grx_guest_io *,uint32_t *reply);
#endif
