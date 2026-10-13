/* GPL-2.0-or-later. Deterministic prototype RTC: 1980-01-01 UTC. */
#include <time.h>
time_t time(time_t *out){time_t value=315532800;if(out)*out=value;return value;}

/* One application-owned worker; never use the toolchain TLS buffer. */
struct tm *localtime(const time_t *t){static struct tm out;return gmtime_r(t,&out);}
