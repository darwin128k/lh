#include <lh/exception.h>
#include <lh/util/ptr.h>
#include <lh/runtime/assert.h>

lh_error_t *
lh_exception_get_error(const lh_exception_t *self)
{
    lh_runtime_assert_ref(self);
    return lh_ptr_cast(lh_error_t, lh_addr_of(self->error));
}

#ifndef NDEBUG
lh_exception_origin_t *
lh_exception_get_origin(const lh_exception_t *self)
{
    lh_runtime_assert_ref(self);
    return lh_ptr_cast(lh_exception_origin_t, lh_addr_of(self->origin));
}
#endif

void
lh_exception_set_code(lh_exception_t *self, lh_error_code_t code)
{
    lh_error_set_code(lh_exception_get_error(self), code);
}

void
lh_exception_set_desc(lh_exception_t *self, lh_error_desc_t desc)
{
    lh_error_set_desc(lh_exception_get_error(self), desc);
}

lh_error_code_t
lh_exception_get_code(const lh_exception_t *self)
{
    return lh_error_get_code(lh_exception_get_error(self));
}

lh_error_desc_t
lh_exception_get_desc(const lh_exception_t *self)
{
    return lh_error_get_desc(lh_exception_get_error(self));
}

lh_error_desc_t
lh_exception_get_desc_or(const lh_exception_t *self, lh_error_desc_t fallback)
{
    return lh_error_get_desc_or(lh_exception_get_error(self), fallback);
}

lh_bool_t
lh_exception_has_code(const lh_exception_t *self, lh_error_code_t code)
{
    return lh_error_has_code(lh_exception_get_error(self), code);
}

lh_bool_t
lh_exception_is_ok(const lh_exception_t *self)
{
    return lh_error_is_ok(lh_exception_get_error(self));
}

lh_bool_t
lh_exception_is_failure(const lh_exception_t *self)
{
    return lh_error_is_failure(lh_exception_get_error(self));
}

lh_bool_t
lh_exception_has_desc(const lh_exception_t *self)
{
    return lh_error_has_desc(lh_exception_get_error(self));
}

lh_bool_t
lh_exception_is_empty(const lh_exception_t *self)
{
    return lh_error_is_empty(lh_exception_get_error(self));
}

lh_bool_t
lh_exception_equals(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_equals(lh_exception_get_error(self), lh_exception_get_error(other));
}

lh_bool_t
lh_exception_has_same_code(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_has_same_code(lh_exception_get_error(self), lh_exception_get_error(other));
}

lh_bool_t
lh_exception_has_diff_code(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_has_diff_code(lh_exception_get_error(self), lh_exception_get_error(other));
}
