#include <lh/exception/catch.h>
#include <lh/assert.h>
#include <lh/util/ptr.h>

lh_exception_t *
lh_exception_catch_get_exception(const lh_exception_catch_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_cast(lh_exception_t, lh_addr_of(self->exception));
}

lh_error_t *
lh_exception_catch_get_error(const lh_exception_catch_t *self)
{
    return lh_exception_get_error(lh_exception_catch_get_exception(self));
}

lh_error_code_t
lh_exception_catch_get_code(const lh_exception_catch_t *self)
{
    return lh_exception_get_code(lh_exception_catch_get_exception(self));
}

lh_error_desc_t
lh_exception_catch_get_desc(const lh_exception_catch_t *self)
{
    return lh_exception_get_desc(lh_exception_catch_get_exception(self));
}

lh_error_desc_t
lh_exception_catch_get_desc_or(const lh_exception_catch_t *self, lh_error_desc_t fallback)
{
    return lh_exception_get_desc_or(lh_exception_catch_get_exception(self), fallback);
}

lh_bool_t
lh_exception_catch_has_code(const lh_exception_catch_t *self, lh_error_code_t code)
{
    return lh_exception_has_code(lh_exception_catch_get_exception(self), code);
}

lh_bool_t
lh_exception_catch_is_ok(const lh_exception_catch_t *self)
{
    return lh_exception_is_ok(lh_exception_catch_get_exception(self));
}

lh_bool_t
lh_exception_catch_is_failure(const lh_exception_catch_t *self)
{
    return lh_exception_is_failure(lh_exception_catch_get_exception(self));
}

lh_bool_t
lh_exception_catch_has_desc(const lh_exception_catch_t *self)
{
    return lh_exception_has_desc(lh_exception_catch_get_exception(self));
}

lh_bool_t
lh_exception_catch_is_empty(const lh_exception_catch_t *self)
{
    return lh_exception_is_empty(lh_exception_catch_get_exception(self));
}

lh_bool_t
lh_exception_catch_equals(const lh_exception_catch_t *self, const lh_exception_catch_t *other)
{
    return lh_exception_equals(lh_exception_catch_get_exception(self),
                               lh_exception_catch_get_exception(other));
}

lh_bool_t
lh_exception_catch_has_same_code(const lh_exception_catch_t *self,
                                 const lh_exception_catch_t *other)
{
    return lh_exception_has_same_code(lh_exception_catch_get_exception(self),
                                      lh_exception_catch_get_exception(other));
}

lh_bool_t
lh_exception_catch_has_diff_code(const lh_exception_catch_t *self,
                                 const lh_exception_catch_t *other)
{
    return lh_exception_has_diff_code(lh_exception_catch_get_exception(self),
                                      lh_exception_catch_get_exception(other));
}
