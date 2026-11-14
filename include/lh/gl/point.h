/**
 * @file point.h
 * @brief A point on the graphics plane: ::lh_gl_point_t with integer `x, y`.
 *
 * The first brick of the `lh/gl` part. A point is a plain value of two
 * ::lh_math_scalar_t coordinates: `x` is horizontal, `y` is vertical, the
 * origin is the top-left corner and both grow right and down, like every OS
 * window coordinate system. The coordinates are integers; there is no
 * floating-point variant here.
 *
 * The members come from ::lh_math_point_fields, the member macro of the
 * numeric point in `lh/core/math/point.h`, so the layout of the two points is
 * the same and no second field macro exists. A point owns nothing, needs no
 * release and may be copied by value.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_gl_point_get_*` / `lh_gl_point_set_*` accessors, the only code that names
 * a field. The struct is defined here only so it can be embedded by value.
 * Everything above the accessors (init, comparison) is built from them.
 */

#ifndef LH_GL_POINT_H
#define LH_GL_POINT_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/math/point/fields.h>
#include <lh/core/math/scalar.h>
#include <lh/core/void.h>

/**
 * @struct lh_gl_point
 * @brief A point on the graphics plane: `x` is horizontal, `y` is vertical.
 */
struct lh_gl_point
{
    /** Members: ::lh_math_point_fields. */
    lh_math_point_fields(lh_math_scalar_t);
};
/** @copybrief lh_gl_point */
typedef struct lh_gl_point lh_gl_point_t;

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief X coordinate of @p self.
 *
 * @param self Point to read; must not be null (asserted in debug builds).
 * @return The horizontal coordinate. Nothing is checked or changed.
 */
lh_math_scalar_t
lh_gl_point_get_x(const lh_gl_point_t *self);

/**
 * @brief Y coordinate of @p self.
 *
 * @param self Point to read; must not be null (asserted in debug builds).
 * @return The vertical coordinate. Nothing is checked or changed.
 */
lh_math_scalar_t
lh_gl_point_get_y(const lh_gl_point_t *self);

/**
 * @brief Set the X coordinate of @p self.
 *
 * @param self Point to change; must not be null (asserted in debug builds).
 * @param x New horizontal coordinate; any value is accepted.
 */
lh_void
lh_gl_point_set_x(lh_gl_point_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y coordinate of @p self.
 *
 * @param self Point to change; must not be null (asserted in debug builds).
 * @param y New vertical coordinate; any value is accepted.
 */
lh_void
lh_gl_point_set_y(lh_gl_point_t *self, lh_math_scalar_t y);

/**
 * @brief Fill @p self from explicit coordinates.
 *
 * Both fields are overwritten; the previous contents of @p self do not matter,
 * so the memory may be uninitialized.
 *
 * @param self Point to fill; must not be null (asserted in debug builds).
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 */
lh_void
lh_gl_point_init(lh_gl_point_t *self, lh_math_scalar_t x, lh_math_scalar_t y);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief True if @p a and @p b have the same `x` and the same `y`.
 *
 * @param a First point; must not be null (asserted in debug builds).
 * @param b Second point; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when both coordinates match, else ::lh_bool_false.
 */
lh_bool_t
lh_gl_point_eq(const lh_gl_point_t *a, const lh_gl_point_t *b);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: `x` first, then `y` when the `x` values are equal. Equal points count
 * as "at least".
 *
 * @param self Point to test; must not be null (asserted in debug builds).
 * @param minimum Lower bound; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self is equal to or later than @p minimum.
 */
lh_bool_t
lh_gl_point_is_at_least(const lh_gl_point_t *self, const lh_gl_point_t *minimum);

/**
 * @brief Fill @p self with the origin `(0, 0)`.
 *
 * @param self Point to fill; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_point_init_empty(lh_gl_point_t *self);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same field order as ::lh_gl_point_is_at_least.
 *
 * @param self Point to test; must not be null (asserted in debug builds).
 * @param other Point to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes before @p other.
 */
lh_bool_t
lh_gl_point_is_less(const lh_gl_point_t *self, const lh_gl_point_t *other);

/**
 * @brief Translate @p self by `(@p dx, @p dy)` into @p out.
 *
 * @p out may be the same object as @p self. Nothing is checked: the sums may
 * overflow like any `lh_math_scalar_t` addition.
 *
 * @param self Point to move; must not be null (asserted in debug builds).
 * @param dx Horizontal shift.
 * @param dy Vertical shift.
 * @param out Receives the moved point; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_point_offset(const lh_gl_point_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                   lh_gl_point_t *out);

/**
 * @brief Component-wise minimum of @p a and @p b into @p out.
 *
 * `out.x = min(a.x, b.x)` and `out.y = min(a.y, b.y)`, so the result is not
 * necessarily either input. @p out may be the same object as @p a or @p b.
 *
 * @param a First point; must not be null (asserted in debug builds).
 * @param b Second point; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_point_min(const lh_gl_point_t *a, const lh_gl_point_t *b, lh_gl_point_t *out);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same field order as ::lh_gl_point_is_at_least.
 *
 * @param self Point to test; must not be null (asserted in debug builds).
 * @param other Point to compare with; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p self comes after @p other.
 */
lh_bool_t
lh_gl_point_is_greater(const lh_gl_point_t *self, const lh_gl_point_t *other);

/**
 * @brief Component-wise maximum of @p a and @p b into @p out.
 *
 * `out.x = max(a.x, b.x)` and `out.y = max(a.y, b.y)`, so the result is not
 * necessarily either input. @p out may be the same object as @p a or @p b.
 *
 * @param a First point; must not be null (asserted in debug builds).
 * @param b Second point; must not be null (asserted in debug builds).
 * @param out Receives the result; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_point_max(const lh_gl_point_t *a, const lh_gl_point_t *b, lh_gl_point_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_POINT_H */