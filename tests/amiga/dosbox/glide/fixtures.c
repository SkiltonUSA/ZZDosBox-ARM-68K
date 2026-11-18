/* Generate wire fixtures without requiring floating point on the 68k host. */
#include "amiga/dosbox/glide/probe_scene.h"
#include <stdio.h>
static int emit(unsigned mode)
{
    unsigned sizes[GRX_PROBE_SCENES];
    printf("static const unsigned char grx_probe_bytes%u[%u][%u]={\n",mode,GRX_PROBE_SCENES,GRX_PACKET_WORDS*4);
    for(unsigned scene=0;scene<GRX_PROBE_SCENES;scene++) {
        grx_packet p;
        if(!(mode?grx_probe_triangle_marker(&p,scene):grx_probe_scene(&p,scene,0x4120+scene)))return 0;
        sizes[scene]=p.words;putchar('{');
        for(unsigned i=0;i<p.words*4;i++)printf("%u,",p.bytes[i]);
        puts("},");
    }
    printf("};\nstatic const unsigned grx_probe_words%u[]={\n",mode);
    for(unsigned i=0;i<GRX_PROBE_SCENES;i++)printf("%u,",sizes[i]);
    puts("};");return 1;
}
int main(void)
{
    puts("/* Generated fixtures: do not edit. */");
    return emit(0)&&emit(1)?0:1;
}
