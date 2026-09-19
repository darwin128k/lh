/**
 * @file fn.h
 * @brief Callable signature for datagram write routines (`sendto`).
 *
 * Same role as ::lh_io_writer_write_fn, with an extra destination address:
 * each call is one whole datagram. A stream callback cannot carry that,
 * which is why UDP does not sit on ::lh_io_writer_t.
 *
 * @see lh_io_dgram_reader_recv_fn
 */

#ifndef LH_IO_DGRAM_WRITER_FN_H
#define LH_IO_DGRAM_WRITER_FN_H

#include <lh/net/socket/addr/ip4.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_io_dgram_writer_send_fn
 * @brief Function type for sending one datagram.
 *
 * @param context Identifies which datagram sink to write.
 * @param buf     Source buffer (the whole datagram).
 * @param size    Number of bytes from @p buf to send.
 * @param addr    Destination IPv4 address.
 *
 * @return Bytes actually sent, or a negative value on failure.
 */
typedef lh_ssize_t(lh_io_dgram_writer_send_fn)(lh_ptr context, const lh_ptr buf, lh_usize_t size,
                                               const lh_net_ip4_socket_addr_t *addr);

#endif /* LH_IO_DGRAM_WRITER_FN_H */
