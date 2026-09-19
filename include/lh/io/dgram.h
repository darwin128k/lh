/**
 * @file dgram.h
 * @brief Duplex datagram: an ::lh_io_dgram_reader_t and an
 *        ::lh_io_dgram_writer_t together.
 *
 * The datagram counterpart of ::lh_io_stream_t. Whatever hands this out
 * (a UDP socket today) plugs `recvfrom` / `sendto` callbacks into the two
 * halves — everything above this point works against
 * ::lh_io_dgram_recv / ::lh_io_dgram_send without knowing a socket is
 * behind it. Not a byte stream: each call is one datagram plus an address.
 */

#ifndef LH_IO_DGRAM_H
#define LH_IO_DGRAM_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/io/dgram/reader.h>
#include <lh/io/dgram/writer.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_dgram
 * @typedef lh_io_dgram_t
 * @brief An ::lh_io_dgram_reader_t paired with an ::lh_io_dgram_writer_t.
 */
struct lh_io_dgram
{
    lh_io_dgram_reader_t reader;
    lh_io_dgram_writer_t writer;
};
typedef struct lh_io_dgram lh_io_dgram_t;

LH_COMPILER_EXTERN_C_BEGIN

LH_ATTRIBUTE_SYMBOL
lh_io_dgram_t
lh_io_dgram_make(const lh_io_dgram_reader_t *reader, const lh_io_dgram_writer_t *writer);

LH_ATTRIBUTE_SYMBOL
void
lh_io_dgram_set(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader,
                const lh_io_dgram_writer_t *writer);

LH_ATTRIBUTE_SYMBOL
void
lh_io_dgram_assign(lh_io_dgram_t *self, const lh_io_dgram_t *other);

LH_ATTRIBUTE_SYMBOL
lh_io_dgram_reader_t
lh_io_dgram_get_reader(const lh_io_dgram_t *self);

LH_ATTRIBUTE_SYMBOL
void
lh_io_dgram_set_reader(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader);

LH_ATTRIBUTE_SYMBOL
lh_io_dgram_writer_t
lh_io_dgram_get_writer(const lh_io_dgram_t *self);

LH_ATTRIBUTE_SYMBOL
void
lh_io_dgram_set_writer(lh_io_dgram_t *self, const lh_io_dgram_writer_t *writer);

/**
 * @brief Receive via @p self's reader. Same contract as ::lh_io_dgram_reader_recv.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_dgram_recv(lh_io_dgram_t *self, lh_ptr buf, lh_usize_t size, lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Send via @p self's writer. Same contract as ::lh_io_dgram_writer_send.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_dgram_send(lh_io_dgram_t *self, const lh_ptr buf, lh_usize_t size,
                 const lh_net_ip4_socket_addr_t *addr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_DGRAM_H */
