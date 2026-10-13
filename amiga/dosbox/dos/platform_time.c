/* GPL-2.0-or-later. Only seeds DOS temporary filenames in this headless build.
 * Guest BIOS date/time services are not yet connected to the host clock. */
#include <time.h>
time_t time(time_t *out){time_t value=0;if(out)*out=value;return value;}
