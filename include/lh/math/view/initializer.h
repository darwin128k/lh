/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_view_t.
 */

#ifndef LH_MATH_VIEW_INITIALIZER_H
#define LH_MATH_VIEW_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_view_initializer(slice, stride, count)
 * @brief Produces a brace-enclosed initializer for ::lh_math_view_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_view_fields.
 *
 * Nothing is checked: @p stride must not be `0` and the elements must lie
 * inside @p slice. ::lh_math_view_init checks both.
 *
 * @param slice  Initializer of the ::lh_math_slice_t the view spans
 *               (::lh_math_slice_initializer).
 * @param stride Elements from one read to the next (not `0`).
 * @param count  How many elements the view has.
 *
 * Example usage:
 * @code{.c}
 * static const lh_float_t data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
 * const lh_math_view_t view = lh_math_view_initializer(
 *     lh_math_slice_initializer(&data[0], &data[3]),  // slice
 *     2,                                              // stride
 *     2);                                             // count
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_view_t
 * @see lh_math_view_init
 */
#define lh_math_view_initializer(slice, stride, count) lh_initializer(slice, stride, count)

#endif /* LH_MATH_VIEW_INITIALIZER_H */
