/**
 * @file shadow.h
 * @brief A soft shadow of a square box: ::lh_gl_shadow_t.
 *
 * A color, a spread and a shift. Nothing here reads a pixel. ::lh_gl_shadow_alpha_at
 * is the coverage at one point: zero inside the box the fill owns, and a fade
 * across the edge of the box after the shift. One spread inside that edge is
 * the full alpha, one spread outside is nothing. The fade is
 * `peak * (spread - distance) / (2 * spread)`, truncated, then clamped.
 *
 * A radius rounds that edge. `0` leaves the square. The radius is not stored
 * here: the box that casts the shadow brings it. ::lh_gl_shadow_fill_t paints
 * the coverage.
 */

#ifndef LH_GL_SHADOW_H
#define LH_GL_SHADOW_H

#include <lh/gl/color.h>
#include <lh/gl/rect.h>
#include <lh/gl/shadow/fields.h>

/**
 * @struct lh_gl_shadow
 * @brief Peak color, spread and shift. Fields via ::lh_gl_shadow_fields.
 */
struct lh_gl_shadow
{
    /** Members: ::lh_gl_shadow_fields. */
    lh_gl_shadow_fields(lh_gl_color_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_shadow */
typedef struct lh_gl_shadow lh_gl_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Peak color. */
const lh_gl_color_t *
lh_gl_shadow_get_color(const lh_gl_shadow_t *self);

/** @brief Mutable peak color. */
lh_gl_color_t *
lh_gl_shadow_get_color_mut(lh_gl_shadow_t *self);

/** @brief Copy @p color over the peak color. */
lh_void
lh_gl_shadow_set_color(lh_gl_shadow_t *self, const lh_gl_color_t *color);

/** @brief Fade distance. Zero and negative paint nothing. */
lh_gl_scalar_t
lh_gl_shadow_get_spread(const lh_gl_shadow_t *self);

/** @brief Store the fade distance as given. */
lh_void
lh_gl_shadow_set_spread(lh_gl_shadow_t *self, lh_gl_scalar_t spread);

/** @brief Shift right. */
lh_gl_scalar_t
lh_gl_shadow_get_offset_x(const lh_gl_shadow_t *self);

/** @brief Shift down. */
lh_gl_scalar_t
lh_gl_shadow_get_offset_y(const lh_gl_shadow_t *self);

/** @brief Store the shift right. */
lh_void
lh_gl_shadow_set_offset_x(lh_gl_shadow_t *self, lh_gl_scalar_t x);

/** @brief Store the shift down. */
lh_void
lh_gl_shadow_set_offset_y(lh_gl_shadow_t *self, lh_gl_scalar_t y);

/** @brief Store both shifts. Positive @p x is right, positive @p y is down. */
lh_void
lh_gl_shadow_set_offset(lh_gl_shadow_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y);

/** @brief No peak alpha, or no positive spread. */
lh_bool_t
lh_gl_shadow_is_empty(const lh_gl_shadow_t *self);

/**
 * @brief Black, alpha 0, spread 0, no shift.
 */
lh_void
lh_gl_shadow_init(lh_gl_shadow_t *self);

/**
 * @brief Radius limited to half the short side of @p rect. Negative and an
 *        empty rect are zero.
 */
lh_gl_scalar_t
lh_gl_shadow_clamp_radius(const lh_gl_rect_t *rect, lh_gl_scalar_t radius);

/**
 * @brief Signed distance from (@p x, @p y) to @p rect. Negative inside, zero
 *        on the outside edge. @p radius rounds the corners; `0` is square.
 */
lh_gl_scalar_t
lh_gl_shadow_distance(lh_gl_scalar_t x, lh_gl_scalar_t y, const lh_gl_rect_t *rect,
                      lh_gl_scalar_t radius);

/**
 * @brief `peak` one spread inside the edge, zero one spread outside.
 *
 * Truncates. A spread that is not positive, or a zero peak, returns 0.
 */
lh_gl_color_channel_t
lh_gl_shadow_falloff(lh_gl_scalar_t distance, lh_gl_scalar_t spread, lh_gl_color_channel_t peak);

/**
 * @brief Coverage at (@p x, @p y) of @p rect, already scaled by the peak alpha.
 *
 * Zero inside @p rect. Outside, the fade of the distance to @p rect shifted by
 * the shadow's offset. @p radius rounds the box; `0` is square.
 */
lh_gl_color_channel_t
lh_gl_shadow_alpha_at(const lh_gl_shadow_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                      const lh_gl_rect_t *rect, lh_gl_scalar_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SHADOW_H */
