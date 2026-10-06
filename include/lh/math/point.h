/**
 * @file point.h
 * @brief A 2D point: ::lh_math_point_t with ::lh_math_scalar_t `x, y`.
 *
 * `x` is horizontal, `y` is vertical (top-left origin, like every OS window
 * coordinate system).
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_point_get_*` / `lh_math_point_set_*` accessors. The struct is
 * defined here only so it can be embedded by value (e.g. as
 * `lh_math_rect_t::origin`).
 */

#ifndef LH_MATH_POINT_H
#define LH_MATH_POINT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/math/scalar.h>
#include <lh/math/point/fields.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/vec2.h>
#endif
#include <lh/void.h>

/**
 * @struct lh_math_point
 * @typedef lh_math_point_t
 * @brief A 2D point: `x` is horizontal, `y` is vertical.
 */
struct lh_math_point
{
    lh_math_point_fields(lh_math_scalar_t);
};
typedef struct lh_math_point lh_math_point_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_point_t` from explicit coordinates.
 */
lh_math_point_t
lh_math_point_make(lh_math_scalar_t x, lh_math_scalar_t y);

/**
 * @brief The origin: `(0, 0)`.
 */
lh_math_point_t
lh_math_point_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X coordinate of @p self.
 */
lh_math_scalar_t
lh_math_point_get_x(const lh_math_point_t *self);

/**
 * @brief Y coordinate of @p self.
 */
lh_math_scalar_t
lh_math_point_get_y(const lh_math_point_t *self);

/**
 * @brief Set the X coordinate of @p self.
 */
lh_void
lh_math_point_set_x(lh_math_point_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y coordinate of @p self.
 */
lh_void
lh_math_point_set_y(lh_math_point_t *self, lh_math_scalar_t y);

/* ── Conversions ─────────────────────────────────────────────────────────── */

#if LH_LIBRARY_OPTION_MATH_FPU
/**
 * @brief Widen @p self to continuous coordinates.
 *
 * Exact: a pixel coordinate is already an integer, so no value is lost.
 *
 * @param self Point to widen.
 * @return `(self.x, self.y)`.
 */
lh_math_vec2_t
lh_math_point_to_vec2(lh_math_point_t self);

/**
 * @brief Narrow @p v to the pixel it lands on.
 *
 * Rounds to the nearest integer, halves toward the lower pixel: `3.4` gives
 * pixel `3`, `3.6` gives `4`, and exactly `3.5` gives `3`. That is the same
 * rule the rasterizer uses to pick the pixels an entity covers
 * (`ceil(x - 0.5f)` in ::lh_entity_2d_draw_background), so a position narrowed here
 * lands on the pixel that drawing at that position would touch.
 *
 * @param v Continuous position.
 * @return The pixel containing @p v.
 */
lh_math_point_t
lh_math_vec2_to_point(lh_math_vec2_t v);
#endif

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_point_eq(const lh_math_point_t *a, const lh_math_point_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_math_point_t
lh_math_point_offset(const lh_math_point_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_POINT_H */