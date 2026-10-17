#pragma once
#include "platform.h"
#include <string.h>
static inline void CopyMem(const void *src,void *dst,ULONG n){memcpy(dst,src,n);}
