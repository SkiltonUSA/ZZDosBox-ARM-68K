/* GPL-3.0-or-later. Host side of the single-producer/single-consumer rings.
 * Synchronize payload before publishing its index. Each index has one writer.
 * Callers supply valid shared memory, callbacks and DB_TX_SIZE output storage. */
#pragma once
#include "stream.h"
#include "protocol.h"

/* Count is validated against ring capacity by the entry points below. */
static inline void db_stream_sync(volatile uint8_t *base, unsigned size,
 uint32_t index, unsigned count, void (*sync)(volatile void *,size_t,void *),void *user) {
 unsigned start=index&(size-1),first=size-start;
 if(!count)return;
 if(first>count)first=count;
 sync(base+start,first,user);
 if(count>first)sync(base,count-first,user);
}

static inline void db_stream_publish(volatile uint8_t *b,const struct ad_io *io,
 unsigned offset,uint32_t value) {
 ad_put(b,offset,value);
 io->push(b+offset,4,io->user);
 io->barrier(io->user);
}

static inline int db_stream_input(volatile uint8_t *b,const struct ad_io *io,
 const uint8_t *data,unsigned count) {
 uint32_t write=ad_get(b,DB_RX_WRITE),used;
 io->pull(b+DB_RX_READ,4,io->user);
 used=write-ad_get(b,DB_RX_READ);
 if(used>DB_RX_SIZE||count>DB_RX_SIZE-used)return 0;
 if(!count)return 1;
 for(unsigned i=0;i<count;i++)b[DB_RX_DATA+((write+i)&(DB_RX_SIZE-1))]=data[i];
 db_stream_sync(b+DB_RX_DATA,DB_RX_SIZE,write,count,io->push,io->user);
 db_stream_publish(b,io,DB_RX_WRITE,write+count);
 return 1;
}

/* Peek leaves the consumer index unchanged until output has been accepted.
 * Returns bytes available, zero for an empty ring, -1 for corrupt indices. */
static inline int db_stream_peek(volatile uint8_t *b,const struct ad_io *io,uint8_t *data) {
 uint32_t read=ad_get(b,DB_TX_READ),count;
 io->pull(b+DB_TX_WRITE,4,io->user);
 count=ad_get(b,DB_TX_WRITE)-read;
 if(count>DB_TX_SIZE)return -1;
 db_stream_sync(b+DB_TX_DATA,DB_TX_SIZE,read,count,io->pull,io->user);
 for(unsigned i=0;i<count;i++)data[i]=b[DB_TX_DATA+((read+i)&(DB_TX_SIZE-1))];
 return (int)count;
}

/* Count is the successfully handled result of peek, with no intervening consume. */
static inline void db_stream_consume(volatile uint8_t *b,const struct ad_io *io,unsigned count) {
 if(count)db_stream_publish(b,io,DB_TX_READ,ad_get(b,DB_TX_READ)+count);
}
