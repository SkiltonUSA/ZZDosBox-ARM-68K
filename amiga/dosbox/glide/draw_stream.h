/* GPL-2.0-or-later. Guest triangle batching; no device or display access. */
#ifndef GRX_DRAW_STREAM_H
#define GRX_DRAW_STREAM_H
#include "guest.h"
#include "translator.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Success means GPU retirement, not FIFO acceptance. A callback may instead
 * copy packets to an offline recorder. Bytes are borrowed only during the call.
 * On failure no further submission is permitted; the transport owns recovery. */
typedef int (*grx_draw_submit)(void *,const grx_packet *);
typedef struct {
    grx_packet packet;
    grx_draw_submit submit;
    void *context;
    unsigned pending,failed;
} grx_draw_stream;
/* Initialize only for a new, idle transport; never reset a failed live GPU. */
grx_result grx_draw_open(grx_draw_stream *,const grx_config *,grx_draw_submit,void *);
grx_result grx_draw_flush(grx_draw_stream *);
/* Validate new state before flushing old state. Textures must already exist. */
grx_result grx_draw_configure(grx_draw_stream *,const grx_config *);
/* Only grDrawTriangle is supported. Does not advertise a board or implement swap.
 * Guest decoding or clipping errors append nothing. An encoding/transport error after fan
 * emission starts makes the stream terminal; the caller must stop the session. */
grx_result grx_draw_vertices(grx_draw_stream *,const grx_vertex [3]);
grx_result grx_draw_call(grx_draw_stream *,const grx_guest_call *,const grx_guest_io *,int tmu_w);
#ifdef __cplusplus
}
#endif
#endif
