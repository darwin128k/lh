/**
 * @file ops.h
 * @brief Component-wise size formulas shared by ::lh_math_size_t and
 *        ::lh_math_fsize_t.
 *
 * One body per formula: `lh_math_size_*` and `lh_math_fsize_*` expand these
 * with their own accessor prefix instead of repeating the expression.
 */

#ifndef LH_MATH_SIZE_OPS_H
#define LH_MATH_SIZE_OPS_H

#include <lh/math/ops.h>

/**
 * @def lh_math_size_pairwise(prefix, op, a, b, out)
 * @brief `prefix_init(out, op(a.w, b.w), op(a.h, b.h))`.
 *
 * @param prefix  Size family, `lh_math_size` or `lh_math_fsize`.
 * @param op      Two-argument macro or function applied per component
 *                (::lh_math_min, ::lh_math_max).
 */
#define lh_math_size_pairwise(prefix, op, a, b, out)                                                \
    prefix##_init((out), op(prefix##_get_width(a), prefix##_get_width(b)),                          \
                  op(prefix##_get_height(a), prefix##_get_height(b)))

/**
 * @def lh_math_size_min_side_of(prefix, self)
 * @brief `min(width, height)` of @p self.
 */
#define lh_math_size_min_side_of(prefix, self) lh_math_min(prefix##_get_width(self), prefix##_get_height(self))

#endif /* LH_MATH_SIZE_OPS_H */
