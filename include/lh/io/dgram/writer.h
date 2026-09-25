/**
 * @file writer.h
 * @brief Datagram write sink: a `sendto` callback plus context.
 *
 * Mirror of ::lh_io_writer_t for message-oriented IO. Each send is one
 * datagram and carries a destination address; that is why this is not a
 * stream writer.
 */

#ifndef LH_IO_DGRAM_WRITER_H
#define LH_IO_DGRAM_WRITER_H

#include <lh/compiler/extern/c.h>
#include <lh/io/dgram/writer/cb.h>
#include <lh/io/dgram/writer/fields.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_dgram_writer
 * @brief A send callback paired with the context it needs.
 */
typedef struct lh_io_dgram_writer
{
    lh_io_dgram_writer_fields(lh_io_dgram_writer_send_fn);
} lh_io_dgram_writer_t;

LH_COMPILER_EXTERN_C_BEGIN

void
lh_io_dgram_writer_assign(lh_io_dgram_writer_t *self, const lh_io_dgram_writer_t *other);

void
lh_io_dgram_writer_set(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb,
                       lh_ptr context);

void
lh_io_dgram_writer_set_send_cb(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb);

void
lh_io_dgram_writer_set_context(lh_io_dgram_writer_t *self, lh_ptr context);

/**
 * @brief Initialize a datagram writer's callback and context.
 *
 * @param self    Writer object to initialize.
 * @param send_cb Send callback (may be ::lh_null to leave sending disabled).
 * @param context Context passed to @p send_cb on every call.
 */
void
lh_io_dgram_writer_init(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb,
                        lh_ptr context);

void
lh_io_dgram_writer_deinit(lh_io_dgram_writer_t *self);

lh_io_dgram_writer_send_cb
lh_io_dgram_writer_get_send_cb(const lh_io_dgram_writer_t *self);

lh_ptr
lh_io_dgram_writer_get_context(const lh_io_dgram_writer_t *self);

/**
 * @brief Send one datagram from @p buf via @p self's callback.
 *
 * Fails with a runtime check when the callback is not initialized.
 *
 * @param self Writer to send through.
 * @param buf  Source buffer (the whole datagram).
 * @param size Number of bytes from @p buf to send.
 * @param addr Destination IPv4 address.
 *
 * @return Bytes actually sent, or negative on failure.
 */
lh_ssize_t
lh_io_dgram_writer_send(lh_io_dgram_writer_t *self, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_DGRAM_WRITER_H */
