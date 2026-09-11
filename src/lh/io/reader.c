#include <lh/io/reader.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

void
lh_io_reader_pack(lh_io_reader_t *self, lh_io_reader_read_cb *read_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(read_cb)
    {
        self->read_cb = lh_ptr_deref(read_cb);
    }

    lh_optional_ref(context)
    {
        self->context = lh_ptr_deref(context);
    }
}

void
lh_io_reader_unpack(const lh_io_reader_t *self, lh_io_reader_read_cb *read_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(read_cb)
    {
        lh_ptr_deref(read_cb) = self->read_cb;
    }

    lh_optional_ref(context)
    {
        lh_ptr_deref(context) = self->context;
    }
}

void
lh_io_reader_assign(lh_io_reader_t *self, const lh_io_reader_t *other)
{
    lh_io_reader_read_cb read_cb;
    lh_ptr context;
    lh_io_reader_unpack(other, lh_addr_of(read_cb), lh_addr_of(context));
    lh_io_reader_set(self, read_cb, context);
}

void
lh_io_reader_set(lh_io_reader_t *self, lh_io_reader_read_cb read_cb, lh_ptr context)
{
    lh_io_reader_pack(self, lh_addr_of(read_cb), lh_addr_of(context));
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
    lh_io_reader_read_cb read_cb;
    lh_io_reader_unpack(self, lh_addr_of(read_cb), lh_null);
    return read_cb;
}

lh_ptr
lh_io_reader_get_context(const lh_io_reader_t *self)
{
    lh_ptr context;
    lh_io_reader_unpack(self, lh_null, lh_addr_of(context));
    return context;
}

lh_ssize_t
lh_io_reader_read(lh_io_reader_t *self, lh_ptr buf, lh_usize_t size)
{
    lh_io_reader_read_cb read_cb = lh_io_reader_get_read_cb(self);
    lh_assert_runtime_ifn(read_cb,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    return read_cb(lh_io_reader_get_context(self), buf, size);
}
