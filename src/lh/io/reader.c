#include <lh/io/reader.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/runtime/error.h>

void
lh_io_reader_set_read_cb(lh_io_reader_t *self, lh_io_reader_read_cb read_cb)
{
    lh_assert_runtime_ref(self);
    self->read_cb = read_cb;
}

void
lh_io_reader_set_context(lh_io_reader_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

void
lh_io_reader_set(lh_io_reader_t *self, lh_io_reader_read_cb read_cb, lh_ptr context)
{
    lh_io_reader_set_read_cb(self, read_cb);
    lh_io_reader_set_context(self, context);
}

void
lh_io_reader_assign(lh_io_reader_t *self, const lh_io_reader_t *other)
{
    lh_io_reader_set(self, lh_io_reader_get_read_cb(other), lh_io_reader_get_context(other));
}

void
lh_io_reader_init(lh_io_reader_t *self, lh_io_reader_read_cb read_cb, lh_ptr context)
{
    lh_io_reader_set(self, read_cb, context);
}

void
lh_io_reader_deinit(lh_io_reader_t *self)
{
    lh_io_reader_set(self, lh_null, lh_null);
}

lh_io_reader_read_cb
lh_io_reader_get_read_cb(const lh_io_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->read_cb;
}

lh_ptr
lh_io_reader_get_context(const lh_io_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_ssize_t
lh_io_reader_read(lh_io_reader_t *self, lh_ptr buf, lh_usize_t size)
{
    lh_io_reader_read_cb read_cb = lh_io_reader_get_read_cb(self);
    lh_assert_runtime_ifn(read_cb, lh_runtime_error_code_invalid_argument);

    return read_cb(lh_io_reader_get_context(self), buf, size);
}
