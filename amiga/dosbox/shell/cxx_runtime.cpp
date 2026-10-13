/* GPL-2.0-or-later. Bare-metal STL failure paths use the worker fault exit. */
#include <bits/functexcept.h>
#include <stdarg.h>
#include <stdio.h>
#include "dosbox.h"
namespace std {
void __throw_length_error(const char *s){E_Exit("C++ length: %s",s);}
void __throw_logic_error(const char *s){E_Exit("C++ logic: %s",s);}
void __throw_out_of_range_fmt(const char *s,...){char text[128];va_list a;va_start(a,s);vsnprintf(text,sizeof(text),s,a);va_end(a);E_Exit("C++ range: %s",text);}
}
