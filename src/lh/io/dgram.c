#include <lh/io/dgram.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_io_dgram_t
lh_io_dgram_make(const lh_io_dgram_reader_t *reader, const lh_io_dgram_writer_t *writer)
{
    lh_io_dgram_t result;
    lh_io_dgram_set(lh_addr_of(result), reader, writer);
    return result;
}

void
lh_io_dgram_set(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader,
                const lh_io_dgram_writer_t *writer)
{
    lh_assert_runtime_ref(self);

    lh_io_dgram_set_reader(self, reader);
    lh_io_dgram_set_writer(self, writer);
}

void
lh_io_dgram_assign(lh_io_dgram_t *self, const lh_io_dgram_t *other)
{
    lh_io_dgram_reader_t reader;
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    reader = lh_io_dgram_get_reader(other);
    writer = lh_io_dgram_get_writer(other);
    lh_io_dgram_set(self, lh_addr_of(reader), lh_addr_of(writer));
}

lh_io_dgram_reader_t
lh_io_dgram_get_reader(const lh_io_dgram_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reader;
}

void
lh_io_dgram_set_reader(lh_io_dgram_t *self, const lh_io_dgram_reader_t *reader)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(reader);
    self->reader = lh_ptr_deref(reader);
}

lh_io_dgram_writer_t
lh_io_dgram_get_writer(const lh_io_dgram_t *self)
{
    lh_assert_runtime_ref(self);
    return self->writer;
}

void
lh_io_dgram_set_writer(lh_io_dgram_t *self, const lh_io_dgram_writer_t *writer)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(writer);
    self->writer = lh_ptr_deref(writer);
}

lh_ssize_t
lh_io_dgram_recv(lh_io_dgram_t *self, lh_ptr buf, lh_usize_t size, lh_net_ip4_socket_addr_t *addr)
{
    lh_io_dgram_reader_t reader;

    lh_assert_runtime_ref(self);

    reader = lh_io_dgram_get_reader(self);
    return lh_io_dgram_reader_recv(lh_addr_of(reader), buf, size, addr);
}

lh_ssize_t
lh_io_dgram_send(lh_io_dgram_t *self, const lh_ptr buf, lh_usize_t size,
                 const lh_net_ip4_socket_addr_t *addr)
{
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);

    writer = lh_io_dgram_get_writer(self);
    return lh_io_dgram_writer_send(lh_addr_of(writer), buf, size, addr);
}
