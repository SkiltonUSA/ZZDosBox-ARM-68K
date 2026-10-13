/* GPL-2.0-or-later. Bounded DOSBox interpreter bring-up interface. */
#pragma once
#include <stdint.h>
struct DosboxResult { uint32_t slices,exit_code,hash,bytes,fault; char text[160]; };
extern "C" int db_run(const uint8_t *program,unsigned bytes,unsigned limit,
                     DosboxResult *result,int (*stop)());
