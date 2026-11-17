/**
 * @file size.h
 * @brief An extent on the gx plane: ::lh_gx_size_t.
 *
 * The `width` and `height` of a thing on the screen. It is a value and
 * nothing more: the caller owns it, passing it copies it, and no part of gx
 * allocates one or frees one. A size lives exactly as long as the caller
 * names it.
 *
 * It is the math size of LH under the name of this part:
 * OFF → ::lh_math_size_t (::lh_math_scalar_t),
 * ON → ::lh_math_fsize_t (::lh_math_fscalar_t).
 * Every function below is a thin layer over the chosen math size: it calls
 * that brick and adds nothing, so the contract of each one is the contract of
 * the math size it names.
 *
 * A size and a point (::lh_gx_point_t) make the rectangle of a thing on the
 * screen.
 */

#ifndef LH_GX_SIZE_H
#define LH_GX_SIZE_H

#include <lh/config.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/math/size.h>
#include <lh/core/void.h>
#include <lh/gx/point.h>
#include <lh/gx/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/math/fsize.h>

/** @brief An extent on the gx plane: ::lh_math_fsize_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_fsize_t lh_gx_size_t;

/**
 * @brief Name of the float math function @p name for ::lh_gx_size_t.
 *
 * The gx size is the float math size or the integer one; this names the
 * family the implementation calls.
 */
#    define LH_GX_SIZE_FN(name) lh_math_fsize_##name
#else

/** @brief An extent on the gx plane: ::lh_math_size_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_size_t lh_gx_size_t;

/**
 * @brief Name of the integer math function @p name for ::lh_gx_size_t.
 */
#    define LH_GX_SIZE_FN(name) lh_math_size_##name
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from explicit extents.
 *
 * @param self   The size; must not be null.
 * @param width  The width.
 * @param height The height.
 */
lh_void
lh_gx_size_init(lh_gx_size_t *self, lh_gx_scalar_t width, lh_gx_scalar_t height);

/**
 * @brief Fill @p self with the empty size `(0, 0)`.
 *
 * @param self The size; must not be null.
 */
lh_void
lh_gx_size_init_empty(lh_gx_size_t *self);

/**
 * @brief Size spanning from @p min (inclusive) to @p max (exclusive).
 *
 * @param min The near corner; must not be null.
 * @param max The far corner; must not be null.
 * @param out The size; must not be null.
 */
lh_void
lh_gx_size_from_extent(const lh_gx_point_t *min, const lh_gx_point_t *max, lh_gx_size_t *out);

/**
 * @brief The width of @p self.
 *
 * @param self The size; must not be null.
 * @return The width.
 */
lh_gx_scalar_t
lh_gx_size_get_width(const lh_gx_size_t *self);

/**
 * @brief The height of @p self.
 *
 * @param self The size; must not be null.
 * @return The height.
 */
lh_gx_scalar_t
lh_gx_size_get_height(const lh_gx_size_t *self);

/**
 * @brief Set the width of @p self.
 *
 * @param self  The size; must not be null.
 * @param width The new width.
 */
lh_void
lh_gx_size_set_width(lh_gx_size_t *self, lh_gx_scalar_t width);

/**
 * @brief Set the height of @p self.
 *
 * @param self   The size; must not be null.
 * @param height The new height.
 */
lh_void
lh_gx_size_set_height(lh_gx_size_t *self, lh_gx_scalar_t height);

/**
 * @brief Narrow @p self to the integer size of ::lh_math_size_t.
 *
 * @param self The size; must not be null.
 * @param out  The integer size; must not be null.
 */
lh_void
lh_gx_size_to_size(const lh_gx_size_t *self, lh_math_size_t *out);

/**
 * @brief Widen the integer size @p self to the gx plane.
 *
 * @param self The integer size; must not be null.
 * @param out  The gx size; must not be null.
 */
lh_void
lh_gx_size_from_size(const lh_math_size_t *self, lh_gx_size_t *out);

/**
 * @brief Element-wise equality of @p a and @p b.
 *
 * @param a The first size; must not be null.
 * @param b The second size; must not be null.
 * @return True when both extents are equal.
 */
lh_bool_t
lh_gx_size_eq(const lh_gx_size_t *a, const lh_gx_size_t *b);

/**
 * @brief True if @p self and @p other have the same `width` and `height`.
 *
 * @param self  The first size; must not be null.
 * @param other The second size; must not be null.
 * @return True when the sizes are equal.
 */
lh_bool_t
lh_gx_size_equals(const lh_gx_size_t *self, const lh_gx_size_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: `width`, then `height`.
 *
 * @param self    The size; must not be null.
 * @param minimum The lower bound; must not be null.
 * @return True when @p self is at or after @p minimum.
 */
lh_bool_t
lh_gx_size_is_at_least(const lh_gx_size_t *self, const lh_gx_size_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * @param self  The size; must not be null.
 * @param other The other size; must not be null.
 * @return True when @p self comes first.
 */
lh_bool_t
lh_gx_size_is_less(const lh_gx_size_t *self, const lh_gx_size_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * @param self  The size; must not be null.
 * @param other The other size; must not be null.
 * @return True when @p self comes last.
 */
lh_bool_t
lh_gx_size_is_greater(const lh_gx_size_t *self, const lh_gx_size_t *other);

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 *
 * @param self The size; must not be null.
 * @return True when the size covers nothing.
 */
lh_bool_t
lh_gx_size_is_empty(const lh_gx_size_t *self);

/**
 * @brief Component-wise minimum of @p a and @p b into @p out.
 *
 * @param a   The first size; must not be null.
 * @param b   The second size; must not be null.
 * @param out The result; must not be null.
 */
lh_void
lh_gx_size_min(const lh_gx_size_t *a, const lh_gx_size_t *b, lh_gx_size_t *out);

/**
 * @brief Component-wise maximum of @p a and @p b into @p out.
 *
 * @param a   The first size; must not be null.
 * @param b   The second size; must not be null.
 * @param out The result; must not be null.
 */
lh_void
lh_gx_size_max(const lh_gx_size_t *a, const lh_gx_size_t *b, lh_gx_size_t *out);

/**
 * @brief The shorter side of @p self: `min(width, height)`.
 *
 * @param self The size; must not be null.
 * @return The shorter side.
 */
lh_gx_scalar_t
lh_gx_size_min_side(const lh_gx_size_t *self);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side into @p out.
 *
 * Positive shrinks, negative grows: width `-= 2 * dx`, height `-= 2 * dy`.
 *
 * @param self The size; must not be null.
 * @param dx   The horizontal step.
 * @param dy   The vertical step.
 * @param out  The result; must not be null.
 */
lh_void
lh_gx_size_inset(const lh_gx_size_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                 lh_gx_size_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_SIZE_H */
