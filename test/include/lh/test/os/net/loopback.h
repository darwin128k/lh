/**
 * @file loopback.h
 * @brief Test helpers: a free port on 127.0.0.1 and a connection to one.
 *
 * `lh_test::free_port()` asks the system for a port nobody holds by binding
 * port `0` and reading back what it got; the port is released at once, so a
 * test listens on it right after. `lh_test::connect_loopback(sock, port)`
 * opens a TCP socket to 127.0.0.1:`port`. The web host's tests use them to
 * reach a host they started themselves.
 */

#ifndef LH_TEST_OS_NET_LOOPBACK_H
#define LH_TEST_OS_NET_LOOPBACK_H

#include <lh/os/net/socket.h>

namespace lh_test
{

/* A port on 127.0.0.1 that was free a moment ago; 0 when the system would not say. */
inline lh_net_port_t
free_port()
{
    lh_net_port_t port = 0u;
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), 0);
    lh_os_net_socket_t probe;
    lh_os_net_socket_init(lh_addr_of(probe));
    if (lh_os_net_socket_open(lh_addr_of(probe), lh_net_socket_type_tcp) &&
        lh_os_net_socket_bind(lh_addr_of(probe), lh_addr_of(addr)) &&
        lh_os_net_socket_get_local_addr(lh_addr_of(probe), lh_addr_of(addr)))
    {
        port = lh_net_ip4_socket_addr_get_port(lh_addr_of(addr));
    }
    lh_os_net_socket_close(lh_addr_of(probe));
    return port;
}

/* Open @p sock to 127.0.0.1:@p port; false, and @p sock closed, when it would not connect. */
inline bool
connect_loopback(lh_os_net_socket_t *sock, lh_net_port_t port)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);
    lh_os_net_socket_init(sock);
    if (!lh_os_net_socket_open(sock, lh_net_socket_type_tcp))
    {
        return false;
    }
    if (!lh_os_net_socket_connect(sock, lh_addr_of(addr)))
    {
        lh_os_net_socket_close(sock);
        return false;
    }
    return true;
}

} // namespace lh_test

#endif // LH_TEST_OS_NET_LOOPBACK_H
