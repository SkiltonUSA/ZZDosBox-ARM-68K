/* Native-only ABI survey. No rasterizer, GPU transport or FPS measurement. */
#ifndef GRX_SURVEY_H
#define GRX_SURVEY_H
#include "guest.h"
typedef struct { unsigned width,height,next_texture,open; uint32_t lfb_address;unsigned retrace; } grx_survey;
uint32_t grx_survey_call(grx_survey *,const grx_guest_call *,const grx_guest_io *);
#endif
