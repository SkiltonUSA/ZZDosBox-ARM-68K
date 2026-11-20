#include "measure.h"

int grx_measure_begin(grx_measure *m, uint32_t epoch, unsigned frames, uint64_t timeout)
{ return grx_measure_begin_batches(m,epoch,frames,timeout,1); }

int grx_measure_begin_batches(grx_measure *m,uint32_t epoch,unsigned frames,uint64_t timeout,unsigned packets)
{
    if (m->state!=GRX_MEASURE_IDLE || !epoch || !frames || frames>1000 || !timeout || !packets || packets>65)
        return 0;
    *m=(grx_measure){.state=GRX_MEASURE_CHECK, .target=frames, .timeout=timeout,
                   .packets_per_frame=packets,.batch={.epoch=epoch}};
    return 1;
}

static void fail(grx_measure *m, enum grx_measure_error error)
{ m->error=error; m->state=GRX_MEASURE_FAULT; }

void grx_measure_poll(grx_measure *m, const grx_measure_io *io)
{
    if (m->state!=GRX_MEASURE_CHECK && m->state!=GRX_MEASURE_RUN) return;
    if (!m->waiting) {
        uint64_t now=io->ticks(io->context);
        if (m->state==GRX_MEASURE_RUN && !m->frames && !m->packet_index) m->started=now;
        ++m->batch.sequence;
        m->batch.state=GRX_COMPLETION_PENDING;
        m->waiting=1;
        m->deadline=now+m->timeout;
        if (!io->submit(io->context,m->scene,m->batch.epoch,m->batch.sequence))
            fail(m,GRX_MEASURE_SUBMIT);
        return;
    }
    grx_completion_state result=grx_completion_poll(&io->completion,&m->batch);
    uint64_t now=io->ticks(io->context);
    if (result==GRX_COMPLETION_PENDING) {
        if (now>=m->deadline) fail(m,GRX_MEASURE_TIMEOUT);
        return;
    }
    if (result!=GRX_COMPLETION_RETIRED) { fail(m,GRX_MEASURE_RETIRE); return; }
    m->waiting=0;
    if (m->state==GRX_MEASURE_CHECK) {
        if (!io->verify(io->context,m->scene)) { fail(m,GRX_MEASURE_PIXELS); return; }
        if (++m->scene==5) { m->scene=m->packets_per_frame==1?1:5; m->state=GRX_MEASURE_RUN; }
    } else {
        if (++m->packet_index<m->packets_per_frame) { ++m->scene; return; }
        m->packet_index=0;
        ++m->frames;
        m->elapsed=now-m->started;
        if (m->frames==m->target) {
            if (!io->verify(io->context,m->scene)) fail(m,GRX_MEASURE_PIXELS);
            else m->state=GRX_MEASURE_DONE;
        } else m->scene=m->packets_per_frame==1?1:5;
    }
}
