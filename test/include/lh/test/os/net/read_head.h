/**
 * @file read_head.h
 * @brief Test helper: read an HTTP head off a socket.
 *
 * `lh_test::read_head(sock)` reads one byte at a time until the blank line that
 * ends an HTTP head, or until the peer closes, and returns what it read. The
 * web host's tests use it to look at a status line and the headers of a reply
 * without parsing a body.
 */

#ifndef LH_TEST_OS_NET_READ_HEAD_H
#define LH_TEST_OS_NET_READ_HEAD_H

#include <lh/os/net/socket.h>

#include <string>

namespace lh_test
{

/* Everything up to the blank line that ends an HTTP head, or up to the peer closing. */
inline std::string
read_head(lh_os_net_socket_t *sock)
{
    std::string head;
    char byte;

    while (head.find("\r\n\r\n") == std::string::npos &&
           lh_os_net_socket_recv(sock, lh_addr_of(byte), 1u) == 1)
    {
        head += byte;
    }
    return head;
}

} // namespace lh_test

#endif // LH_TEST_OS_NET_READ_HEAD_H
