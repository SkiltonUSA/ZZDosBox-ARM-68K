/* GPL-3.0-or-later. Roadshow uses library sockets, not libc file descriptors. */
#pragma once
#include <exec/types.h>
struct Library;
extern struct Library *SocketBase;
/* The ROM-call header otherwise imports kernel mbuf/route definitions that
 * collide with libslirp's independent TCP/IP implementation. */
#define SYS_MBUF_H
#define NET_ROUTE_H
#define LIBRARIES_BSDSOCKET_H
#include <inline/bsdsocket.h>
#include <netdb.h>
#include <errno.h>
#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6 41
#endif
#ifndef PF_INET6
#define PF_INET6 AF_INET6
#endif
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(p) ((p)->s6_addr[0]==0xff)
#endif
#ifndef IPPROTO_ICMPV6
#define IPPROTO_ICMPV6 58
#endif
#ifndef IPV6_UNICAST_HOPS
#define IPV6_UNICAST_HOPS 4
#endif
#ifndef TCP_NODELAY
#define TCP_NODELAY 1
#endif
/* Roadshow uses BSD's leading length byte, then an 8-bit address family. */
struct sockaddr_storage { unsigned char ss_len; sa_family_t ss_family; unsigned char pad[122]; uint32_t align; };
_Static_assert(sizeof(struct sockaddr_storage)==128,"Roadshow address storage size");
_Static_assert(offsetof(struct sockaddr_storage,ss_family)==offsetof(struct sockaddr_in,sin_family),"Roadshow IPv4 family offset");
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif
/* IPv6 is disabled in the backend; these constants only satisfy shared code. */
static const struct in6_addr in6addr_any={{0}};
static const struct in6_addr in6addr_loopback={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}};
