#include "stream.h"
#include <string.h>
_Static_assert(sizeof(grx_stream_request)==64,"one writer per line");
_Static_assert(sizeof(grx_stream_result)==64,"one writer per line");
int grx_stream_open(grx_stream *s,grx_stream_channel *c,const grx_stream_io *io,uint32_t nonce){
 if(s->active||!nonce||!io->pull||!io->push||!io->submit||!io->poll||!io->upload||!io->readback)return 0;
 *s=(grx_stream){.channel=c,.io=*io,.active=1,.nonce=nonce,.next=1};
 memset(&c->request,0,128);io->push(io->context,&c->request,128);return 1;
}
int grx_stream_close(grx_stream *s,uint32_t nonce){
 if(!s->active||s->working||s->failed||s->nonce!=nonce)return 0;
 s->io.pull(s->io.context,&s->channel->request,64);
 if(s->channel->request.sequence==s->next)return 0;
 s->active=0;return 1;
}
static void finish(grx_stream *s,unsigned error){
 grx_stream_result *r=&s->channel->result;
 if(error)s->failed=1;
 *r=(grx_stream_result){.sequence=r->sequence,.nonce=s->nonce,.error=error,.bytes=error?0:s->count};
 s->io.push(s->io.context,r,64);
 r->sequence=s->next;s->io.push(s->io.context,r,64);
 s->next++;s->working=0;
}
static int span(uint32_t a,uint32_t n){return n&&n<=GRX_STREAM_BYTES&&a>=GRX_STREAM_BASE&&a<GRX_STREAM_END&&n<=GRX_STREAM_END-a;}
void grx_stream_poll(grx_stream *s){
 if(!s->active||s->failed||!s->next)return;
 if(s->working){int r=s->io.poll(s->io.context);if(r)finish(s,r>0?GRX_STREAM_OK:GRX_STREAM_GPU);return;}
 grx_stream_channel *c=s->channel;grx_stream_io *io=&s->io;
 io->pull(io->context,&c->request,64);grx_stream_request q=c->request;
 if(q.sequence!=s->next)return;
 s->count=q.bytes;
 if(q.nonce!=s->nonce||q.op<1||q.op>3||!q.bytes||q.bytes>GRX_STREAM_BYTES){finish(s,GRX_STREAM_REQUEST);return;}
 if(q.op==GRX_STREAM_SUBMIT){
  if(q.address||q.bytes<12||q.bytes>505*4||(q.bytes&3)){finish(s,GRX_STREAM_REQUEST);return;}
  io->pull(io->context,c->data,(q.bytes+63)&~63u);
  if(!io->submit(io->context,c->data,q.bytes,s->nonce,s->next)){finish(s,GRX_STREAM_GPU);return;}
  s->working=1;return;
 }
 if(!span(q.address,q.bytes)){finish(s,GRX_STREAM_REQUEST);return;}
 int ok;
 if(q.op==GRX_STREAM_UPLOAD){
  io->pull(io->context,c->data,(q.bytes+63)&~63u);
  ok=io->upload(io->context,q.address,c->data,q.bytes);
 }else{
  ok=io->readback(io->context,q.address,c->data,q.bytes);
  if(ok)io->push(io->context,c->data,(q.bytes+63)&~63u);
 }
 finish(s,ok?GRX_STREAM_OK:GRX_STREAM_IO);
}
