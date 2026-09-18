#include <lh/werror.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/werror/initializer.h>

void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, lh_wstr_view_t desc)
{
    lh_werror_set_code(self, code);
    lh_werror_set_desc(self, desc);
}

void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

void
lh_werror_set_desc(lh_werror_t *self, lh_wstr_view_t desc)
{
    lh_assert_runtime_ref(self);
    self->desc = desc;
}

lh_error_code_t
lh_werror_get_code(const lh_werror_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_wstr_view_t
lh_werror_get_desc(const lh_werror_t *self)
{
    lh_assert_runtime_ref(self);
    return self->desc;
}

lh_wstr_view_t
lh_werror_get_desc_or(const lh_werror_t *self, lh_wstr_view_t fallback)
{
    if (lh_werror_has_desc(self))
    {
        return lh_werror_get_desc(self);
    }
    return fallback;
}

void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other)
{
    lh_werror_set(self, lh_werror_get_code(other), lh_werror_get_desc(other));
}

void
lh_werror_clear(lh_werror_t *self)
{
    const lh_werror_t empty_initializer = lh_werror_empty_initializer();
    lh_werror_assign(self, lh_addr_of(empty_initializer));
}

void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, lh_wstr_view_t desc)
{
    lh_werror_set(self, code, desc);
}

void
lh_werror_init_by_other(lh_werror_t *self, const lh_werror_t *other)
{
    lh_werror_assign(self, other);
}

void
lh_werror_init_by_empty(lh_werror_t *self)
{
    lh_werror_clear(self);
}

lh_error_code_t
lh_werror_get_code_and_clear(lh_werror_t *self)
{
    const lh_error_code_t code = lh_werror_get_code(self);
    lh_werror_clear(self);
    return code;
}

lh_werror_t
lh_werror_make(lh_error_code_t code, lh_wstr_view_t desc)
{
    lh_werror_t self;
    lh_werror_init(lh_addr_of(self), code, desc);
    return self;
}

lh_werror_t
lh_werror_make_by_code(lh_error_code_t code)
{
    lh_werror_t self;
    lh_werror_init_by_empty(lh_addr_of(self));
    lh_werror_set_code(lh_addr_of(self), code);
    return self;
}

lh_bool_t
lh_werror_has_code(const lh_werror_t *self, lh_error_code_t code)
{
    return lh_werror_get_code(self) == code;
}

lh_bool_t
lh_werror_is_ok(const lh_werror_t *self)
{
    return lh_werror_has_code(self, lh_error_code_ok);
}

lh_bool_t
lh_werror_is_failure(const lh_werror_t *self)
{
    return !lh_werror_is_ok(self);
}

lh_bool_t
lh_werror_has_desc(const lh_werror_t *self)
{
    const lh_wstr_view_t desc = lh_werror_get_desc(self);
    return !lh_wstr_view_is_empty(&desc);
}

lh_bool_t
lh_werror_is_empty(const lh_werror_t *self)
{
    return lh_werror_is_ok(self) && !lh_werror_has_desc(self);
}

lh_bool_t
lh_werror_equals(const lh_werror_t *self, const lh_werror_t *other)
{
    const lh_wstr_view_t a = lh_werror_get_desc(self);
    const lh_wstr_view_t b = lh_werror_get_desc(other);

    return lh_werror_get_code(self) == lh_werror_get_code(other) &&
           lh_memory_view_get_begin(&a) == lh_memory_view_get_begin(&b) &&
           lh_memory_view_get_end(&a) == lh_memory_view_get_end(&b);
}

lh_bool_t
lh_werror_has_same_code(const lh_werror_t *self, const lh_werror_t *other)
{
    return lh_werror_get_code(self) == lh_werror_get_code(other);
}

lh_bool_t
lh_werror_has_diff_code(const lh_werror_t *self, const lh_werror_t *other)
{
    return !lh_werror_has_same_code(self, other);
}
