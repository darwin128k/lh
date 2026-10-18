#include <lh/str/scanf/spec.h>
#include <lh/assert.h>

lh_str_scanf_spec_kind_t
lh_str_scanf_spec_get_kind(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_str_scanf_spec_set_kind(lh_str_scanf_spec_t *self, lh_str_scanf_spec_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_bool_t
lh_str_scanf_spec_get_zero_pad(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->zero_pad;
}

lh_void
lh_str_scanf_spec_set_zero_pad(lh_str_scanf_spec_t *self, lh_bool_t zero_pad)
{
    lh_assert_runtime_ref(self);
    self->zero_pad = zero_pad;
}

lh_bool_t
lh_str_scanf_spec_get_left_justify(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left_justify;
}

lh_void
lh_str_scanf_spec_set_left_justify(lh_str_scanf_spec_t *self, lh_bool_t left_justify)
{
    lh_assert_runtime_ref(self);
    self->left_justify = left_justify;
}

lh_bool_t
lh_str_scanf_spec_get_uppercase(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->uppercase;
}

lh_void
lh_str_scanf_spec_set_uppercase(lh_str_scanf_spec_t *self, lh_bool_t uppercase)
{
    lh_assert_runtime_ref(self);
    self->uppercase = uppercase;
}

lh_uint_t
lh_str_scanf_spec_get_width(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_str_scanf_spec_set_width(lh_str_scanf_spec_t *self, lh_uint_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_bool_t
lh_str_scanf_spec_get_have_precision(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->have_precision;
}

lh_void
lh_str_scanf_spec_set_have_precision(lh_str_scanf_spec_t *self, lh_bool_t have_precision)
{
    lh_assert_runtime_ref(self);
    self->have_precision = have_precision;
}

lh_uint_t
lh_str_scanf_spec_get_precision(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->precision;
}

lh_void
lh_str_scanf_spec_set_precision(lh_str_scanf_spec_t *self, lh_uint_t precision)
{
    lh_assert_runtime_ref(self);
    self->precision = precision;
}

const lh_str_ptr
lh_str_scanf_spec_get_literal(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->literal;
}

lh_void
lh_str_scanf_spec_set_literal(lh_str_scanf_spec_t *self, const lh_str_ptr literal)
{
    lh_assert_runtime_ref(self);
    self->literal = literal;
}

lh_usize_t
lh_str_scanf_spec_get_literal_size(const lh_str_scanf_spec_t *self)
{
    lh_assert_runtime_ref(self);
    return self->literal_size;
}

lh_void
lh_str_scanf_spec_set_literal_size(lh_str_scanf_spec_t *self, lh_usize_t literal_size)
{
    lh_assert_runtime_ref(self);
    self->literal_size = literal_size;
}

lh_void
lh_str_scanf_spec_clear(lh_str_scanf_spec_t *self, const lh_str_ptr fmt)
{
    lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_invalid);
    lh_str_scanf_spec_set_zero_pad(self, lh_bool_false);
    lh_str_scanf_spec_set_left_justify(self, lh_bool_false);
    lh_str_scanf_spec_set_uppercase(self, lh_bool_false);
    lh_str_scanf_spec_set_width(self, 0U);
    lh_str_scanf_spec_set_have_precision(self, lh_bool_false);
    lh_str_scanf_spec_set_precision(self, 0U);
    lh_str_scanf_spec_set_literal(self, fmt);
    lh_str_scanf_spec_set_literal_size(self, 0U);
}

lh_usize_t
lh_str_scanf_spec_get_limit(const lh_str_scanf_spec_t *self, lh_usize_t remaining)
{
    const lh_uint_t width = lh_str_scanf_spec_get_width(self);
    return width > 0U && width < remaining ? width : remaining;
}

lh_void
lh_str_scanf_spec_set_conversion(lh_str_scanf_spec_t *self, lh_char_t conversion)
{
    switch (conversion)
    {
    case '%':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_percent);
        break;
    case 'c':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_char);
        break;
    case 's':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_str);
        break;
    case 'u':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_uint);
        break;
    case 'd':
    case 'i':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_sint);
        break;
    case 'x':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_hex);
        break;
    case 'X':
        lh_str_scanf_spec_set_kind(self, lh_str_scanf_spec_kind_hex);
        lh_str_scanf_spec_set_uppercase(self, lh_bool_true);
        break;
    default:
        break; /* kind stays invalid */
    }
}
