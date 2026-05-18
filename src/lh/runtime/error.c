#include <lh/runtime/error.h>
#include <lh/util/ptr.h>
#include <lh/error.h>

void
lh_runtime_error_pack(lh_runtime_error_t *self, const lh_runtime_error_code_t *code,
                      lh_runtime_error_desc_t *desc)
{
    lh_error_pack(lh_ptr_cast(lh_error_t, self), code, desc);
}

void
lh_runtime_error_unpack(const lh_runtime_error_t *self, lh_runtime_error_code_t *code,
                        lh_runtime_error_desc_t *desc)
{
    lh_error_unpack(lh_ptr_ccast(lh_error_t, self), code, desc);
}

void
lh_runtime_error_set(lh_runtime_error_t *self, lh_runtime_error_code_t code,
                     lh_runtime_error_desc_t desc)
{
    lh_error_set(lh_ptr_cast(lh_error_t, self), code, desc);
}

void
lh_runtime_error_set_code(lh_runtime_error_t *self, lh_runtime_error_code_t code)
{
    lh_error_set_code(lh_ptr_cast(lh_error_t, self), code);
}

void
lh_runtime_error_set_desc(lh_runtime_error_t *self, lh_runtime_error_desc_t desc)
{
    lh_error_set_desc(lh_ptr_cast(lh_error_t, self), desc);
}

lh_runtime_error_code_t
lh_runtime_error_get_code(const lh_runtime_error_t *self)
{
    return lh_error_get_code(lh_ptr_ccast(lh_error_t, self));
}

lh_runtime_error_desc_t
lh_runtime_error_get_desc(const lh_runtime_error_t *self)
{
    return lh_error_get_desc(lh_ptr_ccast(lh_error_t, self));
}

lh_runtime_error_desc_t
lh_runtime_error_get_desc_or(const lh_runtime_error_t *self, lh_runtime_error_desc_t fallback)
{
    return lh_error_get_desc_or(lh_ptr_ccast(lh_error_t, self), fallback);
}

lh_bool_t
lh_runtime_error_has_code(const lh_runtime_error_t *self, lh_runtime_error_code_t code)
{
    return lh_error_has_code(lh_ptr_ccast(lh_error_t, self), code);
}

lh_bool_t
lh_runtime_error_is_ok(const lh_runtime_error_t *self)
{
    return lh_error_is_ok(lh_ptr_ccast(lh_error_t, self));
}

lh_bool_t
lh_runtime_error_is_failure(const lh_runtime_error_t *self)
{
    return lh_error_is_failure(lh_ptr_ccast(lh_error_t, self));
}

lh_bool_t
lh_runtime_error_has_desc(const lh_runtime_error_t *self)
{
    return lh_error_has_desc(lh_ptr_ccast(lh_error_t, self));
}

lh_bool_t
lh_runtime_error_is_empty(const lh_runtime_error_t *self)
{
    return lh_error_is_empty(lh_ptr_ccast(lh_error_t, self));
}

lh_bool_t
lh_runtime_error_equals(const lh_runtime_error_t *self, const lh_runtime_error_t *other)
{
    return lh_error_equals(lh_ptr_ccast(lh_error_t, self), lh_ptr_ccast(lh_error_t, other));
}

lh_bool_t
lh_runtime_error_has_same_code(const lh_runtime_error_t *self, const lh_runtime_error_t *other)
{
    return lh_error_has_same_code(lh_ptr_ccast(lh_error_t, self), lh_ptr_ccast(lh_error_t, other));
}

lh_bool_t
lh_runtime_error_has_diff_code(const lh_runtime_error_t *self, const lh_runtime_error_t *other)
{
    return lh_error_has_diff_code(lh_ptr_ccast(lh_error_t, self), lh_ptr_ccast(lh_error_t, other));
}

void
lh_runtime_error_assign(lh_runtime_error_t *self, const lh_runtime_error_t *other)
{
    lh_error_assign(lh_ptr_cast(lh_error_t, self), lh_ptr_ccast(lh_error_t, other));
}

void
lh_runtime_error_unpack_to_other(const lh_runtime_error_t *self, lh_runtime_error_t *other)
{
    lh_runtime_error_assign(other, self);
}

void
lh_runtime_error_clear(lh_runtime_error_t *self)
{
    lh_error_clear(lh_ptr_cast(lh_error_t, self));
}

void
lh_runtime_error_init(lh_runtime_error_t *self, lh_runtime_error_code_t code,
                      lh_runtime_error_desc_t desc)
{
    lh_error_init(lh_ptr_cast(lh_error_t, self), code, desc);
}

void
lh_runtime_error_init_by_other(lh_runtime_error_t *self, const lh_runtime_error_t *other)
{
    lh_error_init_by_other(lh_ptr_cast(lh_error_t, self), lh_ptr_ccast(lh_error_t, other));
}

void
lh_runtime_error_init_by_empty(lh_runtime_error_t *self)
{
    lh_error_init_by_empty(lh_ptr_cast(lh_error_t, self));
}

lh_runtime_error_code_t
lh_runtime_error_get_code_and_clear(lh_runtime_error_t *self)
{
    return lh_error_get_code_and_clear(lh_ptr_cast(lh_error_t, self));
}

lh_runtime_error_t
lh_runtime_error_make(lh_runtime_error_code_t code, lh_runtime_error_desc_t desc)
{
    lh_runtime_error_t self;
    lh_runtime_error_init(lh_addr_of(self), code, desc);
    return self;
}

lh_runtime_error_t
lh_runtime_error_make_by_code(lh_runtime_error_code_t code)
{
    return lh_runtime_error_make(code, lh_null);
}

lh_runtime_error_t
lh_runtime_error_make_by_desc(lh_runtime_error_desc_t desc)
{
    return lh_runtime_error_make(lh_runtime_error_code_interrupt, desc);
}
