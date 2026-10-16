#include <lh/error.h>
#include <lh/error/initializer.h>
#include <lh/str/view/initializer.h>
#include <lh/assert.h>

lh_void
lh_error_set_code(lh_error_t *self, lh_error_code_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

lh_void
lh_error_set_desc(lh_error_t *self, lh_str_view_t desc)
{
    lh_assert_runtime_ref(self);
    self->desc = desc;
}

lh_void
lh_error_set(lh_error_t *self, lh_error_code_t code, lh_str_view_t desc)
{
    lh_error_set_code(self, code);
    lh_error_set_desc(self, desc);
}

lh_error_code_t
lh_error_get_code(const lh_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_void
lh_error_get_desc(const lh_error_t *self, lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = self->desc;
}

lh_void
lh_error_assign(lh_error_t *self, const lh_error_t *other)
{
    lh_str_view_t view;
    lh_error_get_desc(other, lh_addr_of(view));
    lh_error_set(self, lh_error_get_code(other), view);
}

lh_void
lh_error_clear(lh_error_t *self)
{
    const lh_error_t empty_initializer = lh_error_empty_initializer(lh_str_view_empty_initializer());
    lh_error_assign(self, lh_addr_of(empty_initializer));
}

lh_void
lh_error_init(lh_error_t *self, lh_error_code_t code, lh_str_view_t desc)
{
    lh_error_set(self, code, desc);
}

lh_void
lh_error_init_by_other(lh_error_t *self, const lh_error_t *other)
{
    lh_error_assign(self, other);
}

lh_void
lh_error_init_by_empty(lh_error_t *self)
{
    lh_error_clear(self);
}

lh_void
lh_error_init_by_code(lh_error_t *self, lh_error_code_t code)
{
    lh_error_init(self, code, lh_str_view_empty());
}

lh_error_code_t
lh_error_get_code_and_clear(lh_error_t *self)
{
    const lh_error_code_t code = lh_error_get_code(self);
    lh_error_clear(self);
    return code;
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
    lh_str_view_t desc;
    lh_error_get_desc(self, lh_addr_of(desc));
    return !lh_str_view_is_empty(&desc);
}

lh_void
lh_error_get_desc_or(const lh_error_t *self, lh_str_view_t fallback, lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_error_has_desc(self))
    {
        lh_error_get_desc(self, out);
        return;
    }
    lh_ptr_deref(out) = fallback;
}

lh_bool_t
lh_error_is_empty(const lh_error_t *self)
{
    return lh_error_is_ok(self) && !lh_error_has_desc(self);
}

lh_bool_t
lh_error_equals(const lh_error_t *self, const lh_error_t *other)
{
    lh_str_view_t a;
    lh_error_get_desc(self, lh_addr_of(a));
    lh_str_view_t b;
    lh_error_get_desc(other, lh_addr_of(b));

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
