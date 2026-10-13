/* GPL-2.0-or-later. Single-core runtime inside the Exec-owned allocation. */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "dosbox.h"
extern "C" void fb_heap_init(void *,size_t);
alignas(64) static uint8_t heap[2*1024*1024];
extern "C" void (*__init_array_start[])();
extern "C" void (*__init_array_end[])();
extern "C" void db_runtime_init(){
 static bool ready;if(ready)return;ready=true;
 fb_heap_init(heap,sizeof(heap));
 for(auto p=__init_array_start;p!=__init_array_end;++p)(*p)();
}
void *operator new(size_t n){void *p=malloc(n);if(!p)abort();return p;}
void *operator new[](size_t n){return ::operator new(n);}
void operator delete(void *p)noexcept{free(p);}
void operator delete[](void *p)noexcept{free(p);}
void operator delete(void *p,size_t)noexcept{free(p);}
void operator delete[](void *p,size_t)noexcept{free(p);}
extern "C" void __cxa_pure_virtual(){abort();}
extern "C" void __aeabi_atexit(){}
void *__dso_handle;
extern "C" { int errno; }

extern "C" void abort(){E_Exit("ARM runtime abort");}
