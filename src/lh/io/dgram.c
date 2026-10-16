#include <lh/io/dgram.h>
#include <lh/assert.h>

lh_void
lh_io_dgram_get_reader(const lh_io_dgram_t *self, lh_io_dgram_reader_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = self->reader;
}

lh_void
lh_io_dgram_set_reader(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(reader);
    self->reader = lh_ptr_deref(reader);
}

lh_void
lh_io_dgram_get_writer(const lh_io_dgram_t *self, lh_io_dgram_writer_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = self->writer;
}

lh_void
lh_io_dgram_set_writer(lh_io_dgram_t *self, const lh_io_dgram_writer_t *writer)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(writer);
    self->writer = lh_ptr_deref(writer);
}

lh_void
lh_io_dgram_set(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader,
                const lh_io_dgram_writer_t *writer)
{
    lh_assert_runtime_ref(self);

    lh_io_dgram_set_reader(self, reader);
    lh_io_dgram_set_writer(self, writer);
}

lh_void
lh_io_dgram_init(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader,
                 const lh_io_dgram_writer_t *writer)
{
    lh_io_dgram_set(self, reader, writer);
}

lh_void
lh_io_dgram_assign(lh_io_dgram_t *self, const lh_io_dgram_t *other)
{
    lh_io_dgram_reader_t reader;
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    lh_io_dgram_get_reader(other, lh_addr_of(reader));
    lh_io_dgram_get_writer(other, lh_addr_of(writer));
    lh_io_dgram_set(self, lh_addr_of(reader), lh_addr_of(writer));
}

lh_ssize_t
lh_io_dgram_recv(lh_io_dgram_t *self, lh_ptr buf, lh_usize_t size, lh_net_ip4_socket_addr_t *addr)
{
    lh_io_dgram_reader_t reader;

    lh_assert_runtime_ref(self);

    lh_io_dgram_get_reader(self, lh_addr_of(reader));
    return lh_io_dgram_reader_recv(lh_addr_of(reader), buf, size, addr);
}

lh_ssize_t
lh_io_dgram_send(lh_io_dgram_t *self, const lh_ptr buf, lh_usize_t size,
                 const lh_net_ip4_socket_addr_t *addr)
{
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);

    lh_io_dgram_get_writer(self, lh_addr_of(writer));
    return lh_io_dgram_writer_send(lh_addr_of(writer), buf, size, addr);
}
