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
    assert(argc==2);
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
    return 0;
}
