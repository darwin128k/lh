#include <lh/io/writer.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

void
lh_io_writer_pack(lh_io_writer_t *self, lh_io_writer_write_cb *write_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(write_cb)
    {
        self->write_cb = lh_ptr_deref(write_cb);
    }

    lh_optional_ref(context)
    {
        self->context = lh_ptr_deref(context);
    }
}

void
lh_io_writer_unpack(const lh_io_writer_t *self, lh_io_writer_write_cb *write_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(write_cb)
    {
        lh_ptr_deref(write_cb) = self->write_cb;
    }

    lh_optional_ref(context)
    {
        lh_ptr_deref(context) = self->context;
    }
}

void
lh_io_writer_assign(lh_io_writer_t *self, const lh_io_writer_t *other)
{
    lh_io_writer_write_cb write_cb;
    lh_ptr context;
    lh_io_writer_unpack(other, lh_addr_of(write_cb), lh_addr_of(context));
    lh_io_writer_set(self, write_cb, context);
}

void
lh_io_writer_set(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context)
{
    lh_io_writer_pack(self, lh_addr_of(write_cb), lh_addr_of(context));
}

void
lh_io_writer_init(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context)
{
    lh_io_writer_set(self, write_cb, context);
}

void
lh_io_writer_deinit(lh_io_writer_t *self)
{
    lh_io_writer_set(self, lh_null, lh_null);
}

lh_io_writer_write_cb
lh_io_writer_get_write_cb(const lh_io_writer_t *self)
{
    lh_io_writer_write_cb write_cb;
    lh_io_writer_unpack(self, lh_addr_of(write_cb), lh_null);
    return write_cb;
}

lh_ptr
lh_io_writer_get_context(const lh_io_writer_t *self)
{
    lh_ptr context;
    lh_io_writer_unpack(self, lh_null, lh_addr_of(context));
    return context;
}

lh_ssize_t
lh_io_writer_write(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_io_writer_write_cb write_cb = lh_io_writer_get_write_cb(self);
    lh_assert_runtime_ifn(write_cb,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    return write_cb(lh_io_writer_get_context(self), buf, size);
}
