#include <lh/io/writer.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/util/ptr.h>

lh_void
lh_io_writer_set_write_cb(lh_io_writer_t *self, lh_io_writer_write_cb write_cb)
{
    lh_assert_runtime_ref(self);
    self->write_cb = write_cb;
}

lh_void
lh_io_writer_set_context(lh_io_writer_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_io_writer_set(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context)
{
    lh_io_writer_set_write_cb(self, write_cb);
    lh_io_writer_set_context(self, context);
}

lh_void
lh_io_writer_assign(lh_io_writer_t *self, const lh_io_writer_t *other)
{
    lh_io_writer_set(self, lh_io_writer_get_write_cb(other), lh_io_writer_get_context(other));
}

lh_void
lh_io_writer_init(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context)
{
    lh_io_writer_set(self, write_cb, context);
}

lh_void
lh_io_writer_deinit(lh_io_writer_t *self)
{
    lh_io_writer_set(self, lh_null, lh_null);
}

lh_io_writer_write_cb
lh_io_writer_get_write_cb(const lh_io_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->write_cb;
}

lh_ptr
lh_io_writer_get_context(const lh_io_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_ssize_t
lh_io_writer_write(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_io_writer_write_cb write_cb = lh_io_writer_get_write_cb(self);
    lh_assert_runtime_ifn(write_cb, lh_runtime_error_code_invalid_argument);

    return write_cb(lh_io_writer_get_context(self), buf, size);
}

lh_bool_t
lh_io_writer_write_all(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_byte_cptr p = lh_ptr_ccast(lh_byte_t, buf);
    lh_usize_t left = size;

    if (size == 0u)
    {
        return lh_bool_true;
    }
    lh_assert_runtime_ref(buf);
    while (left > 0u)
    {
        const lh_ssize_t w = lh_io_writer_write(self, lh_cast_const(lh_ptr, p), left);
        const lh_usize_t n = lh_cast_static(lh_usize_t, w);

        if (w <= 0)
        {
            return lh_bool_false;
        }
        p = lh_ptr_add_by_offset(const lh_byte_t, p, n);
        left -= n;
    }
    return lh_bool_true;
}
