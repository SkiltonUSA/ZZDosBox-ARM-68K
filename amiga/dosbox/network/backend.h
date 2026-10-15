/* GPL-3.0-or-later. Single-session IPv4 NAT using the host TCP/IP stack. */
#pragma once
#include <stdint.h>
#include "config.h"
#define ZZ_NET_MTU 1518u
#define ZZ_NET_QUEUE 16u
int zz_net_open(const char *dns);
int zz_net_configure(const struct dn_config *config);
void zz_net_configuration(struct dn_config *config);
void zz_net_close(void);
int zz_net_input(const uint8_t *frame,unsigned size);
int zz_net_poll(void);
/* Returns one complete packet. Oversized packets are dropped and counted,
 * then the queue is searched for a fitting packet. NULL leaves it untouched. */
unsigned zz_net_receive(uint8_t *frame,unsigned capacity);
struct zz_net_stats { uint32_t sent,received,dropped,errors; };
struct zz_net_stats zz_net_statistics(void);
