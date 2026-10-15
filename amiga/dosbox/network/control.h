/* GPL-3.0-or-later. Host-only Exec IPC: no pointers enter ARM shared memory. */
#ifndef ZZ_NET_CONTROL_H
#define ZZ_NET_CONTROL_H
#include <exec/ports.h>
#include <stdint.h>
#define DN_PORT "ZZDOS_NETWORK"
#define DN_CONTROL_MAGIC 0x5a4e4331u
enum {DN_STATUS=1,DN_STOP};
struct dn_message {struct Message msg;uint32_t magic,op,result,tx,rx,dropped,errors;};
#endif
