/**
 * @file rect.h
 * @brief A rectangle on the gx plane: ::lh_gx_rect_t.
 *
 * The place a thing occupies on the screen: a top-left corner
 * (::lh_gx_point_t) and an extent (::lh_gx_size_t), half-open. It is a value
 * and nothing more: the caller owns it, passing it copies it, and no part of
 * gx allocates one or frees one. A rectangle lives exactly as long as the
 * caller names it.
 *
 * It is the math rectangle of LH under the name of this part:
 * OFF → ::lh_math_rect_t (::lh_math_scalar_t),
 * ON → ::lh_math_frect_t (::lh_math_fscalar_t).
 * Every function below is a thin layer over the chosen math rectangle: it
 * calls that brick and adds nothing, so the contract of each one is the
 * contract of the math rectangle it names.
 *
 * The rectangle of an entity is where that entity sits on the screen; the
 * entity's own place inside its parent is what the layout pass computes.
 */

#ifndef LH_GX_RECT_H
#define LH_GX_RECT_H

#include <lh/config.h>
#include <lh/core/math/rect.h>
#include <lh/gx/size.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/math/frect.h>

/** @brief A rectangle on the gx plane: ::lh_math_frect_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_frect_t lh_gx_rect_t;

/**
 * @brief Name of the float math function @p name for ::lh_gx_rect_t.
 *
 * The gx rectangle is the float math rectangle or the integer one; this names
 * the family the implementation calls.
 */
#    define LH_GX_RECT_FN(name) lh_math_frect_##name
#else

/** @brief A rectangle on the gx plane: ::lh_math_rect_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_rect_t lh_gx_rect_t;

/**
 * @brief Name of the integer math function @p name for ::lh_gx_rect_t.
 */
#    define LH_GX_RECT_FN(name) lh_math_rect_##name
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Build a rectangle from exclusive extent corners.
 *
 * @p max is exclusive (origin + size). An empty size yields an empty
 * rectangle.
 *
 * @param min The near corner; must not be null.
 * @param max The far corner; must not be null.
 * @param out The rectangle; must not be null.
 */
lh_void
lh_gx_rect_from_extent(const lh_gx_point_t *min, const lh_gx_point_t *max, lh_gx_rect_t *out);

/**
 * @brief Fill @p self from origin coordinates and extents.
 *
 * @param self   The rectangle; must not be null.
 * @param x      The horizontal coordinate of the origin.
 * @param y      The vertical coordinate of the origin.
 * @param width  The width.
 * @param height The height.
 */
lh_void
lh_gx_rect_init(lh_gx_rect_t *self, lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t width,
                lh_gx_scalar_t height);

/**
 * @brief Fill @p self from @p origin and @p size.
 *
 * @param self  The rectangle; must not be null.
 * @param origin The top-left corner.
 * @param size  The extent.
 */
lh_void
lh_gx_rect_init_origin_size(lh_gx_rect_t *self, lh_gx_point_t origin, lh_gx_size_t size);

/**
 * @brief Fill @p self with the empty rectangle `(0, 0)` of size `(0, 0)`.
 *
 * @param self The rectangle; must not be null.
 */
lh_void
lh_gx_rect_init_empty(lh_gx_rect_t *self);

/**
 * @brief Pointer to the top-left corner of @p self, to change it in place.
 *
 * @param self The rectangle; must not be null.
 * @return The corner itself, never null.
 */
lh_gx_point_t *
lh_gx_rect_get_origin_mut(lh_gx_rect_t *self);

/**
 * @brief Pointer to the top-left corner of @p self.
 *
 * @param self The rectangle; must not be null.
 * @return The corner, never null.
 */
const lh_gx_point_t *
lh_gx_rect_get_origin(const lh_gx_rect_t *self);

/**
 * @brief Pointer to the extent of @p self, to change it in place.
 *
 * @param self The rectangle; must not be null.
 * @return The extent itself, never null.
 */
lh_gx_size_t *
lh_gx_rect_get_size_mut(lh_gx_rect_t *self);

/**
 * @brief Pointer to the extent of @p self.
 *
 * @param self The rectangle; must not be null.
 * @return The extent, never null.
 */
const lh_gx_size_t *
lh_gx_rect_get_size(const lh_gx_rect_t *self);

/**
 * @brief Replace the origin of @p self with @p origin.
 *
 * @param self   The rectangle; must not be null.
 * @param origin The new corner.
 */
lh_void
lh_gx_rect_set_origin(lh_gx_rect_t *self, lh_gx_point_t origin);

/**
 * @brief Replace the extent of @p self with @p size.
 *
 * @param self The rectangle; must not be null.
 * @param size The new extent.
 */
lh_void
lh_gx_rect_set_size(lh_gx_rect_t *self, lh_gx_size_t size);

/**
 * @brief The exclusive far corner of @p self: the origin offset by the size.
 *
 * @param self The rectangle; must not be null.
 * @param out  The corner; must not be null.
 */
