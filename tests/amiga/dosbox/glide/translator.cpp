/* Decode the emitted stream independently; no GPU execution claim. */
#include "amiga/dosbox/glide/translator.h"
#include "amiga/dosbox/glide/probe_scene.h"
#include "amiga/dosbox/glide/probe_wire.h"
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
    assert(grx_probe_crc32((const uint8_t*)"123456789",9)==0xcbf43926);
    assert(grx_probe_crc32((const uint8_t*)"",0)==0);
    grx_packet tagged;
    assert(grx_probe_scene(&tagged,1,0x4121));
    auto original=tagged;
    assert(grx_probe_retag(&tagged,0xa619));
    uint32_t rgba=word(tagged,tagged.words-2);
    assert((((rgba>>16)&0xf800)|((rgba>>13)&0x7e0)|((rgba>>11)&31))==0xa619);
    for(unsigned i=0;i<tagged.words;i++)if(i!=tagged.words-2)assert(word(tagged,i)==word(original,i));
    assert(grx_probe_triangle_marker(&tagged,1));original=tagged;
    assert(!grx_probe_retag(&tagged,0xa619));
    assert(!memcmp(&tagged,&original,sizeof(tagged)));
    puts("PASS marker retagging: only clear colour changes; standard CRC32 check vectors");

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
    for(unsigned scene=0;scene<GRX_PROBE_SCENES;scene++) {
        assert(grx_probe_scene(&p,scene,0x4120+scene));
        assert(!p.open&&p.words<=GRX_PACKET_WORDS);
        assert(word(p,0)==(0x90000000u|(p.words-2)*4));
        unsigned tail=p.words-13;
        assert(word(p,tail)==0&&word(p,tail+1)==0);
        unsigned features=0x20|(scene==3?0x40:0)|((scene==2||scene==4)?2:0);
        assert(word(p,tail+2)==0x10000000&&word(p,tail+3)==features);
        unsigned frag=0x78000|(scene==2?((1u<<19)|(1u<<23)):scene==4?((6u<<19)|(7u<<23)):0);
        assert(word(p,tail+4)==0x10000003&&word(p,tail+5)==frag);
        assert(word(p,tail+6)==0x10000006&&word(p,tail+7)==0);
        assert(word(p,tail+8)==0x10000007&&word(p,tail+9)==0x10040);
        uint32_t rgba=word(p,tail+11);
        unsigned marker=((rgba>>16)&0xf800)|((rgba>>13)&0x7e0)|((rgba>>11)&31);
        assert(marker==0x4120+scene);
        assert(word(p,tail+12)==(0x20000012u|(GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT<<8)));
    }
    for(unsigned scene=1;scene<GRX_PROBE_SCENES;scene++) {
        assert(grx_probe_triangle_marker(&p,scene));
        assert(!p.open&&p.words<=GRX_PACKET_WORDS);
        // Last draw is a marker triangle. No register writes or clears follow it.
        unsigned tail=p.words-42;
        assert((word(p,tail)>>28)==3);
        for(int x=4;x<=28;x++) {
            assert(covered(p,tail+1,x,2));
            near(attribute(p,tail+1,12,4,3,x,2),1.0f);
        }
        for(int y=8;y<72;y++)assert(!covered(p,tail+1,4,y));
        assert(word(p,p.words-2)==0&&word(p,p.words-1)==0);
        assert(word(p,0)==(0x90000000u|(p.words-2)*4));
    }
    assert(!grx_probe_scene(&p,5,1)&&!grx_probe_scene(&p,0,0)&&!grx_probe_scene(&p,0,65536));
    puts("PASS probe fixtures: bounded packets, mask restoration, scissor row and marker encoding");
}
