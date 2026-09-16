#include <lh/werror.h>
#include <lh/assert.h>
#include <lh/optional/ref.h>
#include <lh/util/addr.h>
#include <lh/werror/initializer.h>

void
lh_werror_pack(lh_werror_t *self, const lh_error_code_t *code, lh_werror_desc_t *desc)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(code)
    {
        self->code = lh_ptr_deref(code);
    }

    lh_optional_ref(desc)
    {
        self->desc = lh_ptr_deref(desc);
    }
}

void
lh_werror_unpack(const lh_werror_t *self, lh_error_code_t *code, lh_werror_desc_t *desc)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(code)
    {
        lh_ptr_deref(code) = self->code;
    }

    lh_optional_ref(desc)
    {
        lh_ptr_deref(desc) = self->desc;
    }
}

void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc)
{
    lh_werror_pack(self, lh_addr_of(code), lh_addr_of(desc));
}

void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code)
{
    lh_werror_pack(self, lh_addr_of(code), lh_null);
}

void
lh_werror_set_desc(lh_werror_t *self, lh_werror_desc_t desc)
{
    lh_werror_pack(self, lh_null, lh_addr_of(desc));
}

lh_error_code_t
lh_werror_get_code(const lh_werror_t *self)
{
    lh_error_code_t code;
    lh_werror_unpack(self, lh_addr_of(code), lh_null);
    return code;
}

lh_werror_desc_t
lh_werror_get_desc(const lh_werror_t *self)
{
    lh_werror_desc_t desc;
    lh_werror_unpack(self, lh_null, lh_addr_of(desc));
    return desc;
}

lh_werror_desc_t
lh_werror_get_desc_or(const lh_werror_t *self, lh_werror_desc_t fallback)
{
    if (lh_werror_has_desc(self))
    {
        return lh_werror_get_desc(self);
    }
    return fallback;
}

void
lh_werror_unpack_to_other(const lh_werror_t *self, lh_werror_t *other)
{
    lh_werror_assign(other, self);
}

void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other)
{
    lh_error_code_t code;
    lh_werror_desc_t desc;

    lh_werror_unpack(other, lh_addr_of(code), lh_addr_of(desc));
    lh_werror_set(self, code, desc);
}

void
lh_werror_clear(lh_werror_t *self)
{
    const lh_werror_t empty_initializer = lh_werror_empty_initializer();
    lh_werror_assign(self, lh_addr_of(empty_initializer));
}

void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc)
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
lh_werror_make(lh_error_code_t code, lh_werror_desc_t desc)
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
    const lh_werror_desc_t desc = lh_werror_get_desc(self);
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
    const lh_werror_desc_t a = lh_werror_get_desc(self);
    const lh_werror_desc_t b = lh_werror_get_desc(other);

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
