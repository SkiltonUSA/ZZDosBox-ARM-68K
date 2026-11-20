#ifndef GRX_MEASURE_H
#define GRX_MEASURE_H
#include "completion_client.h"

enum grx_measure_state { GRX_MEASURE_IDLE, GRX_MEASURE_CHECK,
    GRX_MEASURE_RUN, GRX_MEASURE_DONE, GRX_MEASURE_FAULT };
enum grx_measure_error { GRX_MEASURE_OK, GRX_MEASURE_SUBMIT,
    GRX_MEASURE_RETIRE, GRX_MEASURE_TIMEOUT, GRX_MEASURE_PIXELS };
typedef struct {
    void *context;
    int (*submit)(void *, unsigned scene, uint32_t epoch, uint32_t sequence);
    int (*verify)(void *, unsigned scene);
    uint64_t (*ticks)(void *);
    grx_completion_io completion;
} grx_measure_io;
typedef struct {
    enum grx_measure_state state;
    enum grx_measure_error error;
    grx_completion_batch batch;
    unsigned target, frames, scene, waiting, packets_per_frame, packet_index;
    uint64_t started, elapsed, deadline, timeout;
} grx_measure;

/* Caller establishes exclusive GPU ownership and a verified empty endpoint.
 * Five untimed correctness scenes precede repeated solid-triangle frames.
 * Ticks are raw CPU global-timer ticks: calibrate separately, never infer FPS
 * from the FPGA clock. Faulted measurements retain ownership until recovery. */
int grx_measure_begin(grx_measure *, uint32_t epoch, unsigned frames, uint64_t timeout);
/* Same five correctness scenes, then scenes 5..5+packets-1 form one frame. */
int grx_measure_begin_batches(grx_measure *,uint32_t,unsigned,uint64_t,unsigned packets);
void grx_measure_poll(grx_measure *, const grx_measure_io *);
#endif
