/* Included after the real firmware validator, not a reimplementation. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "amiga/dosbox/glide/probe_scene.h"

static uint32_t *mutate_source;
static int audited_validate(const volatile uint32_t *src, uint32_t count,
                           XACPGPU_SafeState *next, uint32_t *bad, uint32_t *detail)
{
    if (mutate_source) mutate_source[2]=be32(0x60000000);
    return xacpgpu_validate_submit(src,count,next,bad,detail);
}
#define xacpgpu_validate_submit audited_validate
#include "amiga/dosbox/glide/experimental/submit_prepare.inc"
#undef xacpgpu_validate_submit
_Static_assert(GRX_TAGGED_FIFO_WORDS==GRX_BOARD_FIFO_WORDS,"FIFO size differs from board");

static struct {
    uint32_t before;
    GRX_TaggedSubmission packet;
    uint32_t after;
} storage;
static uint32_t source[XACPGPU_SUBMIT_MAX_WORDS];

int main(int argc,char **argv)
{
    assert(argc==4);
    uint32_t bad,detail;
    XACPGPU_SafeState previous=xacpgpu_safe_state;
    GRX_TaggedSubmission *p=&storage.packet;
    storage.before=0x12345678; storage.after=0xabcdef01;
    FILE *f=fopen(argv[1],"w"); assert(f);
    for(unsigned scene=0;scene<5;scene++) {
        grx_packet input; assert(grx_probe_scene(&input,scene,0x4120+scene));
        memcpy(source,input.bytes,input.words*4);
        assert(grx_prepare_tagged(p,source,input.words,0xabcddcba,scene+1,&bad,&detail)==0);
        assert(p->words==input.words+7);
        assert(!memcmp(p->wire,input.bytes,input.words*4));
        assert(p->next_state.color_base==GRX_PROBE_COLOR);
        for(unsigned i=0;i<p->words;i++) fprintf(f,"%08x\n",be32(p->wire[i]));
    }
    assert(!fclose(f));
    puts("PASS tagged submission: five accepted scene packets");

    /* Renderer fixtures: subject/marker/commit, then state/commit. A single
     * fence must wait for both commits, with the original pixel oracle. */
    f=fopen(argv[2],"w");assert(f);
    FILE *sizes=fopen(argv[3],"w");assert(sizes);unsigned offset=0;
    for(unsigned scene=0;scene<5;scene++){
        grx_packet a,b;assert(grx_probe_scene(&a,scene,0x4120+scene));
        uint32_t commit=0x20000011u|((GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT)<<8);
        for(unsigned n=0;n<4;n++)a.bytes[a.words*4+n]=commit>>(24-8*n);
        a.words++;uint32_t header=0x90000000u|((a.words-2)*4);
        for(unsigned n=0;n<4;n++)a.bytes[n]=header>>(24-8*n);
        assert(grx_begin(&b,&a.config)==GRX_OK&&grx_end(&b)==GRX_OK);
        assert(!grx_prepare_tagged_pair(p,(uint32_t*)a.bytes,a.words,
            (uint32_t*)b.bytes,b.words,0xabcddcba,scene+1,&bad,&detail));
        fprintf(sizes,"starts[%u]=%u;sizes[%u]=%u;\n",scene,offset,scene,p->words);offset+=p->words;
        for(unsigned i=0;i<p->words;i++)fprintf(f,"%08x\n",be32(p->wire[i]));
    }
    assert(!fclose(f)&&!fclose(sizes));

    /* Boundary-sized NOP packets. Error must unpublish an earlier packet. */
    memset(source,0,sizeof(source));
    unsigned largest=GRX_TAGGED_FIFO_WORDS-7;
    source[0]=be32(0x90000000|((largest-2)*4));
    assert(!grx_prepare_tagged(p,source,largest,~0u,~0u,&bad,&detail));
    assert(p->words==GRX_TAGGED_FIFO_WORDS);
    assert(be32(p->wire[p->words-1])==~0u);
    unsigned invalid[]={0,1,2,largest+1,XACPGPU_SUBMIT_MAX_WORDS,~0u};
    for(unsigned i=0;i<sizeof(invalid)/sizeof(*invalid);i++) {
        p->words=123;
        assert(grx_prepare_tagged(p,source,invalid[i],1,2,&bad,&detail)==0x4920);
        assert(p->words==0);
    }
    assert(storage.before==0x12345678 && storage.after==0xabcdef01);
    puts("PASS tagged submission: reserved tail capacity, invalid sizes and guards");

    source[0]=be32(0x90000004); source[1]=0; source[2]=0;
    mutate_source=source;
    assert(!grx_prepare_tagged(p,source,3,7,8,&bad,&detail));
    mutate_source=NULL;
    assert(p->wire[2]==0 && be32(source[2])==0x60000000);
    puts("PASS tagged submission: validation and output use private snapshot");

    /* A client cannot supply the privileged fence/echo tail through ABI 1.0. */
    assert(grx_prepare_tagged(p,source,3,7,8,&bad,&detail)==0x4980);
    assert(p->words==0 && bad==2);
    source[0]=be32(0x50000008); source[1]=0; source[2]=7; source[3]=8;
    assert(grx_prepare_tagged(p,source,4,7,8,&bad,&detail)==0x4921);
    assert(p->words==0);
    source[0]=be32(0x90000004); source[2]=0;
    source[3]=be32(0x50000008); source[4]=0; source[5]=7; source[6]=8;
    assert(grx_prepare_tagged(p,source,7,7,8,&bad,&detail)==0x4921);
    assert(p->words==0);
    source[0]=be32(0x90000010); source[2]=be32(0x50000008);
    source[3]=0; source[4]=7; source[5]=8;
    /* Inside the render envelope, opcode 5 means texture pages, not DMA echo. */
    assert(grx_prepare_tagged(p,source,6,7,8,&bad,&detail)==0x4971);
    assert(p->words==0);
    assert(!memcmp(&previous,&xacpgpu_safe_state,sizeof(previous)));
    puts("PASS tagged submission: client fence/echo attempts rejected; live state unchanged");

    /* Joining preserves every render command, including both commits. Only
     * the duplicate envelope and first completion suffix disappear. */
    grx_packet left,right;
    assert(grx_probe_scene(&left,0,0x4120));
    assert(grx_probe_scene(&right,1,0x4121));
    assert(!grx_prepare_tagged_pair(p,(uint32_t*)left.bytes,left.words,
        (uint32_t*)right.bytes,right.words,7,8,&bad,&detail));
    unsigned joined=left.words+right.words-2;
    assert(p->words==joined+7);
    assert(be32(p->wire[0])==(0x90000000u|((joined-2)*4)));
    assert(!memcmp(p->wire+2,left.bytes+8,(left.words-2)*4));
    assert(!memcmp(p->wire+left.words,right.bytes+8,(right.words-2)*4));
    assert(be32(p->wire[joined+2])==0x60000000);
    assert(be32(p->wire[joined+5])==7 && be32(p->wire[joined+6])==8);
    right.bytes[0]=0x50;
    assert(grx_prepare_tagged_pair(p,(uint32_t*)left.bytes,left.words,
        (uint32_t*)right.bytes,right.words,7,8,&bad,&detail)==0x4921 && !p->words);
    assert(grx_probe_scene(&right,1,0x4121));right.bytes[7]=1;
    assert(grx_prepare_tagged_pair(p,(uint32_t*)left.bytes,left.words,
        (uint32_t*)right.bytes,right.words,7,8,&bad,&detail)==0x4922 && !p->words);
    memset(source,0,sizeof(source));source[0]=be32(0x90000000u|((252-2)*4));
    assert(!grx_prepare_tagged_pair(p,source,252,source,252,7,8,&bad,&detail));
    assert(p->words==509);
    assert(grx_prepare_tagged_pair(p,source,252,source,256,7,8,&bad,&detail)==0x4920 && !p->words);
    assert(grx_prepare_tagged_pair(p,source,~0u,source,3,7,8,&bad,&detail)==0x4920 && !p->words);
    assert(grx_prepare_tagged_pair(p,source,2,source,3,7,8,&bad,&detail)==0x4920 && !p->words);
    source[0]=be32(0x90000004);source[2]=be32(0x60000000);
    assert(grx_prepare_tagged_pair(p,(uint32_t*)left.bytes,left.words,
        source,3,7,8,&bad,&detail)==0x4980 && !p->words);
    assert(storage.before==0x12345678 && storage.after==0xabcdef01);
    assert(!memcmp(&previous,&xacpgpu_safe_state,sizeof(previous)));
    puts("PASS paired submission: command order, commits, one fence, bounds and invalid input");
    return 0;
}
