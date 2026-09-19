#include <lh/io/dgram/writer.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/runtime/error.h>

void
lh_io_dgram_writer_set_send_cb(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb)
{
    lh_assert_runtime_ref(self);
    self->send_cb = send_cb;
}

void
lh_io_dgram_writer_set_context(lh_io_dgram_writer_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

void
lh_io_dgram_writer_set(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb,
                       lh_ptr context)
{
    lh_io_dgram_writer_set_send_cb(self, send_cb);
    lh_io_dgram_writer_set_context(self, context);
}

void
lh_io_dgram_writer_assign(lh_io_dgram_writer_t *self, const lh_io_dgram_writer_t *other)
{
    lh_io_dgram_writer_set(self, lh_io_dgram_writer_get_send_cb(other),
                           lh_io_dgram_writer_get_context(other));
}

void
lh_io_dgram_writer_init(lh_io_dgram_writer_t *self, lh_io_dgram_writer_send_cb send_cb,
                        lh_ptr context)
{
    lh_io_dgram_writer_set(self, send_cb, context);
}

void
lh_io_dgram_writer_deinit(lh_io_dgram_writer_t *self)
{
    lh_io_dgram_writer_set(self, lh_null, lh_null);
}

lh_io_dgram_writer_send_cb
lh_io_dgram_writer_get_send_cb(const lh_io_dgram_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->send_cb;
}

lh_ptr
lh_io_dgram_writer_get_context(const lh_io_dgram_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_ssize_t
lh_io_dgram_writer_send(lh_io_dgram_writer_t *self, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr)
{
    lh_io_dgram_writer_send_cb send_cb = lh_io_dgram_writer_get_send_cb(self);
    lh_assert_runtime_ifn(send_cb,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    return send_cb(lh_io_dgram_writer_get_context(self), buf, size, addr);
}
