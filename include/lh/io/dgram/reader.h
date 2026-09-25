/**
 * @file reader.h
 * @brief Datagram read source: a `recvfrom` callback plus context.
 *
 * Mirror of ::lh_io_reader_t for message-oriented IO. Each recv is one
 * datagram and carries a sender address; that is why this is not a stream
 * reader.
 */

#ifndef LH_IO_DGRAM_READER_H
#define LH_IO_DGRAM_READER_H

#include <lh/compiler/extern/c.h>
#include <lh/io/dgram/reader/cb.h>
#include <lh/io/dgram/reader/fields.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_dgram_reader
 * @brief A recv callback paired with the context it needs.
 */
typedef struct lh_io_dgram_reader
{
    lh_io_dgram_reader_fields(lh_io_dgram_reader_recv_fn);
} lh_io_dgram_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

void
lh_io_dgram_reader_assign(lh_io_dgram_reader_t *self, const lh_io_dgram_reader_t *other);

void
lh_io_dgram_reader_set(lh_io_dgram_reader_t *self, lh_io_dgram_reader_recv_cb recv_cb,
                       lh_ptr context);

void
lh_io_dgram_reader_set_recv_cb(lh_io_dgram_reader_t *self, lh_io_dgram_reader_recv_cb recv_cb);

void
lh_io_dgram_reader_set_context(lh_io_dgram_reader_t *self, lh_ptr context);

/**
 * @brief Initialize a datagram reader's callback and context.
 *
 * @param self    Reader object to initialize.
 * @param recv_cb Recv callback (may be ::lh_null to leave receiving disabled).
 * @param context Context passed to @p recv_cb on every call.
 */
void
lh_io_dgram_reader_init(lh_io_dgram_reader_t *self, lh_io_dgram_reader_recv_cb recv_cb,
                        lh_ptr context);

void
lh_io_dgram_reader_deinit(lh_io_dgram_reader_t *self);

lh_io_dgram_reader_recv_cb
lh_io_dgram_reader_get_recv_cb(const lh_io_dgram_reader_t *self);

lh_ptr
lh_io_dgram_reader_get_context(const lh_io_dgram_reader_t *self);

/**
 * @brief Receive one datagram into @p buf via @p self's callback.
 *
 * Fails with a runtime check when the callback is not initialized.
 *
 * @param self Reader to receive from.
 * @param buf  Destination buffer.
 * @param size Maximum bytes to copy into @p buf.
 * @param addr Receives the sender's IPv4 address.
 *
 * @return Bytes actually received (`0` is an empty datagram), or negative
 *         on failure.
 */
lh_ssize_t
lh_io_dgram_reader_recv(lh_io_dgram_reader_t *self, lh_ptr buf, lh_usize_t size,
                        lh_net_ip4_socket_addr_t *addr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_DGRAM_READER_H */
