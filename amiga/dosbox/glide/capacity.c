#include "capacity.h"
#include "translator.h"
/* Retain every output for verification outside the timed interval. */
static struct { uint32_t before; grx_packet packet; uint32_t after; } output[GC_BATCHES];
static grx_vertex vertices[GC_BATCHES][5][3];
int grx_capacity(struct gc_result *result,int (*stop)(void),uint32_t (*clock)(void)) {
    result->triangles=0;result->bytes=0;result->hash=2166136261u;result->ticks=0;
    for(unsigned b=0;b<GC_BATCHES;b++) {
        output[b].before=0x1234cafe;output[b].after=0x7654beef;
        for(unsigned t=0;t<5;t++) {
            float x=(float)(8+(b%8)*64+t),y=(float)(8+(b/8)*48+t);
            const grx_vertex v[3]={
                {x,y,255,64,32,255,0,0,1},
                {x+48,y,64,255,32,128,128,0,.5f},
                {x,y+32,32,64,255,64,0,64,.25f}};
            for(unsigned n=0;n<3;n++)vertices[b][t][n]=v[n];
        }
    }
    uint32_t begin=clock?clock():0;
    for(unsigned b=0;b<GC_BATCHES;b++) {
        if(stop && stop())return 2;
        grx_config c={640,480,0x06600000,64,32,0x06700000,b%2,b%3,0};
        grx_packet *p=&output[b].packet;
        if(grx_begin(p,&c)!=GRX_OK)return 1;
        for(unsigned t=0;t<5;t++)if(grx_triangle(p,vertices[b][t])!=GRX_OK)return 1;
        if(grx_end(p)!=GRX_OK)return 1;
    }
    result->ticks=clock?clock()-begin:0;
    for(unsigned b=0;b<GC_BATCHES;b++) {
        const grx_packet *p=&output[b].packet;
        if(output[b].before!=0x1234cafe || output[b].after!=0x7654beef ||
           p->words!=224 || p->open)return 1;
        for(unsigned i=0;i<p->words*4;i++)result->hash=(result->hash^p->bytes[i])*16777619u;
        result->triangles+=5;result->bytes+=p->words*4;
    }
    return 0;
}
