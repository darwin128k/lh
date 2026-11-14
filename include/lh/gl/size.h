/**
 * @file size.h
 * @brief A size on the graphics plane: ::lh_gl_size_t, a ::lh_gl_point_t read as `width, height`.
 *
 * In LH terminology a size is a point: ::lh_gl_size_t is a direct alias of
 * ::lh_gl_point_t, the same two ::lh_math_scalar_t values, with `x` holding the
 * width and `y` the height. No second struct and no second field macro exist.
 * What the alias adds is a vocabulary: this header names the operations that
 * make sense when the pair is an extent ("how much") rather than a place
 * ("where").
 *
 * Fields are private. Nobody touches `x` or `y` of a size: read and mutate it
 * through the `lh_gl_size_get_*` / `lh_gl_size_set_*` methods, which are built on
 * the `lh_gl_point_*` accessors and are the only way in. If a method is missing,
 * add the method; do not reach into the struct.
 *
 * Negative extents are valid values; ::lh_gl_size_is_empty treats a zero or
 * negative extent as empty, so "nothing" needs no sentinel. A size owns nothing
 * and may be copied by value. Because it is the same type as a point, the
 * compiler does not tell the two apart; the method prefix does.
 */

#ifndef LH_GL_SIZE_H
#define LH_GL_SIZE_H

#include <lh/gl/point.h>

/**
 * @typedef lh_gl_size_t
 * @brief A size: a ::lh_gl_point_t whose `x` is the width and `y` is the height.
 */
typedef lh_gl_point_t lh_gl_size_t;

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Width of @p self.
 *
 * @param self Size to read; must not be null (asserted in debug builds).
 * @return The horizontal extent. Nothing is checked or changed.
 */
lh_math_scalar_t
lh_gl_size_get_width(const lh_gl_size_t *self);

/**
 * @brief Height of @p self.
 *
 * @param self Size to read; must not be null (asserted in debug builds).
 * @return The vertical extent. Nothing is checked or changed.
 */
lh_math_scalar_t
lh_gl_size_get_height(const lh_gl_size_t *self);

/**
 * @brief Set the width of @p self.
 *
 * @param self Size to change; must not be null (asserted in debug builds).
 * @param width New horizontal extent; any value is accepted.
 */
lh_void
lh_gl_size_set_width(lh_gl_size_t *self, lh_math_scalar_t width);

/**
 * @brief Set the height of @p self.
 *
 * @param self Size to change; must not be null (asserted in debug builds).
 * @param height New vertical extent; any value is accepted.
 */
lh_void
lh_gl_size_set_height(lh_gl_size_t *self, lh_math_scalar_t height);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief Fill @p self from explicit extents.
 *
 * Both values are overwritten; the memory may be uninitialized.
 *
 * @param self Size to fill; must not be null (asserted in debug builds).
 * @param width Horizontal extent.
 * @param height Vertical extent.
 */
lh_void
lh_gl_size_init(lh_gl_size_t *self, lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief Fill @p self with the empty size `(0, 0)`.
 *
 * @param self Size to fill; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_size_init_empty(lh_gl_size_t *self);

/**
 * @brief True if @p a and @p b have the same width and the same height.
 *
 * @param a First size; must not be null (asserted in debug builds).
 * @param b Second size; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when both extents match, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_size_eq(const lh_gl_size_t *a, const lh_gl_size_t *b);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: width first, then height when the widths are equal. Equal sizes count
 * as "at least".
 *
 * @param self Size to test; must not be null (asserted in debug builds).
 * @param minimum Lower bound; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self is equal to or later than @p minimum.
 */
lh_bool_t
lh_gl_size_is_at_least(const lh_gl_size_t *self, const lh_gl_size_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same order as ::lh_gl_size_is_at_least.
 *
 * @param self Size to test; must not be null (asserted in debug builds).
 * @param other Size to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes before @p other.
 */
lh_bool_t
lh_gl_size_is_less(const lh_gl_size_t *self, const lh_gl_size_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same order as ::lh_gl_size_is_at_least.
 *
 * @param self Size to test; must not be null (asserted in debug builds).
 * @param other Size to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes after @p other.
 */
lh_bool_t
lh_gl_size_is_greater(const lh_gl_size_t *self, const lh_gl_size_t *other);

/**
 * @brief Component-wise minimum of @p a and @p b into @p out.
 *
 * @p out may be the same object as @p a or @p b.
 *
 * @param a First size; must not be null (asserted in debug builds).
 * @param b Second size; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_size_min(const lh_gl_size_t *a, const lh_gl_size_t *b, lh_gl_size_t *out);

/**
 * @brief Component-wise maximum of @p a and @p b into @p out.
 *
 * @p out may be the same object as @p a or @p b.
 *
 * @param a First size; must not be null (asserted in debug builds).
 * @param b Second size; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_size_max(const lh_gl_size_t *a, const lh_gl_size_t *b, lh_gl_size_t *out);

/**
 * @brief Size spanning from @p min (inclusive) to @p max (exclusive).
 *
 * Computes `max - min` on each axis. @p out may be the same object as
 * @p min or @p max. Nothing is checked: a @p max before @p min gives a
 * negative extent.
 *
 * @param min Near corner; must not be null (asserted in debug builds).
 * @param max Far corner; must not be null (asserted in debug builds).
 * @param out Receives the extent; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_size_from_extent(const lh_gl_point_t *min, const lh_gl_point_t *max, lh_gl_size_t *out);

/**
 * @brief True if @p self is empty: its width or its height is zero or negative.
 *
 * @param self Size to test; must not be null (asserted in debug builds).
 * @return ::lh_bool_true for an empty size, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_size_is_empty(const lh_gl_size_t *self);

/**
 * @brief The shorter side of @p self: `min(width, height)`.
 *
 * @param self Size to read; must not be null (asserted in debug builds).
 * @return The smaller of the two extents.
 */
lh_math_scalar_t
lh_gl_size_min_side(const lh_gl_size_t *self);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side into @p out.
 *
 * Positive values shrink, negative values grow: width `-= 2 * dx`, height
 * `-= 2 * dy`. @p out may be the same object as @p self. The result is not
 * clamped and may be negative.
 *
 * @param self Size to inset; must not be null (asserted in debug builds).
 * @param dx Inset on the left and on the right.
 * @param dy Inset on the top and on the bottom.
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_size_inset(const lh_gl_size_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                 lh_gl_size_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SIZE_H */