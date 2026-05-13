#include <lh/error.h>
#include <lh/error/initializer.h>
#include <lh/runtime/assert.h>
#include <lh/optional/ref.h>
#include <lh/util/addr.h>

void
lh_error_pack(lh_error_t *self, const lh_error_code_t *code, lh_error_desc_t *desc)
{
    lh_runtime_assert_ref(self);

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
lh_error_unpack(const lh_error_t *self, lh_error_code_t *code, lh_error_desc_t *desc)
{
    lh_runtime_assert_ref(self);

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
lh_error_set(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc)
{
    lh_error_pack(self, lh_addr_of(code), lh_addr_of(desc));
}

void
lh_error_set_code(lh_error_t *self, lh_error_code_t code)
{
    lh_error_pack(self, lh_addr_of(code), lh_null);
}

void
lh_error_set_desc(lh_error_t *self, lh_error_desc_t desc)
{
    lh_error_pack(self, lh_null, lh_addr_of(desc));
}

lh_error_code_t
lh_error_get_code(const lh_error_t *self)
{
    lh_error_code_t code;
    lh_error_unpack(self, lh_addr_of(code), lh_null);
    return code;
}

lh_error_desc_t
lh_error_get_desc(const lh_error_t *self)
{
    lh_error_desc_t desc;
    lh_error_unpack(self, lh_null, lh_addr_of(desc));
    return desc;
}

lh_error_desc_t
lh_error_get_desc_or(const lh_error_t *self, lh_error_desc_t fallback)
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
    lh_error_code_t code;
    lh_error_desc_t desc;

    lh_error_unpack(other, lh_addr_of(code), lh_addr_of(desc));
    lh_error_set(self, code, desc);
}

void
lh_error_clear(lh_error_t *self)
{
    const lh_error_t empty_initializer = lh_error_empty_initializer();
    lh_error_assign(self, lh_addr_of(empty_initializer));
}

void
lh_error_init(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc)
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
    return lh_error_get_desc(self) != lh_null;
}

lh_bool_t
lh_error_is_empty(const lh_error_t *self)
{
    return lh_error_is_ok(self) && !lh_error_has_desc(self);
}

lh_bool_t
lh_error_equals(const lh_error_t *self, const lh_error_t *other)
{
    return lh_error_get_code(self) == lh_error_get_code(other) &&
           lh_error_get_desc(self) == lh_error_get_desc(other);
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
