#include <lh/exception/origin.h>
#include <lh/optional/ref.h>
#include <lh/assert.h>

void
lh_exception_origin_pack(lh_exception_origin_t *self, const lh_str_ptr *timestamp,
                         const lh_str_ptr *file, const lh_str_ptr *function,
                         const lh_usize_t *line)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(timestamp)
    {
        self->timestamp = lh_ptr_deref(timestamp);
    }
    lh_optional_ref(file)
    {
        self->filename = lh_ptr_deref(file);
    }
    lh_optional_ref(function)
    {
        self->function = lh_ptr_deref(function);
    }
    lh_optional_ref(line)
    {
        self->line = lh_ptr_deref(line);
    }
}

void
lh_exception_origin_set(lh_exception_origin_t *self, const lh_str_ptr timestamp,
                        const lh_str_ptr file, const lh_str_ptr function, lh_usize_t line)
{
    lh_exception_origin_pack(self, lh_addr_of(timestamp), lh_addr_of(file), lh_addr_of(function),
                             lh_addr_of(line));
}

void
lh_exception_origin_init(lh_exception_origin_t *self, const lh_str_ptr timestamp,
                         const lh_str_ptr file, const lh_str_ptr function, lh_usize_t line)
{
    lh_exception_origin_set(self, timestamp, file, function, line);
}

void
lh_exception_origin_unpack(const lh_exception_origin_t *self, const lh_str_ptr *timestamp,
                           const lh_str_ptr *file, const lh_str_ptr *function, lh_usize_t *line)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(timestamp)
    {
        lh_ptr_deref(timestamp) = self->timestamp;
    }
    lh_optional_ref(file)
    {
        lh_ptr_deref(file) = self->filename;
    }
    lh_optional_ref(function)
    {
        lh_ptr_deref(function) = self->function;
    }
    lh_optional_ref(line)
    {
        lh_ptr_deref(line) = self->line;
    }
}

lh_str_cptr
lh_exception_origin_get_timestamp(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timestamp;
}

lh_str_cptr
lh_exception_origin_get_file(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->filename;
}

lh_str_cptr
lh_exception_origin_get_function(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->function;
}

lh_usize_t
lh_exception_origin_get_line(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}

void
lh_exception_origin_assign(lh_exception_origin_t *self, const lh_exception_origin_t *other)
{
    lh_assert_runtime_ref(other);
    lh_exception_origin_set(self, other->timestamp, other->filename, other->function, other->line);
}

void
lh_exception_origin_init_by_other(lh_exception_origin_t *self, const lh_exception_origin_t *other)
{
    lh_exception_origin_assign(self, other);
}

void
lh_exception_origin_unpack_to_other(const lh_exception_origin_t *self, lh_exception_origin_t *other)
{
    lh_exception_origin_assign(other, self);
}
