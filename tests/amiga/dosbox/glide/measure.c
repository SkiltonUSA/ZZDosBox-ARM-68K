#include "amiga/dosbox/glide/experimental/measure.h"
#include <assert.h>
#include <stdio.h>
struct fixture { uint64_t now; unsigned submitted,verified,valid,fault;
    uint32_t epoch,sequence; unsigned submit_fail,pixel_fail,packets; };
static int submit(void *p,unsigned scene,uint32_t epoch,uint32_t sequence)
{
    struct fixture *f=p;
    assert(scene==(f->submitted<5?f->submitted:(f->packets?5+(f->submitted-5)%f->packets:1)));
    f->submitted++; f->epoch=epoch; f->sequence=sequence;
    return !f->submit_fail;
}
static int verify(void *p,unsigned scene)
{
    struct fixture *f=p; assert(scene==(f->verified<5?f->verified:(f->packets?4+f->packets:1)));
    f->verified++; return !f->pixel_fail;
}
static uint64_t ticks(void *p) { return ((struct fixture*)p)->now; }
static int read_word(void *p,unsigned offset,uint32_t *value)
{
    struct fixture *f=p;
    if(offset==0)*value=f->valid|(f->fault?4:0);
    else if(offset==4)*value=f->epoch;
    else if(offset==8)*value=f->sequence;
    else assert(0);
    return 1;
}
static int write_word(void *p,unsigned offset,uint32_t value)
{
    struct fixture *f=p;
    if(offset==12)assert(value==f->epoch);
    else if(offset==16)assert(value==f->sequence);
    else { assert(offset==20&&value==1);f->valid=0; }
    return 1;
}
static grx_measure_io io(struct fixture *f)
{ return (grx_measure_io){f,submit,verify,ticks,{f,read_word,write_word}}; }
int main(void)
{
    struct fixture f={0};grx_measure m={0};grx_measure_io i=io(&f);
    assert(!grx_measure_begin(&m,0,10,100));
    assert(!grx_measure_begin(&m,1,1001,100));
    assert(grx_measure_begin(&m,1,10,100));
    assert(!grx_measure_begin(&m,2,10,100));
    for(unsigned n=0;n<15;n++) {
        f.now=n*10;grx_measure_poll(&m,&i);
        assert(m.waiting && m.frames==(n<5?0:n-5));
        f.now++;grx_measure_poll(&m,&i); /* FIFO submission alone earns no frame. */
        assert(m.frames==(n<5?0:n-5));
        f.valid=1;f.now+=3;grx_measure_poll(&m,&i);
    }
    assert(m.state==GRX_MEASURE_DONE&&m.frames==10&&m.elapsed==94&&f.verified==6);
    grx_measure_poll(&m,&i);assert(f.submitted==15);
    for(unsigned fault=0;fault<6;fault++) {
        f=(struct fixture){0};m=(grx_measure){0};i=io(&f);
        assert(grx_measure_begin(&m,2,10,100));
        f.submit_fail=fault==0;
        grx_measure_poll(&m,&i);
        if(fault==1)f.fault=1;
        if(fault==2)f.now=100;
        if(fault==3){f.valid=1;f.pixel_fail=1;}
        if(fault==4){f.valid=1;f.sequence++;}
        if(fault==5)f.valid=9; /* Cancellation is not a rendered frame. */
        grx_measure_poll(&m,&i);
        assert(m.state==GRX_MEASURE_FAULT&&m.frames==0);
        unsigned submissions=f.submitted;grx_measure_poll(&m,&i);
        assert(f.submitted==submissions&&!grx_measure_begin(&m,3,1,100));
    }
    f=(struct fixture){.packets=65};m=(grx_measure){0};i=io(&f);
    assert(grx_measure_begin_batches(&m,3,3,100,65));
    for(unsigned n=0;n<200;n++) {
        f.now=n*10;grx_measure_poll(&m,&i);
        unsigned completed=n<5?0:(n-5)/65;
        assert(m.frames==completed);
        f.now++;grx_measure_poll(&m,&i);assert(m.frames==completed);
        f.valid=1;f.now+=3;grx_measure_poll(&m,&i);
        assert(m.frames==(n<5?0:(n-4)/65));
    }
    assert(m.state==GRX_MEASURE_DONE&&m.frames==3&&m.elapsed==1944&&f.verified==6);
    puts("PASS multi-packet frames: 65 retirements per frame, no packet-as-frame counting");
    puts("PASS completion-driven counter: warmup, delayed retirement, elapsed ticks, terminal faults");
}
