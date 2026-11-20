/* Existing physical scenes, reused for full-renderer simulation. */
#include "amiga/dosbox/glide/probe_readback.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc==1 || (argc==2 && !strcmp(argv[1],"--ordered-state"))) {
        int ordered=argc==2;
        unsigned offset=0;
        for(unsigned scene=0;scene<5;scene++) {
            grx_packet p;
            if(!grx_probe_scene(&p,scene,0x4120+scene))return 1;
            fprintf(stderr,"%u %u\n",offset,p.words+7+ordered);offset+=p.words+7+ordered;
            for(unsigned i=0;i<p.words;i++) {
                if(ordered && i==p.words-13)puts("60000000");
                unsigned v=0;for(unsigned b=0;b<4;b++)v=(v<<8)|p.bytes[i*4+b];
                if(ordered && i==0)v+=4;
                printf("%08x\n",v);
            }
            /* Separate fence and ordered response envelopes. Offline only. */
            printf("90000004\n00000000\n60000000\n50000008\n00000000\nabcddcba\n%08x\n",scene+1);
        }
        return 0;
    }
    if(argc!=3)return 2;
    unsigned scene=(unsigned)strtoul(argv[1],0,10),value,checked;
    if(scene>=5)return 2;
    uint16_t pixels[GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT];
    FILE *f=fopen(argv[2],"r");if(!f)return 3;
    for(unsigned i=0;i<sizeof(pixels)/sizeof(*pixels);i++) {
        if(fscanf(f,"%x",&value)!=1 || value>65535)return 4;
        pixels[i]=(uint16_t)value;
    }
    fclose(f);
    struct grx_readback_trace trace;
    unsigned bad=grx_probe_verify_trace(pixels,scene,&checked,&trace);
    printf("PIXELS scene=%u checked=%u bad=%u\n",scene,checked,bad);
    for(unsigned i=0;i<trace.count;i++)printf("BAD offset=%u got=%04x want=%04x\n",trace.samples[i].offset,trace.samples[i].got,trace.samples[i].want);
    return bad?5:0;
}
