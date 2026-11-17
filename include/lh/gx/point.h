/**
 * @file point.h
 * @brief A position on the gx plane: ::lh_gx_point_t.
 *
 * The `x` and `y` of a place on the screen. It is a value and nothing more:
 * the caller owns it, passing it copies it, and no part of gx allocates one
 * or frees one. A point lives exactly as long as the caller names it.
 *
 * It is the math point of LH under the name of this part:
 * OFF → ::lh_math_point_t (::lh_math_scalar_t),
 * ON → ::lh_math_fpoint_t (::lh_math_fscalar_t).
 * Every function below is a thin layer over the chosen math point: it calls
 * that brick and adds nothing, so the contract of each one is the contract of
 * the math point it names.
 *
 * A point and a size (::lh_gx_size_t) make the rectangle of a thing on the
 * screen.
 */

#ifndef LH_GX_POINT_H
#define LH_GX_POINT_H

#include <lh/config.h>
#include <lh/core/math/point.h>
#include <lh/gx/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/math/fpoint.h>

/** @brief A point on the gx plane: ::lh_math_fpoint_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_fpoint_t lh_gx_point_t;

/**
 * @brief Name of the float math function @p name for ::lh_gx_point_t.
 *
 * The gx point is the float math point or the integer one; this names the
 * family the implementation calls.
 */
#    define LH_GX_POINT_FN(name) lh_math_fpoint_##name
#else

/** @brief A point on the gx plane: ::lh_math_point_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_point_t lh_gx_point_t;

/**
 * @brief Name of the integer math function @p name for ::lh_gx_point_t.
 */
#    define LH_GX_POINT_FN(name) lh_math_point_##name
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from explicit coordinates.
 *
 * @param self The point; must not be null.
 * @param x    The horizontal coordinate.
 * @param y    The vertical coordinate.
 */
lh_void
lh_gx_point_init(lh_gx_point_t *self, lh_gx_scalar_t x, lh_gx_scalar_t y);

/**
 * @brief Fill @p self with the origin `(0, 0)`.
 *
 * @param self The point; must not be null.
 */
lh_void
lh_gx_point_init_empty(lh_gx_point_t *self);

/**
 * @brief The horizontal coordinate of @p self.
 *
 * @param self The point; must not be null.
 * @return The coordinate.
 */
lh_gx_scalar_t
lh_gx_point_get_x(const lh_gx_point_t *self);

/**
 * @brief The vertical coordinate of @p self.
 *
 * @param self The point; must not be null.
 * @return The coordinate.
 */
lh_gx_scalar_t
lh_gx_point_get_y(const lh_gx_point_t *self);

/**
 * @brief Set the horizontal coordinate of @p self.
 *
 * @param self The point; must not be null.
 * @param x    The new coordinate.
 */
lh_void
lh_gx_point_set_x(lh_gx_point_t *self, lh_gx_scalar_t x);

/**
 * @brief Set the vertical coordinate of @p self.
 *
 * @param self The point; must not be null.
 * @param y    The new coordinate.
 */
lh_void
lh_gx_point_set_y(lh_gx_point_t *self, lh_gx_scalar_t y);

/**
 * @brief Narrow @p self to the integer point of ::lh_math_point_t.
 *
 * @param self The point; must not be null.
 * @param out  The integer point; must not be null.
 */
lh_void
lh_gx_point_to_point(const lh_gx_point_t *self, lh_math_point_t *out);

/**
 * @brief Widen the integer point @p self to the gx plane.
 *
 * @param self The integer point; must not be null.
 * @param out  The gx point; must not be null.
 */
lh_void
lh_gx_point_from_point(const lh_math_point_t *self, lh_gx_point_t *out);

/**
 * @brief Element-wise equality of @p a and @p b.
 *
 * @param a The first point; must not be null.
 * @param b The second point; must not be null.
 * @return True when both coordinates are equal.
 */
lh_bool_t
lh_gx_point_eq(const lh_gx_point_t *a, const lh_gx_point_t *b);

/**
 * @brief True if @p self and @p other have the same `x` and `y`.
 *
 * @param self  The first point; must not be null.
 * @param other The second point; must not be null.
 * @return True when the points are equal.
 */
lh_bool_t
lh_gx_point_equals(const lh_gx_point_t *self, const lh_gx_point_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: `x`, then `y`.
 *
 * @param self    The point; must not be null.
 * @param minimum The lower bound; must not be null.
 * @return True when @p self is at or after @p minimum.
 */
lh_bool_t
lh_gx_point_is_at_least(const lh_gx_point_t *self, const lh_gx_point_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * @param self  The point; must not be null.
 * @param other The other point; must not be null.
 * @return True when @p self comes first.
 */
lh_bool_t
lh_gx_point_is_less(const lh_gx_point_t *self, const lh_gx_point_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * @param self  The point; must not be null.
 * @param other The other point; must not be null.
 * @return True when @p self comes last.
 */
lh_bool_t
lh_gx_point_is_greater(const lh_gx_point_t *self, const lh_gx_point_t *other);

/**
 * @brief Element-wise minimum of @p a and @p b into @p out.
 *
 * @param a   The first point; must not be null.
 * @param b   The second point; must not be null.
 * @param out The result; must not be null.
 */
lh_void
lh_gx_point_min(const lh_gx_point_t *a, const lh_gx_point_t *b, lh_gx_point_t *out);

/**
 * @brief Element-wise maximum of @p a and @p b into @p out.
 *
 * @param a   The first point; must not be null.
 * @param b   The second point; must not be null.
 * @param out The result; must not be null.
 */
lh_void
lh_gx_point_max(const lh_gx_point_t *a, const lh_gx_point_t *b, lh_gx_point_t *out);

/**
 * @brief Test whether @p self lies in the half-open extent `[@p min, @p max)`.
 *
 * @param self The point; must not be null.
 * @param min  The near corner; must not be null.
 * @param max  The far corner; must not be null.
 * @return True when @p self is inside.
 */
lh_bool_t
lh_gx_point_in_extent(const lh_gx_point_t *self, const lh_gx_point_t *min,
                      const lh_gx_point_t *max);

/**
 * @brief Translate @p self by `(@p dx, @p dy)` into @p out.
 *
 * @param self The point; must not be null.
 * @param dx   The horizontal step.
 * @param dy   The vertical step.
 * @param out  The result; must not be null.
 */
lh_void
lh_gx_point_offset(const lh_gx_point_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                   lh_gx_point_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_POINT_H */
