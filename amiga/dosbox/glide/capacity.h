/* Synthetic translator workload; no GPU or guest FPS measurement. */
#pragma once
#include <stdint.h>
#define GC_BATCHES 64u
#define GC_TRIANGLES 320u
struct gc_result { uint32_t triangles, bytes, hash, ticks; };
int grx_capacity(struct gc_result *, int (*stop)(void), uint32_t (*clock)(void));
