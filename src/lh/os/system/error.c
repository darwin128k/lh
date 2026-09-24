#include <lh/os/system/error.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/memory/view.h>
#include <lh/os/system/error/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

void
lh_os_system_error_set_code(lh_os_system_error_t *self, lh_os_system_error_code_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

void
lh_os_system_error_set_desc(lh_os_system_error_t *self, lh_os_error_desc_t desc)
{
    lh_assert_runtime_ref(self);
    self->desc = desc;
}

void
lh_os_system_error_set(lh_os_system_error_t *self, lh_os_system_error_code_t code, lh_os_error_desc_t desc)
{
    lh_os_system_error_set_code(self, code);
    lh_os_system_error_set_desc(self, desc);
}

lh_os_system_error_code_t
lh_os_system_error_get_code(const lh_os_system_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_os_error_desc_t
lh_os_system_error_get_desc(const lh_os_system_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->desc;
}

lh_os_error_desc_t
lh_os_system_error_get_desc_or(const lh_os_system_error_t *self, lh_os_error_desc_t fallback)
{
    if (lh_os_system_error_has_desc(self))
    {
        return lh_os_system_error_get_desc(self);
    }
    return fallback;
}

void
lh_os_system_error_assign(lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    lh_os_system_error_set(self, lh_os_system_error_get_code(other), lh_os_system_error_get_desc(other));
}

void
lh_os_system_error_clear(lh_os_system_error_t *self)
{
    const lh_os_system_error_t empty_initializer = lh_os_system_error_empty_initializer();
    lh_os_system_error_assign(self, lh_addr_of(empty_initializer));
}

void
lh_os_system_error_init(lh_os_system_error_t *self, lh_os_system_error_code_t code, lh_os_error_desc_t desc)
{
    lh_os_system_error_set(self, code, desc);
}

void
lh_os_system_error_init_by_other(lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    lh_os_system_error_assign(self, other);
}

void
lh_os_system_error_init_by_empty(lh_os_system_error_t *self)
{
    lh_os_system_error_clear(self);
}

lh_os_system_error_code_t
lh_os_system_error_get_code_and_clear(lh_os_system_error_t *self)
{
    const lh_os_system_error_code_t code = lh_os_system_error_get_code(self);
    lh_os_system_error_clear(self);
    return code;
}

lh_os_system_error_t
lh_os_system_error_make(lh_os_system_error_code_t code, lh_os_error_desc_t desc)
{
    lh_os_system_error_t self;

    lh_os_system_error_init(lh_addr_of(self), code, desc);
    return self;
}

lh_os_system_error_t
lh_os_system_error_make_by_code(lh_os_system_error_code_t code)
{
    lh_os_system_error_t self;

    lh_os_system_error_init_by_empty(lh_addr_of(self));
    lh_os_system_error_set_code(lh_addr_of(self), code);
    return self;
}

lh_bool_t
lh_os_system_error_has_code(const lh_os_system_error_t *self, lh_os_system_error_code_t code)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_os_system_error_get_code(self), code));
}

lh_bool_t
lh_os_system_error_is_ok(const lh_os_system_error_t *self)
{
    return lh_os_system_error_has_code(self, lh_os_system_error_code_ok);
}

lh_bool_t
lh_os_system_error_is_failure(const lh_os_system_error_t *self)
{
    return lh_cast_static(lh_bool_t, !lh_os_system_error_is_ok(self));
}

lh_bool_t
lh_os_system_error_has_desc(const lh_os_system_error_t *self)
{
    const lh_os_error_desc_t desc = lh_os_system_error_get_desc(self);
    return lh_cast_static(lh_bool_t, !lh_memory_view_is_empty(lh_addr_of(desc)));
}

lh_bool_t
lh_os_system_error_is_empty(const lh_os_system_error_t *self)
{
    return lh_cast_static(lh_bool_t, lh_os_system_error_is_ok(self) && !lh_os_system_error_has_desc(self));
}

lh_bool_t
lh_os_system_error_has_same_code(const lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(lh_os_system_error_get_code(self), lh_os_system_error_get_code(other)));
}

lh_bool_t
lh_os_system_error_has_diff_code(const lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_os_system_error_has_same_code(self, other));
}

lh_bool_t
lh_os_system_error_equals(const lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    const lh_os_error_desc_t a = lh_os_system_error_get_desc(self);
    const lh_os_error_desc_t b = lh_os_system_error_get_desc(other);

    return lh_cast_static(lh_bool_t,
                          lh_os_system_error_has_same_code(self, other) &&
                              lh_math_eq(lh_memory_view_get_begin(lh_addr_of(a)), lh_memory_view_get_begin(lh_addr_of(b))) &&
                              lh_math_eq(lh_memory_view_get_end(lh_addr_of(a)), lh_memory_view_get_end(lh_addr_of(b))));
}
