/* No SDL runtime in this frontend. Only the upstream timer declaration. */
#pragma once
#include <stdint.h>
#define SDL_VERSION_ATLEAST(a,b,c) 0
extern "C" uint32_t SDL_GetTicks(void);
