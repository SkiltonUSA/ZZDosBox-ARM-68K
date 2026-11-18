/* Glide window-space subset -> XACP RasterIX submit ABI 1.0.
 * No MMIO, device allocation, guest pointers or automatic fallbacks here.
 */
#include "descriptor.h"

#define POOL_START 0x06600000u
#define POOL_END   0x067f0000u
#define PAGE_BYTES 2048u
#define TAIL_WORDS 3u

static void write_word(grx_packet *p,unsigned index,uint32_t v)
{
    for(unsigned b=0;b<4;b++)p->bytes[index*4+b]=(uint8_t)(v>>(24-b*8));
}
static void append(grx_packet *p,uint32_t v) {write_word(p,p->words++,v);}
static void reg(grx_packet *p,unsigned r,uint32_t v) {append(p,0x10000000u|r);append(p,v);}
static int span(uint32_t address,uint32_t size)
{
    return address>=POOL_START&&address<POOL_END&&size<=POOL_END-address;
}
static int power2(uint32_t n) {return n&&n<=256&&!(n&(n-1));}
static unsigned log2size(unsigned n) {unsigned s=0;while(n>1){n>>=1;s++;}return s;}
static grx_result check_config(const grx_config *c)
{
    if(c->depth_test)return GRX_UNSUPPORTED;
    if(!c->width||!c->height||c->width>1024||c->height>768||
       (c->color_address&1)||!span(c->color_address,c->width*c->height*2)||
       (c->origin!=GRX_LOWER_LEFT&&c->origin!=GRX_UPPER_LEFT)||
       (c->blend!=GRX_OPAQUE&&c->blend!=GRX_ALPHA&&c->blend!=GRX_ADDITIVE))return GRX_BAD_ARGUMENT;
    if(!c->texture_address&&!c->texture_width&&!c->texture_height)return GRX_OK;
    if(!power2(c->texture_width)||!power2(c->texture_height)||
       c->texture_width>c->texture_height*8||c->texture_height>c->texture_width*8||
       (c->texture_address&(PAGE_BYTES-1)))return GRX_BAD_ARGUMENT;
    unsigned bytes=(c->texture_width*c->texture_height*2+PAGE_BYTES-1)&~(PAGE_BYTES-1);
    if(!span(c->texture_address,bytes))return GRX_BAD_ARGUMENT;
    if(c->texture_address<c->color_address+c->width*c->height*2&&
       c->color_address<c->texture_address+bytes)return GRX_BAD_ARGUMENT;
    return GRX_OK;
}
grx_result grx_begin(grx_packet *p,const grx_config *c)
{
    if(!p)return GRX_BAD_ARGUMENT;
    p->words=0;p->open=0;
    if(!c)return GRX_BAD_ARGUMENT;
    grx_result result=check_config(c);if(result!=GRX_OK)return result;
    p->config=*c;p->open=1;append(p,0);append(p,0);
    unsigned features=c->texture_address?0x40u:0u,fragment=0x78000u;
    if(c->blend!=GRX_OPAQUE) {
        features|=2;
        fragment|=c->blend==GRX_ALPHA?((6u<<19)|(7u<<23)):((1u<<19)|(1u<<23));
    }
    reg(p,0,features);reg(p,3,fragment);reg(p,8,0);
    reg(p,9,(c->height<<16)|c->width);reg(p,16,c->color_address);
    if(c->texture_address) {
        /* RGB565, nearest, no mipmapping; texture modulates vertex RGBA. */
        reg(p,10,0x00a9c709u);reg(p,11,0);
        reg(p,12,0x2000u|log2size(c->texture_width)|(log2size(c->texture_height)<<4));
        unsigned pages=(c->texture_width*c->texture_height*2+PAGE_BYTES-1)/PAGE_BYTES;
        append(p,0x50000000u|pages);
        for(unsigned i=0;i<pages;i++)append(p,c->texture_address+i*PAGE_BYTES);
    }
    return GRX_OK;
}
static int finite_vertex(const grx_vertex *v)
{
    return __builtin_isfinite(v->x)&&__builtin_isfinite(v->y)&&__builtin_isfinite(v->r)&&__builtin_isfinite(v->g)&&
           __builtin_isfinite(v->b)&&__builtin_isfinite(v->a)&&__builtin_isfinite(v->sow)&&
           __builtin_isfinite(v->tow)&&__builtin_isfinite(v->oow);
}
grx_result grx_triangle(grx_packet *p,const grx_vertex v[3])
{
    if(!p||!p->open)return GRX_BAD_STATE;
    if(!v)return GRX_BAD_ARGUMENT;
    for(unsigned i=0;i<3;i++) {
        if(!finite_vertex(v+i)||v[i].r<0||v[i].r>255||v[i].g<0||v[i].g>255||
           v[i].b<0||v[i].b>255||v[i].a<0||v[i].a>255||v[i].oow<=0)return GRX_BAD_ARGUMENT;
        if(v[i].x<0||v[i].x>p->config.width-1||v[i].y<0||v[i].y>p->config.height-1)return GRX_NEEDS_CLIP;
    }
    uint32_t desc[39];
    if(!grx_descriptor(&p->config,v,desc))return GRX_EMPTY;
    /* Finite inputs can still overflow interpolants for extreme guest values. */
    for(unsigned i=12;i<39;i++)if((desc[i]&0x7f800000u)==0x7f800000u)return GRX_BAD_ARGUMENT;
    if(p->words+40+TAIL_WORDS>GRX_PACKET_WORDS)return GRX_FULL;
    append(p,0x3000009cu);
    for(unsigned i=0;i<39;i++)append(p,desc[i]);
    return GRX_OK;
}
grx_result grx_end(grx_packet *p)
{
    if(!p||!p->open)return GRX_BAD_STATE;
    append(p,0);append(p,0);
    append(p,0x20000011u|((p->config.width*p->config.height)<<8));
    write_word(p,0,0x90000000u|((p->words-2)*4));p->open=0;
    return GRX_OK;
}
