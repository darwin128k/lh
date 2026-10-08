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

#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/shadow.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

/**
 * @struct lh_ui_shadow
 * @typedef lh_ui_shadow_t
 * @brief One soft shadow: a color, a fade distance and a shift.
 *
 * Read and written through the accessors; the struct is the caller's, not the
 * canvas's.
 */
struct lh_ui_shadow
{
    lh_ui_color_t color;      /* peak colour; its alpha is the peak opacity */
    lh_ui_scalar_t spread;    /* how far the fade reaches, in pixels */
    lh_ui_scalar_t offset_x;  /* shift right / down */
    lh_ui_scalar_t offset_y;
};
typedef struct lh_ui_shadow lh_ui_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A shadow that paints nothing: no colour, no spread.
 *
 * Use ::lh_ui_shadow_set_color and ::lh_ui_shadow_set_spread, then
 * ::lh_ui_shadow_set_offset if the light is not straight ahead.
 */
lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self);

/**
 * @brief Peak colour of @p self. Its alpha is the strongest the shadow gets.
 */
lh_ui_color_t
lh_ui_shadow_get_color(const lh_ui_shadow_t *self);

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
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_ui_scalar_t spread);

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
 * @brief Signed distance from (@p x, @p y) to @p rect with corners rounded by
 *        @p radius, in pixels: negative inside, zero on the edge.
 *
 * The one piece of geometry both this and anything that wants a soft edge of
 * its own needs, and a plain function of its arguments.
 */
lh_s32_t
lh_ui_shadow_distance(lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief The smoothstep of @p distance into `0 .. @p peak`: `peak` one spread
 *        inside, zero one spread outside.
 */
lh_byte_t
lh_ui_shadow_falloff(lh_s32_t distance, lh_ui_scalar_t spread, lh_byte_t peak);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_H */