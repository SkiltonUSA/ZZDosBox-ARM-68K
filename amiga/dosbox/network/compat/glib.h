/* GPL-3.0-or-later. Small libslirp-specific portability layer, not a GLib ABI.
 * Process spawning is deliberately absent; guest execution/forwarding is disabled. */
#pragma once
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <limits.h>
#define GLIB_CHECK_VERSION(a,b,c) 1
/* libslirp needs this to keep IP overlays 20 bytes on BOTH 32/64-bit hosts. */
#define GLIB_SIZEOF_VOID_P __SIZEOF_POINTER__
#define G_LITTLE_ENDIAN 1234
#define G_BIG_ENDIAN 4321
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define G_BYTE_ORDER G_BIG_ENDIAN
#else
#define G_BYTE_ORDER G_LITTLE_ENDIAN
#endif
#define G_GNUC_PRINTF(a,b) __attribute__((format(printf,a,b)))
#define G_STATIC_ASSERT(x) _Static_assert(x,#x)
#define G_UNLIKELY(x) (x)
#define G_N_ELEMENTS(a) (sizeof(a)/sizeof((a)[0]))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define g_assert assert
#define g_assert_not_reached() abort()
#define g_return_if_fail(x) do { if(!(x)) return; } while(0)
#define g_return_val_if_fail(x,v) do { if(!(x)) return (v); } while(0)
#define g_warn_if_fail(x) do { if(!(x)) fprintf(stderr,"libslirp: %s\n",#x); } while(0)
#define g_warn_if_reached() g_warning("unexpected libslirp branch")
#define g_warning(...) do { fprintf(stderr,__VA_ARGS__); fputc('\n',stderr); } while(0)
#define g_critical g_warning
#define g_error(...) do { g_warning(__VA_ARGS__); abort(); } while(0)
#define g_debug(...) ((void)0)
#define g_free free
#define g_getenv getenv
#define g_strerror strerror
static inline char *g_strdup(const char *s) { if(!s)return NULL;char *p=strdup(s);if(!p)abort();return p; }
#define g_ascii_strcasecmp strcasecmp
#define g_snprintf snprintf
#define g_vsnprintf vsnprintf
typedef int gboolean;
typedef int gint;
typedef char gchar;
typedef char **GStrv;
typedef void *gpointer;
typedef struct { const char *key; unsigned value; } GDebugKey;
typedef struct { char *str; size_t len,allocated_len; } GString;
typedef struct { uint32_t state; } GRand;
static inline void *g_malloc(size_t n) { void *p=malloc(n?n:1); if(!p)abort(); return p; }
static inline void *g_malloc0(size_t n) { void *p=g_malloc(n);memset(p,0,n);return p; }
static inline void *g_realloc(void *p,size_t n) { p=realloc(p,n?n:1);if(!p)abort();return p; }
static inline size_t zz_glib_product(size_t a,size_t b) { if(b&&a>SIZE_MAX/b)abort();return a*b; }
#define g_new(t,n) ((t*)g_malloc(zz_glib_product(sizeof(t),(n))))
#define g_new0(t,n) ((t*)g_malloc0(zz_glib_product(sizeof(t),(n))))
static inline size_t g_strlcpy(char *d,const char *s,size_t n) { size_t z=strlen(s);if(n){size_t k=z<n-1?z:n-1;memcpy(d,s,k);d[k]=0;}return z; }
static inline int g_str_has_prefix(const char*s,const char*p) { return !strncmp(s,p,strlen(p)); }
static inline char *g_strstr_len(const char*s,long n,const char*p) { size_t z=strlen(p);if(n<0)return strstr(s,p);for(long i=0;i<=n-(long)z&&s[i];i++)if(!strncmp(s+i,p,z))return (char*)s+i;return NULL; }
static inline unsigned g_strv_length(char **s) { unsigned n=0;while(s&&s[n])n++;return n; }
static inline void g_strfreev(char **s) { if(s){for(unsigned i=0;s[i];i++)free(s[i]);free(s);} }
static inline unsigned g_parse_debug_string(const char*s,const GDebugKey*k,unsigned n) { (void)s;(void)k;(void)n;return 0; }
static inline GRand *g_rand_new(void) { GRand*r=g_new(GRand,1);r->state=0x87d2f357u^(uint32_t)(uintptr_t)r;return r; }
static inline void g_rand_free(GRand*r) { free(r); }
static inline int32_t g_rand_int_range(GRand*r,int32_t a,int32_t b) { uint32_t x=r->state;assert(b>a);x^=x<<13;x^=x>>17;x^=x<<5;r->state=x;return a+(int32_t)(x/(UINT32_MAX/(uint32_t)(b-a)+1u)); }
static inline GString *g_string_new(const char*s) { GString*g=g_new0(GString,1);g->str=strdup(s?s:"");if(!g->str)abort();g->len=strlen(g->str);g->allocated_len=g->len+1;return g; }
static inline char *g_string_free(GString*g,int drop) { char*s=g->str;free(g);if(drop){free(s);return NULL;}return s; }
static inline GString *g_string_append_printf(GString*g,const char*f,...) { va_list a,b;va_start(a,f);va_copy(b,a);int n=vsnprintf(NULL,0,f,b);va_end(b);if(n<0||(size_t)n>SIZE_MAX-g->len-1)abort();g->allocated_len=g->len+(size_t)n+1;g->str=g_realloc(g->str,g->allocated_len);vsnprintf(g->str+g->len,(size_t)n+1,f,a);va_end(a);g->len+=(size_t)n;return g; }
