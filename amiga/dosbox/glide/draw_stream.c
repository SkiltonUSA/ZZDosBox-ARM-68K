/* GPL-2.0-or-later. Ordered guest triangle batches with a terminal transport fault. */
#include "draw_stream.h"
#include "guest_calls.h"
#include "guest_vertex.h"
#include "clip.h"

grx_result grx_draw_open(grx_draw_stream *s,const grx_config *c,
                         grx_draw_submit submit,void *context)
{
    *s=(grx_draw_stream){.submit=submit,.context=context,.failed=1};
    if(!submit)return GRX_BAD_ARGUMENT;
    grx_result rc=grx_begin(&s->packet,c);
    if(rc==GRX_OK)s->failed=0;
    return rc;
}

grx_result grx_draw_flush(grx_draw_stream *s)
{
    if(s->failed)return GRX_BAD_STATE;
    if(!s->pending)return GRX_EMPTY;
    grx_config config=s->packet.config;
    /* Mark terminal before the callback: a rejected or indeterminate submission
     * must never be retried or followed by a resource change. */
    s->failed=1;
    grx_result rc=grx_end(&s->packet);
    if(rc!=GRX_OK)return rc;
    if(!s->submit(s->context,&s->packet))return GRX_BAD_STATE;
    rc=grx_begin(&s->packet,&config);
    if(rc==GRX_OK){s->pending=0;s->failed=0;}
    return rc;
}

grx_result grx_draw_configure(grx_draw_stream *s,const grx_config *config)
{
    if(s->failed)return GRX_BAD_STATE;
    grx_packet next;
    grx_result rc=grx_begin(&next,config);
    if(rc!=GRX_OK)return rc;
    rc=grx_draw_flush(s);
    if(rc!=GRX_OK&&rc!=GRX_EMPTY)return rc;
    s->packet=next;
    return GRX_OK;
}

static grx_result append_triangle(grx_draw_stream *s,const grx_vertex tri[3])
{
    grx_result rc=grx_triangle(&s->packet,tri);
    if(rc==GRX_FULL){
        rc=grx_draw_flush(s);
        if(rc==GRX_OK)rc=grx_triangle(&s->packet,tri);
    }
    if(rc==GRX_OK)++s->pending;
    else if(rc!=GRX_EMPTY)s->failed=1;
    return rc;
}

grx_result grx_draw_call(grx_draw_stream *s,const grx_guest_call *call,
                         const grx_guest_io *io,int tmu_w)
{
    if(s->failed)return GRX_BAD_STATE;
    if(call->id!=GRX_CALL_grDrawTriangle)return GRX_UNSUPPORTED;
    if(call->count!=3)return GRX_BAD_ARGUMENT;
    grx_vertex tri[3];
    if(!grx_guest_vertices(tri,call->args,tmu_w,io))return GRX_BAD_ARGUMENT;
    return grx_draw_vertices(s,tri);
}

grx_result grx_draw_vertices(grx_draw_stream *s,const grx_vertex tri[3])
{
    if(s->failed)return GRX_BAD_STATE;
    grx_vertex polygon[GRX_CLIP_VERTICES];unsigned count;
    grx_result rc=grx_clip_triangle(tri,s->packet.config.width,
                                  s->packet.config.height,polygon,&count);
    if(rc!=GRX_OK)return rc;
    unsigned emitted=0;
    for(unsigned i=1;i+1<count;i++){
        grx_vertex fan[3]={polygon[0],polygon[i],polygon[i+1]};
        rc=append_triangle(s,fan);
        if(rc==GRX_OK)++emitted;
        else if(rc!=GRX_EMPTY)return rc;
    }
    return emitted?GRX_OK:GRX_EMPTY;
}
