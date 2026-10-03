/**
 * @file addr.h
 * @brief Backend-private: ::lh_net_ip4_socket_addr_t <-> `struct sockaddr_in`.
 *
 * Shared by `win/net/socket.c` and `posix/net/socket.c` — `sockaddr_in` has
 * the same member names on both (Windows' is our own mirror, win/ws2_32.h;
 * POSIX's comes from `<netinet/in.h>`). Not installed, not part of the API.
 */

#ifndef LH_SRC_OS_SYSTEM_NET_SOCKET_ADDR_H
#define LH_SRC_OS_SYSTEM_NET_SOCKET_ADDR_H

#include <lh/attribute/static.h>
#include <lh/compiler/os.h>
#include <lh/memory.h>
#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/numeric/types.h>
#include <lh/util/addr.h>
#include <lh/bit/endian.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    include <lh/os/system/win/ws2_32.h>
typedef lh_os_system_win_sockaddr_in_t lh_os_system_net_socket_native_addr_t;
#    define LH_OS_SYSTEM_NET_SOCKET_NATIVE_AF_INET LH_OS_SYSTEM_WIN_AF_INET
#else
#    include <netinet/in.h>
typedef struct sockaddr_in lh_os_system_net_socket_native_addr_t;
#    define LH_OS_SYSTEM_NET_SOCKET_NATIVE_AF_INET AF_INET
#endif

LH_ATTRIBUTE_STATIC
void
lh_os_system_net_socket_native_addr_from_ip4(lh_os_system_net_socket_native_addr_t *native_addr,
                                             const lh_net_ip4_socket_addr_t *addr)
{
    lh_net_ip4_t ip;
    lh_net_port_t port;
    lh_uchar_t *addr_bytes;

    ip = lh_net_ip4_socket_addr_get_ip(addr);
    port = lh_net_ip4_socket_addr_get_port(addr);

    lh_memory_set(lh_addr_of(*native_addr), sizeof(*native_addr), 0);
    native_addr->sin_family = LH_OS_SYSTEM_NET_SOCKET_NATIVE_AF_INET;

    /* sin_addr is always network byte order in memory, byte for byte the same
     * order as the dotted-quad octets — writing the raw bytes avoids
     * depending on htonl. sin_port is also network byte order, but that one
     * genuinely is "pack a u16 as big-endian bytes", so it goes through
     * lh_bit_pack_be16 instead of a second hand-rolled shift/mask. */
    addr_bytes = lh_ptr_rcast(lh_uchar_t, lh_addr_of(native_addr->sin_addr));
    addr_bytes[0] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_0);
    addr_bytes[1] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_1);
    addr_bytes[2] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_2);
    addr_bytes[3] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_3);

    lh_bit_pack_be16(port, lh_ptr_rcast(lh_uchar_t, lh_addr_of(native_addr->sin_port)));
}

LH_ATTRIBUTE_STATIC
void
lh_os_system_net_socket_ip4_from_native_addr(lh_net_ip4_socket_addr_t *addr,
                                             const lh_os_system_net_socket_native_addr_t *native_addr)
{
    const lh_uchar_t *addr_bytes;
    lh_net_ip4_t ip;
    lh_net_port_t port;

    addr_bytes = lh_ptr_ccast(lh_uchar_t, lh_addr_of(native_addr->sin_addr));
    ip = lh_net_ip4_make(addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);
    port = lh_bit_unpack_be16(lh_ptr_ccast(lh_uchar_t, lh_addr_of(native_addr->sin_port)));
    *addr = lh_net_ip4_socket_addr_make(lh_addr_of(ip), port);
}

#endif /* LH_SRC_OS_SYSTEM_NET_SOCKET_ADDR_H */