lh_void
lh_gx_rect_far(const lh_gx_rect_t *self, lh_gx_point_t *out);

/**
 * @brief The smaller of the two origins, element-wise.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The corner; must not be null.
 */
lh_void
lh_gx_rect_origin_min(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out);

/**
 * @brief The larger of the two origins, element-wise.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The corner; must not be null.
 */
lh_void
lh_gx_rect_origin_max(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out);

/**
 * @brief The smaller of the two far corners, element-wise.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The corner; must not be null.
 */
lh_void
lh_gx_rect_far_min(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out);

/**
 * @brief The larger of the two far corners, element-wise.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The corner; must not be null.
 */
lh_void
lh_gx_rect_far_max(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out);

/**
 * @brief Narrow @p self to the integer rectangle of ::lh_math_rect_t.
 *
 * @param self The rectangle; must not be null.
 * @param out  The integer rectangle; must not be null.
 */
lh_void
lh_gx_rect_to_rect(const lh_gx_rect_t *self, lh_math_rect_t *out);

/**
 * @brief Widen the integer rectangle @p self to the gx plane.
 *
 * @param self The integer rectangle; must not be null.
 * @param out  The gx rectangle; must not be null.
 */
lh_void
lh_gx_rect_from_rect(const lh_math_rect_t *self, lh_gx_rect_t *out);

/**
 * @brief Test whether @p self covers nothing.
 *
 * @param self The rectangle; must not be null.
 * @return True when the rectangle is empty.
 */
lh_bool_t
lh_gx_rect_is_empty(const lh_gx_rect_t *self);

/**
 * @brief Test whether @p self covers @p point.
 *
 * The top and left edges belong to the rectangle, the bottom and right edges
 * do not: the rectangle is half-open.
 *
 * @param self  The rectangle; must not be null.
 * @param point The point.
 * @return True when the point is inside.
 */
lh_bool_t
lh_gx_rect_contains_point(const lh_gx_rect_t *self, lh_gx_point_t point);

/**
 * @brief Test whether two rectangles share any non-empty area.
 *
 * @param a The first rectangle; must not be null.
 * @param b The second rectangle; must not be null.
 * @return True when they overlap.
 */
lh_bool_t
lh_gx_rect_intersects(const lh_gx_rect_t *a, const lh_gx_rect_t *b);

/**
 * @brief Element-wise equality of @p a and @p b.
 *
 * @param a The first rectangle; must not be null.
 * @param b The second rectangle; must not be null.
 * @return True when both corners and extents are equal.
 */
lh_bool_t
lh_gx_rect_eq(const lh_gx_rect_t *a, const lh_gx_rect_t *b);

/**
 * @brief True if @p self and @p other have the same origin and size.
 *
 * @param self  The first rectangle; must not be null.
 * @param other The second rectangle; must not be null.
 * @return True when the rectangles are equal.
 */
lh_bool_t
lh_gx_rect_equals(const lh_gx_rect_t *self, const lh_gx_rect_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * @param self    The rectangle; must not be null.
 * @param minimum The lower bound; must not be null.
 * @return True when @p self is at or after @p minimum.
 */
lh_bool_t
lh_gx_rect_is_at_least(const lh_gx_rect_t *self, const lh_gx_rect_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * @param self  The rectangle; must not be null.
 * @param other The other rectangle; must not be null.
 * @return True when @p self comes first.
 */
lh_bool_t
lh_gx_rect_is_less(const lh_gx_rect_t *self, const lh_gx_rect_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * @param self  The rectangle; must not be null.
 * @param other The other rectangle; must not be null.
 * @return True when @p self comes last.
 */
lh_bool_t
lh_gx_rect_is_greater(const lh_gx_rect_t *self, const lh_gx_rect_t *other);

/**
 * @brief The overlap of @p a and @p b into @p out.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The overlap; must not be null.
 */
lh_void
lh_gx_rect_intersection(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_rect_t *out);

/**
 * @brief The smallest rectangle containing both, into @p out.
 *
 * @param a   The first rectangle; must not be null.
 * @param b   The second rectangle; must not be null.
 * @param out The result; must not be null.
 */
lh_void
lh_gx_rect_union(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_rect_t *out);

/**
 * @brief Translate @p self by `(@p dx, @p dy)` into @p out.
 *
 * @param self The rectangle; must not be null.
 * @param dx   The horizontal step.
 * @param dy   The vertical step.
 * @param out  The result; must not be null.
 */
lh_void
lh_gx_rect_offset(const lh_gx_rect_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                  lh_gx_rect_t *out);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side into @p out.
 *
 * @param self The rectangle; must not be null.
 * @param dx   The horizontal step.
 * @param dy   The vertical step.
 * @param out  The result; must not be null.
 */
lh_void
lh_gx_rect_inset(const lh_gx_rect_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                 lh_gx_rect_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_RECT_H */
