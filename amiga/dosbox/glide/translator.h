/* Experimental command encoder, not a Glide DLL/OVL or a GPU driver. */
#ifndef DOSBOX_GRX_TRANSLATOR_H
#define DOSBOX_GRX_TRANSLATOR_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Half the 512-word FIFO, including envelope and commit. */
#define GRX_PACKET_WORDS 248u
typedef enum { GRX_OK, GRX_EMPTY, GRX_FULL, GRX_NEEDS_CLIP,
               GRX_UNSUPPORTED, GRX_BAD_ARGUMENT, GRX_BAD_STATE } grx_result;
typedef enum { GRX_LOWER_LEFT, GRX_UPPER_LEFT } grx_origin;
typedef enum { GRX_OPAQUE, GRX_ALPHA, GRX_ADDITIVE } grx_blend;
typedef struct {
    uint32_t width, height, color_address;
    /* One RGB565 mip level, contiguous 2 KiB pages, already uploaded.
       Zero dimensions/address mean untextured. No allocation is performed. */
    uint32_t texture_width, texture_height, texture_address;
    uint32_t origin; /* grx_origin; fixed storage across ARM enum ABIs. */
    uint32_t blend;  /* grx_blend */
    unsigned depth_test; /* ABI 1.0: any nonzero value returns UNSUPPORTED. */
    /* Zero preserves the original RGB565/nearest path. */
    unsigned texture_options; /* bit0 RGBA5551, bit1 linear min, bit2 linear mag */
    unsigned alpha_gt128; /* GTA subset: reject alpha <= 128; 0 disables */
} grx_config;
typedef struct {
    float x,y,r,g,b,a;  /* Window pixels and Glide 0..255 colours. */
    float sow,tow,oow; /* TMU0 s/w, t/w, 1/w; NOT unweighted s,t. */
} grx_vertex;
typedef struct {
    uint8_t bytes[GRX_PACKET_WORDS*4]; /* Big-endian XACP staging bytes. */
    uint32_t words;
    grx_config config;
    unsigned open;
} grx_packet;

/* Begin invalidates any old packet, including on error. Use QUERY before
   hardware integration; these functions do not access hardware. */
grx_result grx_begin(grx_packet *, const grx_config *);
/* Rejected/empty/full triangles append nothing. Clipping is not implemented. */
grx_result grx_triangle(grx_packet *, const grx_vertex vertices[3]);
/* Finalise a stream. COMMIT and XACP DONE are NOT GPU completion fences.
   A transport must fence GPU completion before reusing textures/targets. */
grx_result grx_end(grx_packet *);

#ifdef __cplusplus
}
#endif
#endif
