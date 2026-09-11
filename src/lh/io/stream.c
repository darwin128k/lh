#include <lh/io/stream.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_io_stream_t
lh_io_stream_make(const lh_io_reader_t *reader, const lh_io_writer_t *writer)
{
    lh_io_stream_t result;
    lh_io_stream_set(lh_addr_of(result), reader, writer);
    return result;
}

void
lh_io_stream_set(lh_io_stream_t *self, const lh_io_reader_t *reader, const lh_io_writer_t *writer)
{
    lh_assert_runtime_ref(self);

    lh_io_stream_set_reader(self, reader);
    lh_io_stream_set_writer(self, writer);
}

void
lh_io_stream_assign(lh_io_stream_t *self, const lh_io_stream_t *other)
{
    lh_io_reader_t reader;
    lh_io_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    reader = lh_io_stream_get_reader(other);
    writer = lh_io_stream_get_writer(other);
    lh_io_stream_set(self, lh_addr_of(reader), lh_addr_of(writer));
}

lh_io_reader_t
lh_io_stream_get_reader(const lh_io_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reader;
}

void
lh_io_stream_set_reader(lh_io_stream_t *self, const lh_io_reader_t *reader)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(reader);
    self->reader = lh_ptr_deref(reader);
}

lh_io_writer_t
lh_io_stream_get_writer(const lh_io_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->writer;
}

void
lh_io_stream_set_writer(lh_io_stream_t *self, const lh_io_writer_t *writer)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(writer);
    self->writer = lh_ptr_deref(writer);
}

lh_ssize_t
lh_io_stream_read(lh_io_stream_t *self, lh_ptr buf, lh_usize_t size)
{
    lh_io_reader_t reader;

    lh_assert_runtime_ref(self);

    reader = lh_io_stream_get_reader(self);
    return lh_io_reader_read(lh_addr_of(reader), buf, size);
}

lh_ssize_t
lh_io_stream_write(lh_io_stream_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_io_writer_t writer;

    lh_assert_runtime_ref(self);

    writer = lh_io_stream_get_writer(self);
    return lh_io_writer_write(lh_addr_of(writer), buf, size);
}
