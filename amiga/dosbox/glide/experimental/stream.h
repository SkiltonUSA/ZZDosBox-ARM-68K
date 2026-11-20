/* Private Core1/Core0 GPU transport. One outstanding request, separate writers. */
#ifndef GRX_STREAM_H
#define GRX_STREAM_H
#include <stdint.h>
#define GRX_STREAM_MAGIC 0x47525331u
#define GRX_STREAM_BYTES 32768u
#define GRX_STREAM_BASE 0x06600000u
#define GRX_STREAM_END 0x067f0000u
enum {GRX_STREAM_SUBMIT=1,GRX_STREAM_UPLOAD,GRX_STREAM_READ};
enum {GRX_STREAM_OK=0,GRX_STREAM_REQUEST=1,GRX_STREAM_IO=2,GRX_STREAM_GPU=3};
typedef struct {uint32_t sequence,nonce,op,address,bytes,reserved[11];} grx_stream_request;
typedef struct {uint32_t sequence,nonce,error,bytes,reserved[12];} grx_stream_result;
typedef struct {grx_stream_request request;grx_stream_result result;unsigned char data[GRX_STREAM_BYTES];} grx_stream_channel;
typedef struct {
 void *context;
 void (*pull)(void *,void *,unsigned);
 void (*push)(void *,void *,unsigned);
 int (*upload)(void *,uint32_t,const void *,unsigned);
 int (*readback)(void *,uint32_t,void *,unsigned);
 /* submit copies + validates commands before returning. */
 int (*submit)(void *,const void *,unsigned,uint32_t,uint32_t);
 /* 0 pending, 1 retired, -1 quarantined. */
 int (*poll)(void *);
} grx_stream_io;
typedef struct {
 grx_stream_channel *channel;grx_stream_io io;
 unsigned active,working,failed;uint32_t nonce,next,count;
} grx_stream;
int grx_stream_open(grx_stream *,grx_stream_channel *,const grx_stream_io *,uint32_t);
int grx_stream_close(grx_stream *,uint32_t);
void grx_stream_poll(grx_stream *);
#endif
