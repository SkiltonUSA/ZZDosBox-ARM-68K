/* Decode the emitted stream independently; no GPU execution claim. */
#include "amiga/dosbox/glide/translator.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <initializer_list>

static uint32_t word(const grx_packet &p, unsigned i) {
    const unsigned char *b=p.bytes+4*i;
    return uint32_t(b[0])<<24 | uint32_t(b[1])<<16 | uint32_t(b[2])<<8 | b[3];
}
static float fp(uint32_t w) {float f; memcpy(&f,&w,4); return f;}
static unsigned descriptor(const grx_packet &p) {
    for(unsigned i=2;i<p.words;) {
        uint32_t w=word(p,i);
        if(w>>28==3) {assert(w==0x3000009c);return i+1;}
        if(w>>28==1)i+=2;
        else if(w>>28==5)i+=1+(w&0x7ffff);
        else {assert(w==0 || w>>28==2);i++;}
    }
    assert(false);return 0;
}
static float attribute(const grx_packet &p,unsigned d,unsigned start,unsigned count,unsigned lane,int x,int y) {
    int bx=word(p,d+1)&65535,by=word(p,d+1)>>16;
    return fp(word(p,d+start+lane))+(x-bx)*fp(word(p,d+start+count+lane))+
           (y-by)*fp(word(p,d+start+2*count+lane));
}
static bool covered(const grx_packet &p,unsigned d,int x,int y) {
    int bx=word(p,d+1)&65535,by=word(p,d+1)>>16;
    for(unsigned i=0;i<3;i++) {
        int64_t edge=int32_t(word(p,d+3+i));
        edge+=int64_t(x-bx)*int32_t(word(p,d+6+i));
        edge+=int64_t(y-by)*int32_t(word(p,d+9+i));
        if(edge<0)return false;
    }
    return true;
}
static void near(float a,float b) {assert(std::fabs(a-b)<0.00002f);}
static grx_config config() {
    grx_config c={};c.width=64;c.height=64;c.color_address=0x06600000;
    c.origin=GRX_LOWER_LEFT;c.blend=GRX_OPAQUE;return c;
}
static void triangle(grx_vertex *v) {
    v[0]={8,8,255,0,0,255,0,0,1};
    v[1]={40,8,0,255,0,255,256,0,1};
    v[2]={8,40,0,0,255,255,0,256,1};
}
int main() {
    grx_packet p={};auto c=config();grx_vertex v[3];triangle(v);
    assert(grx_begin(&p,&c)==GRX_OK);
    assert(grx_triangle(&p,v)==GRX_OK);assert(grx_end(&p)==GRX_OK);
    assert(word(p,0)==(0x90000000u|(p.words-2)*4));assert(word(p,1)==0);
    unsigned d=descriptor(p);
    for(int y=0;y<64;y++)for(int x=0;x<64;x++) {
        bool inside=x>=8&&y>=8&&x+y<=48;
        assert(covered(p,d,x,y)==inside);
        if(inside) {
            near(attribute(p,d,12,4,0,x,y),1-(x-8)/32.f-(y-8)/32.f);
            near(attribute(p,d,12,4,1,x,y),(x-8)/32.f);
            near(attribute(p,d,12,4,2,x,y),(y-8)/32.f);
        }
    }
    puts("PASS triangle coverage and RGB interpolation against analytic reference");
    c.origin=GRX_UPPER_LEFT;
    assert(grx_begin(&p,&c)==GRX_OK);assert(grx_triangle(&p,v)==GRX_OK);
    d=descriptor(p);assert(covered(p,d,16,47));assert(!covered(p,d,16,16));
    near(attribute(p,d,12,4,1,16,47),0.25f);
    c=config();c.texture_address=0x06610000;c.texture_width=64;c.texture_height=32;
    v[1].oow=0.5f;v[1].sow=128;v[2].oow=0.25f;v[2].tow=32;
    assert(grx_begin(&p,&c)==GRX_OK);assert(grx_triangle(&p,v)==GRX_OK);
    d=descriptor(p);
    float q=attribute(p,d,30,3,2,16,16);
    near(q,0.6875f);
    near(attribute(p,d,30,3,0,16,16)/q,0.125f/q);
    near(attribute(p,d,30,3,1,16,16)/q,0.0625f/q);
    puts("PASS origin conversion and rectangular perspective-weighted texture coordinates");
    for(auto blend:{GRX_OPAQUE,GRX_ALPHA,GRX_ADDITIVE}) {
        c.blend=blend;assert(grx_begin(&p,&c)==GRX_OK);
        uint32_t features=0,fragment=0;
        for(unsigned i=2;i<p.words;) {
            uint32_t w=word(p,i);
            if(w>>28==1) {if((w&31)==0)features=word(p,i+1);if((w&31)==3)fragment=word(p,i+1);i+=2;}
            else {assert(w>>28==5);i+=1+(w&0x7ffff);}
        }
        assert((features&2)==(blend==GRX_OPAQUE?0u:2u));
        if(blend==GRX_ALPHA)assert(((fragment>>19)&15)==6&&((fragment>>23)&15)==7);
        if(blend==GRX_ADDITIVE)assert(((fragment>>19)&15)==1&&((fragment>>23)&15)==1);
    }
    c=config();c.depth_test=1;assert(grx_begin(&p,&c)==GRX_UNSUPPORTED);
    c=config();c.color_address=0x067efffe;assert(grx_begin(&p,&c)==GRX_BAD_ARGUMENT);
    c=config();c.texture_address=c.color_address;c.texture_width=64;c.texture_height=64;
    assert(grx_begin(&p,&c)==GRX_BAD_ARGUMENT);
    c.texture_address=0x067ef800;c.texture_width=256;c.texture_height=256;
    assert(grx_begin(&p,&c)==GRX_BAD_ARGUMENT);
    c=config();assert(grx_begin(&p,&c)==GRX_OK);triangle(v);
    unsigned before=p.words;v[0].x=-1;
    assert(grx_triangle(&p,v)==GRX_NEEDS_CLIP&&p.words==before);
    v[0].x=std::numeric_limits<float>::quiet_NaN();
    assert(grx_triangle(&p,v)==GRX_BAD_ARGUMENT&&p.words==before);
    triangle(v);v[2]=v[1];assert(grx_triangle(&p,v)==GRX_EMPTY&&p.words==before);
    triangle(v);unsigned drawn=0;
    while(grx_triangle(&p,v)==GRX_OK)drawn++;
    assert(drawn>1);before=p.words;
    assert(grx_triangle(&p,v)==GRX_FULL&&p.words==before);
    assert(grx_end(&p)==GRX_OK&&p.words<=GRX_PACKET_WORDS);
    assert(grx_triangle(&p,v)==GRX_BAD_STATE);assert(grx_end(&p)==GRX_BAD_STATE);
    puts("PASS blend mapping, unsupported depth, pool bounds, atomic rejection and bounded packets");
}
