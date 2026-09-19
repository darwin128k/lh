/**
 * @file fn.h
 * @brief Callable signature for datagram read routines (`recvfrom`).
 *
 * Same role as ::lh_io_reader_read_fn, with an extra address: each call is
 * one whole datagram, and @p addr receives the sender. A stream callback
 * cannot carry that, which is why UDP does not sit on ::lh_io_reader_t.
 *
 * @see lh_io_dgram_writer_send_fn
 */

#ifndef LH_IO_DGRAM_READER_FN_H
#define LH_IO_DGRAM_READER_FN_H

#include <lh/net/socket/addr/ip4.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_io_dgram_reader_recv_fn
 * @brief Function type for receiving one datagram.
 *
 * @param context Identifies which datagram source to read.
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to copy into @p buf (one datagram; a short
 *                buffer may truncate, depending on the OS).
 * @param addr    Receives the sender's IPv4 address.
 *
 * @return Bytes actually received. `0` is a zero-length datagram, not
 *         end-of-stream. Negative on failure.
 */
typedef lh_ssize_t(lh_io_dgram_reader_recv_fn)(lh_ptr context, lh_ptr buf, lh_usize_t size,
                                               lh_net_ip4_socket_addr_t *addr);

#endif /* LH_IO_DGRAM_READER_FN_H */
