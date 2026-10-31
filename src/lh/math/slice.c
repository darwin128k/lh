/**
 * @file slice.c
 * @brief Implementation of `lh/math/slice.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math/slice.h>
#include <lh/util/interval.h>
#include <lh/util/return.h>

lh_void
lh_math_slice_set_begin(lh_math_slice_t *self, const lh_float_t *first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_void
lh_math_slice_set_end(lh_math_slice_t *self, const lh_float_t *last)
{
    lh_assert_runtime_ref(self);
    self->second = last;
}

const lh_float_t *
lh_math_slice_get_begin(const lh_math_slice_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

const lh_float_t *
lh_math_slice_get_end(const lh_math_slice_t *self)
{
    lh_assert_runtime_ref(self);
    return self->second;
}

lh_void
lh_math_slice_set(lh_math_slice_t *self, const lh_float_t *first, const lh_float_t *last)
{
    lh_math_slice_set_begin(self, first);
    lh_math_slice_set_end(self, last);
}

lh_bool_t
lh_math_slice_is_empty(const lh_math_slice_t *self)
{
    return lh_null_eq(lh_math_slice_get_begin(self)) && lh_null_eq(lh_math_slice_get_end(self));
}

lh_void
lh_math_slice_init(lh_math_slice_t *self, const lh_float_t *data, lh_usize_t count)
{
    const lh_bool_t empty = lh_null_eq(data) || count == 0U;

    lh_math_slice_set(self, empty ? lh_null : data, empty ? lh_null : data + (count - 1U));
}

lh_void
lh_math_slice_init_empty(lh_math_slice_t *self)
{
    lh_math_slice_set(self, lh_null, lh_null);
}

lh_void
lh_math_slice_init_by_other(lh_math_slice_t *self, const lh_math_slice_t *other)
{
    lh_math_slice_set(self, lh_math_slice_get_begin(other), lh_math_slice_get_end(other));
}

lh_bool_t
lh_math_slice_is_valid(const lh_math_slice_t *self)
{
    lh_return_if(lh_math_slice_is_empty(self), lh_bool_true);
    return lh_null_ne(lh_math_slice_get_begin(self)) && lh_null_ne(lh_math_slice_get_end(self)) &&
           lh_interval_closed_is_valid(lh_math_slice_get_begin(self), lh_math_slice_get_end(self));
}

lh_usize_t
lh_math_slice_get_size(const lh_math_slice_t *self)
{
    lh_assert_runtime_ifn(lh_math_slice_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_return_if(lh_math_slice_is_empty(self), 0U);
    return lh_cast_static(lh_usize_t, lh_interval_closed_get_size(lh_math_slice_get_begin(self), lh_math_slice_get_end(self)));
}

lh_bool_t
lh_math_slice_is_valid_index(const lh_math_slice_t *self, lh_usize_t index)
{
    return index < lh_math_slice_get_size(self) &&
           lh_interval_closed_contains_value(lh_math_slice_get_begin(self), lh_math_slice_get_end(self), lh_math_slice_get_begin(self) + index);
}

lh_void
lh_math_slice_get_sub(const lh_math_slice_t *self, lh_usize_t offset, lh_usize_t count,
                      lh_math_slice_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_usize_t size = lh_math_slice_get_size(self);
    lh_math_slice_t sub;

    lh_assert_runtime_ifn(offset <= size && count <= size - offset,
                          lh_runtime_error_code_out_of_range);
    lh_math_slice_init(lh_addr_of(sub), count == 0U ? lh_null : lh_math_slice_get_begin(self) + offset, count);
    lh_ptr_deref(out) = sub;
}

lh_float_t
lh_math_slice_get_at(const lh_math_slice_t *self, lh_usize_t index)
{
    lh_assert_runtime_ifn(lh_math_slice_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);
    return lh_math_slice_get_begin(self)[index];
}

lh_void
lh_math_slice_take_first(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out)
{
    lh_math_slice_get_sub(self, 0U, n, out);
}

lh_void
lh_math_slice_take_last(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out)
{
    lh_assert_runtime_ifn(n <= lh_math_slice_get_size(self), lh_runtime_error_code_out_of_range);
    lh_math_slice_get_sub(self, lh_math_slice_get_size(self) - n, n, out);
}

lh_void
lh_math_slice_drop_first(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out)
{
    lh_assert_runtime_ifn(n <= lh_math_slice_get_size(self), lh_runtime_error_code_out_of_range);
    lh_math_slice_get_sub(self, n, lh_math_slice_get_size(self) - n, out);
}

lh_void
lh_math_slice_drop_last(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out)
{
    lh_assert_runtime_ifn(n <= lh_math_slice_get_size(self), lh_runtime_error_code_out_of_range);
    lh_math_slice_get_sub(self, 0U, lh_math_slice_get_size(self) - n, out);
}

lh_bool_t
lh_math_slice_equals(const lh_math_slice_t *a, const lh_math_slice_t *b)
{
    lh_return_if(lh_math_slice_get_size(a) != lh_math_slice_get_size(b), lh_bool_false);
    for (lh_usize_t i = 0U; i < lh_math_slice_get_size(a); ++i)
    {
        lh_return_if(lh_math_slice_get_at(a, i) != lh_math_slice_get_at(b, i), lh_bool_false);
    }
    return lh_bool_true;
}

lh_float_t
lh_math_slice_sum(const lh_math_slice_t *self)
{
    /* From the first element, not from 0: 0 + -0 is +0, and a sum written out by
       hand (x + y + z) keeps the sign of an all-negative-zero row. */
    lh_return_if(lh_math_slice_is_empty(self), 0.0f);

    lh_float_t sum = lh_math_slice_get_at(self, 0U);

    for (lh_usize_t i = 1U; i < lh_math_slice_get_size(self); ++i)
    {
        sum += lh_math_slice_get_at(self, i);
    }
    return sum;
}

lh_float_t
lh_math_slice_dot(const lh_math_slice_t *a, const lh_math_slice_t *b)
{
    lh_assert_runtime_ifn(lh_math_slice_get_size(a) == lh_math_slice_get_size(b),
                          lh_runtime_error_code_invalid_range);
    lh_return_if(lh_math_slice_is_empty(a), 0.0f);

    lh_float_t sum = lh_math_slice_get_at(a, 0U) * lh_math_slice_get_at(b, 0U);

    for (lh_usize_t i = 1U; i < lh_math_slice_get_size(a); ++i)
    {
        sum += lh_math_slice_get_at(a, i) * lh_math_slice_get_at(b, i);
    }
    return sum;
}
