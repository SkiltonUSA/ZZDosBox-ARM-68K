/* Internal, validated and viewport-bounded inputs only. */
#ifndef DOSBOX_GRX_DESCRIPTOR_H
#define DOSBOX_GRX_DESCRIPTOR_H
#include "translator.h"
int grx_descriptor(const grx_config *, const grx_vertex *, uint32_t out[39]);
#endif
