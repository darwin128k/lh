/**
 * @file view.h
 * @brief Read-only strided view of ::lh_float_t values: ::lh_math_view_t.
 *
 * A view reads every `stride`-th element of a ::lh_math_slice_t from an offset:
 * the components of a vector (stride 1), a row of a row-major matrix (stride 1)
 * or a column of it (stride 4), without copying a value. It keeps the slice of
 * the elements it spans, so every read goes through ::lh_math_slice_get_at and
 * its bounds.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_VIEW_H
#define LH_MATH_VIEW_H

#include <lh/math/slice.h>
#include <lh/math/view/fields.h>

/**
 * @struct lh_math_view
 * @brief Read-only strided view of floats.
 */
struct lh_math_view
{
    /** Members: ::lh_math_view_fields. */
    lh_math_view_fields(lh_math_slice_t, lh_usize_t);
};
/** @copybrief lh_math_view */
typedef struct lh_math_view lh_math_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The slice the view walks: its first element is element 0 of the view. */
const lh_math_slice_t *
lh_math_view_get_slice(const lh_math_view_t *self);

/** @brief The slice the view walks, to fill in place (::lh_math_view_init). */
lh_math_slice_t *
lh_math_view_get_slice_mut(lh_math_view_t *self);

/** @brief Distance in elements between two neighbours of the view. */
lh_usize_t
lh_math_view_get_stride(const lh_math_view_t *self);

/** @brief Store the distance between neighbours. Level-1 setter: no validation. */
lh_void
lh_math_view_set_stride(lh_math_view_t *self, lh_usize_t stride);

/** @brief Store the number of elements (::lh_math_view_get_size reads it). Level-1 setter. */
lh_void
lh_math_view_set_count(lh_math_view_t *self, lh_usize_t count);

/**
 * @brief How many elements of a slice @p count elements @p stride apart span:
 *        `(count - 1) * stride + 1`, or `0` for no elements.
 */
lh_usize_t
lh_math_view_get_span(lh_usize_t stride, lh_usize_t count);

/**
 * @brief How many elements the view has.
 */
lh_usize_t
lh_math_view_get_size(const lh_math_view_t *self);

/**
 * @brief A view of @p count elements of @p base, from element @p offset, each
 *        @p stride after the one before it.
 *
 * @fails ::lh_runtime_error_code_invalid_range @p stride is `0`.
 * @fails ::lh_runtime_error_code_out_of_range the last element is past @p base.
 */
lh_void
lh_math_view_init(lh_math_view_t *self, const lh_math_slice_t *base, lh_usize_t offset,
                  lh_usize_t stride, lh_usize_t count);

/**
 * @brief True when the view has no elements.
 */
lh_bool_t
lh_math_view_is_empty(const lh_math_view_t *self);

/**
 * @brief Element @p index of the view: element `index * stride` of its slice.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p index is not below the size.
 */
lh_float_t
lh_math_view_get_at(const lh_math_view_t *self, lh_usize_t index);

/**
 * @brief A view of every element of @p base (stride `1`).
 */
lh_void
lh_math_view_init_by_slice(lh_math_view_t *self, const lh_math_slice_t *base);

/**
 * @brief `a[0] * b[0] + a[1] * b[1] + ...`, added first to last starting from the
 *        first product (the hand-written sum); `0` when both are empty.
 *
 * @fails ::lh_runtime_error_code_invalid_range the sizes differ.
 */
lh_float_t
lh_math_view_dot(const lh_math_view_t *a, const lh_math_view_t *b);

/**
 * @brief Squared length: ::lh_math_view_dot of the view with itself.
 */
lh_float_t
lh_math_view_length_sq(const lh_math_view_t *self);

/**
 * @brief Length: ::lh_math_float_sqrt of ::lh_math_view_length_sq.
 */
lh_float_t
lh_math_view_length(const lh_math_view_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_VIEW_H */
