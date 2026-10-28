/**
 * @file view.c
 * @brief Implementation of `lh/math/view.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math/float/sqrt.h>
#include <lh/math/view.h>
#include <lh/util/interval.h>
#include <lh/util/return.h>

const lh_math_slice_t *
lh_math_view_get_slice(const lh_math_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->slice);
}

lh_math_slice_t *
lh_math_view_get_slice_mut(lh_math_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->slice);
}

lh_usize_t
lh_math_view_get_stride(const lh_math_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stride;
}

lh_void
lh_math_view_set_stride(lh_math_view_t *self, lh_usize_t stride)
{
    lh_assert_runtime_ref(self);
    self->stride = stride;
}

lh_void
lh_math_view_set_count(lh_math_view_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_usize_t
lh_math_view_get_span(lh_usize_t stride, lh_usize_t count)
{
    lh_return_if(count == 0U, 0U);
    return lh_interval_closed_get_size(0U, (count - 1U) * stride);
}

lh_usize_t
lh_math_view_get_size(const lh_math_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_math_view_init(lh_math_view_t *self, const lh_math_slice_t *base, lh_usize_t offset,
                  lh_usize_t stride, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(stride != 0U, lh_runtime_error_code_invalid_range);
    lh_math_slice_get_sub(base, offset, lh_math_view_get_span(stride, count),
                          lh_math_view_get_slice_mut(self));
    lh_math_view_set_stride(self, stride);
    lh_math_view_set_count(self, count);
}

lh_bool_t
lh_math_view_is_empty(const lh_math_view_t *self)
{
    return lh_math_view_get_size(self) == 0U;
}

lh_float_t
lh_math_view_get_at(const lh_math_view_t *self, lh_usize_t index)
{
    lh_assert_runtime_ifn(lh_interval_ropen_contains_value(0U, lh_math_view_get_size(self), index),
                          lh_runtime_error_code_out_of_range);
    return lh_math_slice_get_at(lh_math_view_get_slice(self),
                                index * lh_math_view_get_stride(self));
}

lh_void
lh_math_view_init_by_slice(lh_math_view_t *self, const lh_math_slice_t *base)
{
    lh_math_view_init(self, base, 0U, 1U, lh_math_slice_get_size(base));
}

lh_float_t
lh_math_view_dot(const lh_math_view_t *a, const lh_math_view_t *b)
{
    lh_float_t sum;
    lh_usize_t i;

    lh_assert_runtime_ifn(lh_math_view_get_size(a) == lh_math_view_get_size(b),
                          lh_runtime_error_code_invalid_range);
    lh_return_if(lh_math_view_is_empty(a), 0.0f);
    for (sum = lh_math_view_get_at(a, 0U) * lh_math_view_get_at(b, 0U), i = 1U;
         i < lh_math_view_get_size(a); ++i)
    {
        sum += lh_math_view_get_at(a, i) * lh_math_view_get_at(b, i);
    }
    return sum;
}

lh_float_t
lh_math_view_length_sq(const lh_math_view_t *self)
{
    return lh_math_view_dot(self, self);
}

lh_float_t
lh_math_view_length(const lh_math_view_t *self)
{
    return lh_math_float_sqrt(lh_math_view_length_sq(self));
}
