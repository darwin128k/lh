/**
 * @file shadow.h
 * @brief A soft shadow cast by a box: ::lh_ui_shadow_t.
 *
 * Not a paint pass and not a shape: a shadow is the same box, shifted, with its
 * alpha falling off over a spread. Everything here is a value the caller sets
 * and ::lh_ui_canvas_shadow draws, so one shadow can hang on a card and on the
 * frame of a window alike.
 *
 * Nothing here reads a pixel. The alpha of the shadow at a point is
 * ::lh_ui_shadow_alpha_at, which is a plain function of the shadow and the box —
 * testable without a canvas, and the same answer whether it is drawn into a
 * surface, a memory buffer or a screenshot.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_SHADOW_H
#define LH_UI_SHADOW_H

#include <lh/ui/color.h>
#include <lh/ui/rect.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/shadow.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

/**
 * @struct lh_ui_shadow
 * @brief One soft shadow: a color, a fade distance and a shift.
 *
 * Read and written through the accessors; the struct is the caller's, not the
 * canvas's.
 */
struct lh_ui_shadow
{
    lh_ui_color_t color;     /**< peak colour; its alpha is the peak opacity */
    lh_ui_scalar_t spread;   /**< how far the fade reaches, in pixels */
    lh_ui_scalar_t offset_x; /**< shift right */
    lh_ui_scalar_t offset_y; /**< shift down */
};
/** @copybrief lh_ui_shadow */
typedef struct lh_ui_shadow lh_ui_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A shadow that paints nothing: no colour, no spread.
 *
 * Use ::lh_ui_shadow_set_color and ::lh_ui_shadow_set_spread_v, then
 * ::lh_ui_shadow_set_offset if the light is not straight ahead.
 */
lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self);

/**
 * @brief True when @p self would paint nothing: no peak alpha, or no fade to
 *        paint it over.
 *
 * The same answer ::lh_ui_canvas_shadow gives (it needs an outset and there is
 * none), told without a rect and without a canvas — what a caller draws every
 * frame asks this first instead of measuring anything.
 */
lh_bool_t
lh_ui_shadow_is_empty(const lh_ui_shadow_t *self);

/**
 * @brief Peak colour of @p self. Its alpha is the strongest the shadow gets.
 */
lh_void
lh_ui_shadow_copy_color(const lh_ui_shadow_t *self, lh_ui_color_t *out);

/**
 * @brief Set the peak colour.
 */
lh_void
lh_ui_shadow_set_color(lh_ui_shadow_t *self, lh_ui_color_t color);

/**
 * @brief Fade distance of @p self in pixels; `0` paints nothing.
 */
lh_ui_scalar_t
lh_ui_shadow_get_spread(const lh_ui_shadow_t *self);

/**
 * @brief Set the fade distance. Negative becomes zero.
 */
lh_void
lh_ui_shadow_set_spread_v(lh_ui_shadow_t *self, lh_ui_scalar_t spread);

/**
 * @brief Shift of @p self. Positive @p offset_x is right, @p offset_y is down.
 */
lh_void
lh_ui_shadow_set_offset(lh_ui_shadow_t *self, lh_ui_scalar_t offset_x, lh_ui_scalar_t offset_y);

/**
 * @brief Horizontal shift of @p self.
 */
lh_ui_scalar_t
lh_ui_shadow_get_offset_x(const lh_ui_shadow_t *self);

/**
 * @brief Vertical shift of @p self.
 */
lh_ui_scalar_t
lh_ui_shadow_get_offset_y(const lh_ui_shadow_t *self);

/**
 * @brief Pixels the shadow needs around @p rect: the spread plus the shift.
 *
 * The padding a caller has to leave around the box so nothing of the shadow is
 * cut. Zero when @p self would paint nothing.
 */
lh_ui_scalar_t
lh_ui_shadow_get_outset(const lh_ui_shadow_t *self, const lh_ui_rect_t *rect);

/**
 * @brief How much of @p self reaches (@p x, @p y) of @p rect with corners
 *        rounded by @p radius: `0 .. 255`, already scaled by the colour's alpha.
 *
 * Zero inside the box, where the fill is, and one spread past the edge. The
 * fade straddles the *shifted* edge rather than holding a flat band for the
 * whole offset, which is what makes a shadow look like light and not like a
 * second box.
 */
lh_byte_t
lh_ui_shadow_alpha_at(const lh_ui_shadow_t *self, lh_ui_scalar_t x, lh_ui_scalar_t y,
                      const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief ::lh_ui_shadow_alpha_at at the center of pixel (@p x, @p y).
 *
 * The same center ::lh_ui_radius_coverage samples, so a pixel the fill covers
 * wholly is outside the shadow, and a rounded corner fades along the fill's
 * circle rather than along `width / 2` of the box.
 */
lh_byte_t
lh_ui_shadow_alpha_at_pixel(const lh_ui_shadow_t *self, lh_s32_t x, lh_s32_t y,
                            const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief Signed distance from (@p x, @p y) to @p rect with corners rounded by
 *        @p radius, in pixels: negative inside, zero on the edge.
 *
 * The one piece of geometry both this and anything that wants a soft edge of
 * its own needs, and a plain function of its arguments.
 */
lh_s32_t
lh_ui_shadow_distance(lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect,
                      lh_ui_scalar_t radius);

/**
 * @brief The smoothstep of @p distance into `0 .. @p peak`: `peak` one spread
 *        inside, zero one spread outside.
 */
lh_byte_t
lh_ui_shadow_falloff(lh_s32_t distance, lh_ui_scalar_t spread, lh_byte_t peak);

/** Signed distance, in the same fixed point as ::lh_ui_radius_coverage, from the
   sample (@p px, @p py) to the rounded edge. The centre is the middle of the
   two edges, not `width / 2` of a truncated corner: an odd width then leans a
   pixel to one side, and the shadow on the right of a box is heavier than the
   shadow on the left of it. */
lh_s64_t
lh_ui_shadow_signed_distance(lh_s64_t px, lh_s64_t py, const lh_ui_rect_t *rect,
                             lh_ui_scalar_t radius);

/**
 * @brief ::lh_ui_shadow_falloff in fixed point: @p distance and @p spread share one
 *        fixed-point scale, and the result is rounded instead of truncated (what a
 *        pixel center, half a pixel off the integer grid, is faded by).
 */
lh_byte_t
lh_ui_shadow_falloff_sub(lh_s64_t distance, lh_s64_t spread, lh_byte_t peak);

/**
 * @brief The color of the shadow, to change in place (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_color_t *
lh_ui_shadow_get_color_mut(lh_ui_shadow_t *self);

/**
 * @brief Store how far the shadow is moved right (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param offset_x The new value.
 */
lh_void
lh_ui_shadow_set_offset_x(lh_ui_shadow_t *self, lh_ui_scalar_t offset_x);

/**
 * @brief Store how far the shadow is moved down (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param offset_y The new value.
 */
lh_void
lh_ui_shadow_set_offset_y(lh_ui_shadow_t *self, lh_ui_scalar_t offset_y);

/**
 * @brief The color of the shadow (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_color_t *
lh_ui_shadow_get_color(const lh_ui_shadow_t *self);

/**
 * @brief Store how far the shadow spreads (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param spread The new value.
 */
lh_void
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_ui_scalar_t spread);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_H */