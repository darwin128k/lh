#include <lh/error.h>
#include <lh/error/initializer.h>
#include <lh/util/addr.h>
#include <lh/assert.h>

void
lh_error_set(lh_error_t *self, lh_error_code_t code, lh_str_view_t desc)
{
    lh_error_set_code(self, code);
    lh_error_set_desc(self, desc);
}

void
lh_error_set_code(lh_error_t *self, lh_error_code_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

void
lh_error_set_desc(lh_error_t *self, lh_str_view_t desc)
{
    lh_assert_runtime_ref(self);
    self->desc = desc;
}

lh_error_code_t
lh_error_get_code(const lh_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_str_view_t
lh_error_get_desc(const lh_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->desc;
}

lh_str_view_t
lh_error_get_desc_or(const lh_error_t *self, lh_str_view_t fallback)
{
    if (lh_error_has_desc(self))
    {
        return lh_error_get_desc(self);
    }
    return fallback;
}

void
lh_error_assign(lh_error_t *self, const lh_error_t *other)
{
    lh_error_set(self, lh_error_get_code(other), lh_error_get_desc(other));
}

void
lh_error_clear(lh_error_t *self)
{
    const lh_error_t empty_initializer = lh_error_empty_initializer();
    lh_error_assign(self, lh_addr_of(empty_initializer));
}

void
lh_error_init(lh_error_t *self, lh_error_code_t code, lh_str_view_t desc)
{
    lh_error_set(self, code, desc);
}

void
lh_error_init_by_other(lh_error_t *self, const lh_error_t *other)
{
    lh_error_assign(self, other);
}

void
lh_error_init_by_empty(lh_error_t *self)
{
    lh_error_clear(self);
}

lh_error_code_t
lh_error_get_code_and_clear(lh_error_t *self)
{
    const lh_error_code_t code = lh_error_get_code(self);
    lh_error_clear(self);
    return code;
}

lh_error_t
lh_error_make(lh_error_code_t code, lh_str_view_t desc)
{
    lh_error_t self;
    lh_error_init(lh_addr_of(self), code, desc);
    return self;
}

lh_error_t
lh_error_make_by_code(lh_error_code_t code)
{
    lh_error_t self;
    lh_error_init_by_empty(lh_addr_of(self));
    lh_error_set_code(lh_addr_of(self), code);
    return self;
}

lh_bool_t
lh_error_has_code(const lh_error_t *self, lh_error_code_t code)
{
    return lh_error_get_code(self) == code;
}

lh_bool_t
lh_error_is_ok(const lh_error_t *self)
{
    return lh_error_has_code(self, lh_error_code_ok);
}

lh_bool_t
lh_error_is_failure(const lh_error_t *self)
{
    return !lh_error_is_ok(self);
}

lh_bool_t
lh_error_has_desc(const lh_error_t *self)
{
    const lh_str_view_t desc = lh_error_get_desc(self);
    return !lh_str_view_is_empty(&desc);
}

lh_bool_t
lh_error_is_empty(const lh_error_t *self)
{
    return lh_error_is_ok(self) && !lh_error_has_desc(self);
}

lh_bool_t
lh_error_equals(const lh_error_t *self, const lh_error_t *other)
{
    const lh_str_view_t a = lh_error_get_desc(self);
    const lh_str_view_t b = lh_error_get_desc(other);

    return lh_error_get_code(self) == lh_error_get_code(other) &&
           lh_memory_view_get_begin(&a) == lh_memory_view_get_begin(&b) &&
           lh_memory_view_get_end(&a) == lh_memory_view_get_end(&b);
}

lh_bool_t
lh_error_has_same_code(const lh_error_t *self, const lh_error_t *other)
{
    return lh_error_get_code(self) == lh_error_get_code(other);
}

lh_bool_t
lh_error_has_diff_code(const lh_error_t *self, const lh_error_t *other)
{
    return !lh_error_has_same_code(self, other);
}
