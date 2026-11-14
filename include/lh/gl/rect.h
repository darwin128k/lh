/**
 * @file rect.h
 * @brief An axis-aligned rectangle on the graphics plane: ::lh_gl_rect_t, an
 *        `origin` (top-left) plus a `size`, half-open.
 *
 * A rectangle is two bricks side by side: a ::lh_gl_point_t `origin` and a
 * ::lh_gl_size_t `size`. `width = 10` covers the columns `[origin.x, origin.x + 10)`,
 * that is `origin.x` through `origin.x + 9`; the far corner (::lh_gl_rect_get_end)
 * is exclusive. The members come from ::lh_math_rect_fields, the member macro
 * of the numeric rectangle in `lh/core/math/rect.h`, so no second field macro
 * exists.
 *
 * Fields are private. Nobody names `origin` or `size` outside the four level-1
 * accessors (::lh_gl_rect_get_origin, ::lh_gl_rect_get_origin_mut,
 * ::lh_gl_rect_get_size, ::lh_gl_rect_get_size_mut); everything else, here and
 * in callers, goes through them and through the `lh_gl_point_*` and
 * `lh_gl_size_*` methods of the parts. The struct is defined here only so it can
 * be embedded by value.
 *
 * A rectangle is empty when its size is empty (::lh_gl_size_is_empty), so
 * ::lh_gl_rect_is_empty, ::lh_gl_rect_intersection and ::lh_gl_rect_union share
 * one definition and an "empty" result is the same value whichever operation
 * produced it. A rectangle owns nothing and may be copied by value.
 */

#ifndef LH_GL_RECT_H
#define LH_GL_RECT_H

#include <lh/core/math/rect/fields.h>
#include <lh/gl/size.h>

/**
 * @struct lh_gl_rect
 * @brief An axis-aligned rectangle: `origin` plus `size`, half-open.
 */
struct lh_gl_rect
{
    /** Members: ::lh_math_rect_fields. */
    lh_math_rect_fields(lh_gl_point_t, lh_gl_size_t);
};
/** @copybrief lh_gl_rect */
typedef struct lh_gl_rect lh_gl_rect_t;

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mutable pointer to the top-left corner of @p self.
 *
 * @param self Rectangle to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_gl_point_t *
lh_gl_rect_get_origin_mut(lh_gl_rect_t *self);

/**
 * @brief Const pointer to the top-left corner of @p self.
 *
 * @param self Rectangle to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_gl_point_t *
lh_gl_rect_get_origin(const lh_gl_rect_t *self);

/**
 * @brief Mutable pointer to the size of @p self.
 *
 * @param self Rectangle to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_gl_size_t *
lh_gl_rect_get_size_mut(lh_gl_rect_t *self);

/**
 * @brief Const pointer to the size of @p self.
 *
 * @param self Rectangle to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_gl_size_t *
lh_gl_rect_get_size(const lh_gl_rect_t *self);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief Replace the origin of @p self with @p origin.
 *
 * @param self Rectangle to change; must not be null (asserted in debug builds).
 * @param origin New top-left corner, copied.
 */
lh_void
lh_gl_rect_set_origin(lh_gl_rect_t *self, lh_gl_point_t origin);

/**
 * @brief Replace the size of @p self with @p size.
 *
 * @param self Rectangle to change; must not be null (asserted in debug builds).
 * @param size New extent, copied.
 */
lh_void
lh_gl_rect_set_size(lh_gl_rect_t *self, lh_gl_size_t size);

/**
 * @brief Exclusive far corner of @p self: the origin offset by the size
 *        (`x + width`, `y + height`).
 *
 * @param self Rectangle to read; must not be null (asserted in debug builds).
 * @param out Receives the corner; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_get_end(const lh_gl_rect_t *self, lh_gl_point_t *out);

/**
 * @brief Component-wise minimum of the origins of @p a and @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the point; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_get_origin_min(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out);

/**
 * @brief Component-wise maximum of the origins of @p a and @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the point; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_get_origin_max(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out);

/**
 * @brief True if @p self is empty: ::lh_gl_size_is_empty on its size.
 *
 * @param self Rectangle to test; must not be null (asserted in debug builds).
 * @return ::lh_bool_true for an empty rectangle, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_rect_is_empty(const lh_gl_rect_t *self);

/**
 * @brief True if @p a and @p b have the same origin and the same size.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when both parts match, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_rect_eq(const lh_gl_rect_t *a, const lh_gl_rect_t *b);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: origin (::lh_gl_point_is_at_least), then size (::lh_gl_size_is_at_least)
 * when the origins are equal. Equal rectangles count as "at least".
 *
 * @param self Rectangle to test; must not be null (asserted in debug builds).
 * @param minimum Lower bound; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self is equal to or later than @p minimum.
 */
lh_bool_t
lh_gl_rect_is_at_least(const lh_gl_rect_t *self, const lh_gl_rect_t *minimum);

/**
 * @brief Fill @p self from @p origin and @p size.
 *
 * Both parts are overwritten; the memory may be uninitialized.
 *
 * @param self Rectangle to fill; must not be null (asserted in debug builds).
 * @param origin Top-left corner, copied.
 * @param size Extent, copied.
 */
