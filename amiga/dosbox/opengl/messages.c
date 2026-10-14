/* GPL-3.0-or-later. TinyGL diagnostics avoid stdio in the freestanding worker. */
#include <stdlib.h>
void tgl_warning(const char *format,...){(void)format;}
void tgl_trace(const char *format,...){(void)format;}
void tgl_fixme(const char *format,...){(void)format;}
void gl_fatal_error(char *format,...){(void)format;abort();}

void *gl_malloc(int size){void*p=malloc(size);if(!p)abort();return p;}
void *gl_zalloc(int size){void*p=calloc(1,size);if(!p)abort();return p;}
void gl_free(void*p){free(p);}
