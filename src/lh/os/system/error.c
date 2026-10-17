#include <lh/os/system/error.h>
#include <lh/assert.h>
#include <lh/os/system/error/initializer.h>
#include <lh/math.h>

lh_void
lh_os_system_error_set_code(lh_os_system_error_t *self, lh_os_system_error_code_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

lh_void
lh_os_system_error_set_desc(lh_os_system_error_t *self, const lh_os_error_desc_ptr desc)
{
    lh_assert_runtime_ref(self);
    self->desc = desc;
}

lh_void
lh_os_system_error_set(lh_os_system_error_t *self, lh_os_system_error_code_t code,
                       const lh_os_error_desc_ptr desc)
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

const lh_os_error_desc_ptr
lh_os_system_error_get_desc(const lh_os_system_error_t *self)
{
    lh_assert_runtime_ref(self);
    return self->desc;
}

lh_void
lh_os_system_error_assign(lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    lh_os_system_error_set(self, lh_os_system_error_get_code(other),
                           lh_os_system_error_get_desc(other));
}

lh_void
lh_os_system_error_clear(lh_os_system_error_t *self)
{
    const lh_os_system_error_t empty_initializer = lh_os_system_error_empty_initializer();
    lh_os_system_error_assign(self, lh_addr_of(empty_initializer));
}

lh_void
lh_os_system_error_init(lh_os_system_error_t *self, lh_os_system_error_code_t code,
                        const lh_os_error_desc_ptr desc)
{
    lh_os_system_error_set(self, code, desc);
}

lh_void
lh_os_system_error_init_by_other(lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    lh_os_system_error_assign(self, other);
}

lh_void
lh_os_system_error_init_by_empty(lh_os_system_error_t *self)
{
    lh_os_system_error_clear(self);
}

lh_void
lh_os_system_error_init_by_code(lh_os_system_error_t *self, lh_os_system_error_code_t code)
{
    lh_os_system_error_init(self, code, lh_null);
}

lh_os_system_error_code_t
lh_os_system_error_get_code_and_clear(lh_os_system_error_t *self)
{
    const lh_os_system_error_code_t code = lh_os_system_error_get_code(self);
    lh_os_system_error_clear(self);
    return code;
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
    return lh_cast_static(lh_bool_t, !lh_os_error_desc_is_empty(lh_os_system_error_get_desc(self)));
}

const lh_os_error_desc_ptr
lh_os_system_error_get_desc_or(const lh_os_system_error_t *self,
                               const lh_os_error_desc_ptr fallback)
{
    return lh_os_system_error_has_desc(self) ? lh_os_system_error_get_desc(self) : fallback;
}

lh_bool_t
lh_os_system_error_is_empty(const lh_os_system_error_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_os_system_error_is_ok(self) && !lh_os_system_error_has_desc(self));
}

lh_bool_t
lh_os_system_error_has_same_code(const lh_os_system_error_t *self,
                                 const lh_os_system_error_t *other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_os_system_error_get_code(self),
                                                lh_os_system_error_get_code(other)));
}

lh_bool_t
lh_os_system_error_has_diff_code(const lh_os_system_error_t *self,
                                 const lh_os_system_error_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_os_system_error_has_same_code(self, other));
}

lh_bool_t
lh_os_system_error_equals(const lh_os_system_error_t *self, const lh_os_system_error_t *other)
{
    return lh_cast_static(lh_bool_t, lh_os_system_error_has_same_code(self, other) &&
                                         lh_ptr_eq(lh_os_system_error_get_desc(self),
                                                   lh_os_system_error_get_desc(other)));
}
