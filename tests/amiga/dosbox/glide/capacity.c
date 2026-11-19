#include <assert.h>
#include <stdio.h>
#include "amiga/dosbox/glide/capacity.h"
static unsigned polls;
static int cancel(void) { return ++polls==3; }
int main(void) {
    struct gc_result a,b;
    assert(!grx_capacity(&a,0,0));
    assert(a.triangles==GC_TRIANGLES && a.bytes==57344u && a.hash);
    assert(!grx_capacity(&b,0,0));
    assert(a.hash==b.hash && a.bytes==b.bytes && a.ticks==0);
    assert(grx_capacity(&b,cancel,0)==2 && polls==3);
    printf("CAPACITY triangles=%u bytes=%u hash=%08x\n",a.triangles,a.bytes,a.hash);
}