lh_void
lh_gl_rect_init_origin_size(lh_gl_rect_t *self, lh_gl_point_t origin, lh_gl_size_t size);

/**
 * @brief Component-wise minimum of the exclusive far corners of @p a and @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the point; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_get_end_min(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out);

/**
 * @brief Component-wise maximum of the exclusive far corners of @p a and @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the point; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_get_end_max(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out);

/**
 * @brief True if @p self covers @p point: inclusive at top and left, exclusive
 *        at bottom and right.
 *
 * An empty rectangle covers nothing.
 *
 * @param self Rectangle to test; must not be null (asserted in debug builds).
 * @param point Point to look for, copied.
 * @return ::lh_bool_true when @p point lies inside, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_rect_contains_point(const lh_gl_rect_t *self, lh_gl_point_t point);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same order as ::lh_gl_rect_is_at_least.
 *
 * @param self Rectangle to test; must not be null (asserted in debug builds).
 * @param other Rectangle to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes before @p other.
 */
lh_bool_t
lh_gl_rect_is_less(const lh_gl_rect_t *self, const lh_gl_rect_t *other);

/**
 * @brief Fill @p self from origin coordinates and extents.
 *
 * @param self Rectangle to fill; must not be null (asserted in debug builds).
 * @param x Horizontal coordinate of the top-left corner.
 * @param y Vertical coordinate of the top-left corner.
 * @param width Horizontal extent.
 * @param height Vertical extent.
 */
lh_void
lh_gl_rect_init(lh_gl_rect_t *self, lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width,
                lh_math_scalar_t height);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same order as ::lh_gl_rect_is_at_least.
 *
 * @param self Rectangle to test; must not be null (asserted in debug builds).
 * @param other Rectangle to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes after @p other.
 */
lh_bool_t
lh_gl_rect_is_greater(const lh_gl_rect_t *self, const lh_gl_rect_t *other);

/**
 * @brief Translate @p self by `(@p dx, @p dy)` into @p out.
 *
 * The size is unchanged. @p out may be the same object as @p self.
 *
 * @param self Rectangle to move; must not be null (asserted in debug builds).
 * @param dx Horizontal shift.
 * @param dy Vertical shift.
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_offset(const lh_gl_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                  lh_gl_rect_t *out);

/**
 * @brief Fill @p self with the empty rectangle: origin `(0, 0)`, size `(0, 0)`.
 *
 * @param self Rectangle to fill; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_init_empty(lh_gl_rect_t *self);

/**
 * @brief Build a rectangle from exclusive extent corners.
 *
 * @p max is exclusive (`origin + size`). When `max - min` is empty
 * (::lh_gl_size_is_empty) the result is the empty rectangle
 * ::lh_gl_rect_init_empty, not a rectangle at @p min.
 *
 * @param min Top-left corner, inclusive; must not be null (asserted in debug builds).
 * @param max Far corner, exclusive; must not be null (asserted in debug builds).
 * @param out Receives the rectangle; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_from_extent(const lh_gl_point_t *min, const lh_gl_point_t *max, lh_gl_rect_t *out);

/**
 * @brief Build a rectangle from min/max scalars (left, top, right, bottom).
 *
 * `right` and `bottom` are exclusive, as in ::lh_gl_rect_from_extent, which
 * this calls.
 *
 * @param x_min Left, inclusive.
 * @param y_min Top, inclusive.
 * @param x_max Right, exclusive.
 * @param y_max Bottom, exclusive.
 * @param out Receives the rectangle; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_from_min_max(lh_math_scalar_t x_min, lh_math_scalar_t y_min, lh_math_scalar_t x_max,
                        lh_math_scalar_t y_max, lh_gl_rect_t *out);

/**
 * @brief Intersection of @p a and @p b.
 *
 * Writes the empty rectangle (::lh_gl_rect_init_empty) when either input is
 * empty or they do not overlap. @p out may be the same object as @p a or @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_intersection(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_rect_t *out);

/**
 * @brief Union of @p a and @p b: the smallest rectangle containing both.
 *
 * An empty @p a gives a copy of @p b and an empty @p b gives a copy of @p a.
 * @p out may be the same object as @p a or @p b.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_union(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_rect_t *out);

/**
 * @brief True if @p a and @p b share any non-empty area.
 *
 * @param a First rectangle; must not be null (asserted in debug builds).
 * @param b Second rectangle; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when ::lh_gl_rect_intersection is not empty.
 */
lh_bool_t
lh_gl_rect_intersects(const lh_gl_rect_t *a, const lh_gl_rect_t *b);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side into @p out.
 *
 * Positive values shrink, negative values grow: the origin moves by
 * `(dx, dy)` and the size follows ::lh_gl_size_inset. The result is not
 * clamped and may be empty. @p out may be the same object as @p self.
 *
 * @param self Rectangle to inset; must not be null (asserted in debug builds).
 * @param dx Inset on the left and on the right.
 * @param dy Inset on the top and on the bottom.
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_rect_inset(const lh_gl_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                 lh_gl_rect_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_RECT_H */