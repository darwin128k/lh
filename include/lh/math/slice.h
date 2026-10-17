/**
 * @file slice.h
 * @brief Read-only closed slice of ::lh_float_t values: ::lh_math_slice_t.
 *
 * A slice names elements it does not own: `first` and `second` point to its first
 * and its last element, the closed interval `[first, second]` of
 * ::lh_memory_view_slice_t over floats instead of bytes, so its size is
 * `second - first + 1` (::lh_interval_closed_get_size). An empty slice has both
 * bounds ::lh_null. Taking or dropping elements makes a new slice over the same
 * data; nothing is ever copied.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_SLICE_H
#define LH_MATH_SLICE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/slice/fields.h>
#include <lh/numeric/float.h>
#include <lh/size.h>
#include <lh/void.h>

/**
 * @struct lh_math_slice
 * @brief Read-only closed slice of floats.
 */
struct lh_math_slice
{
    /** Members: ::lh_math_slice_fields. */
    lh_math_slice_fields(lh_float_t);
};
/** @copybrief lh_math_slice */
typedef struct lh_math_slice lh_math_slice_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set both bounds of @p self as they are: its @p first and @p last
 *        element, or both ::lh_null for an empty slice. What every init is made of.
 */
lh_void
lh_math_slice_set(lh_math_slice_t *self, const lh_float_t *first, const lh_float_t *last);

/**
 * @brief A slice over the @p count elements from @p data; empty when @p count is
 *        `0` or @p data is ::lh_null.
 */
lh_void
lh_math_slice_init(lh_math_slice_t *self, const lh_float_t *data, lh_usize_t count);

/**
 * @brief An empty slice.
 */
lh_void
lh_math_slice_init_empty(lh_math_slice_t *self);

/**
 * @brief A slice over the same elements as @p other.
 */
lh_void
lh_math_slice_init_by_other(lh_math_slice_t *self, const lh_math_slice_t *other);

/**
 * @brief The first element (`first`); ::lh_null when empty.
 */
const lh_float_t *
lh_math_slice_get_begin(const lh_math_slice_t *self);

/**
 * @brief The last element (`second`: the slice is closed); ::lh_null when empty.
 */
const lh_float_t *
lh_math_slice_get_end(const lh_math_slice_t *self);

/**
 * @brief True when @p self has no elements (both bounds ::lh_null).
 */
lh_bool_t
lh_math_slice_is_empty(const lh_math_slice_t *self);

/**
 * @brief True when @p self is empty, or both bounds are set and in order
 *        (::lh_interval_closed_is_valid).
 */
lh_bool_t
lh_math_slice_is_valid(const lh_math_slice_t *self);

/**
 * @brief How many elements: `0` when empty, else ::lh_interval_closed_get_size
 *        of the bounds.
 *
 * @fails ::lh_runtime_error_code_invalid_range @p self is not valid.
 */
lh_usize_t
lh_math_slice_get_size(const lh_math_slice_t *self);

/**
 * @brief True when @p index names an element of @p self: the element it points
 *        to lies in `[first, second]` (::lh_interval_closed_contains_value).
 */
lh_bool_t
lh_math_slice_is_valid_index(const lh_math_slice_t *self, lh_usize_t index);

/**
 * @brief Element @p index.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p index is not below the size.
 */
lh_float_t
lh_math_slice_get_at(const lh_math_slice_t *self, lh_usize_t index);

/**
 * @brief The @p count elements of @p self from @p offset, as a new slice over the
 *        same data (empty when @p count is `0`). What take and drop are made of.
 *
 * @fails ::lh_runtime_error_code_out_of_range `offset + count` passes the size.
 */
lh_void
lh_math_slice_get_sub(const lh_math_slice_t *self, lh_usize_t offset, lh_usize_t count,
                      lh_math_slice_t *out);

/**
 * @brief The first @p n elements.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p n is above the size.
 */
lh_void
lh_math_slice_take_first(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out);

/**
 * @brief The last @p n elements.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p n is above the size.
 */
lh_void
lh_math_slice_take_last(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out);

/**
 * @brief All but the first @p n elements.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p n is above the size.
 */
lh_void
lh_math_slice_drop_first(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out);

/**
 * @brief All but the last @p n elements.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p n is above the size.
 */
lh_void
lh_math_slice_drop_last(const lh_math_slice_t *self, lh_usize_t n, lh_math_slice_t *out);

/**
 * @brief True when @p a and @p b have the same size and equal elements (`==`: a
 *        NaN is never equal, and `0` equals `-0`).
 */
lh_bool_t
lh_math_slice_equals(const lh_math_slice_t *a, const lh_math_slice_t *b);

/**
 * @brief Sum of the elements, added first to last starting from the first (so the
 *        sum is the hand-written `x + y + ...`, sign of zero included); `0` when
 *        empty.
 */
lh_float_t
lh_math_slice_sum(const lh_math_slice_t *self);

/**
 * @brief `a[0] * b[0] + a[1] * b[1] + ...`, added first to last starting from the
 *        first product; `0` when both are empty.
 *
 * @fails ::lh_runtime_error_code_invalid_range the sizes differ.
 */
lh_float_t
lh_math_slice_dot(const lh_math_slice_t *a, const lh_math_slice_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SLICE_H */
