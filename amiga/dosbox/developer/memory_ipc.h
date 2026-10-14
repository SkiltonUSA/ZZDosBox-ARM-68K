/* GPL-3.0-or-later. Fixed-size same-machine Exec message; no payload pointers. */
#pragma once
#include <exec/ports.h>
#include "memory_wire.h"
#define DM_PORT "ZZDosBox.GuestMemory.v1"
struct dm_message {struct Message msg;ULONG magic;struct dm_request request;struct dm_reply reply;};
