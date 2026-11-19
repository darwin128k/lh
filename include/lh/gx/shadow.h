/**
 * @file shadow.h
 * @brief A soft shadow a box casts: ::lh_gx_shadow_t.
 *
 * The same box, shifted, its alpha falling off over a spread — what CSS calls a
 * `box-shadow`. A style points at one (::lh_gx_style_get_shadow), and the box class
 * casts it before it draws itself, through the canvas's `fill_shadow` slot
 * (`lh/gx/canvas/fill_shadow/fn.h`). A canvas without that slot draws no shadow.
 *
 * A shadow reaches beyond the box (::lh_gx_shadow_get_bounds): an entity with one
 * is drawn and reported over its place and its shadow together
 * (::lh_gx_entity_get_ink), so the shadow is not cut at the box's edge and moving
 * the box clears where its shadow was.
 *
 * Every pixel's alpha is the core's one reading of a shadow (`lh/core/pixel/shadow.h`,
 * ::lh_gx_shadow_get_pixel_shadow). A value, usually a constant in flash
 * (::lh_gx_shadow_initializer); it points at nothing.
 */

#ifndef LH_GX_SHADOW_H
#define LH_GX_SHADOW_H

#include <lh/core/pixel/shadow.h>
#include <lh/gx/color.h>
#include <lh/gx/rect.h>
#include <lh/gx/shadow/fields.h>

/**
 * @struct lh_gx_shadow
 * @brief A colour, a spread and a shift. Members: ::lh_gx_shadow_fields.
 */
struct lh_gx_shadow
{
    /** Members: ::lh_gx_shadow_fields. */
    lh_gx_shadow_fields(lh_gx_color_t, lh_gx_scalar_t);
};
/** @copybrief lh_gx_shadow */
typedef struct lh_gx_shadow lh_gx_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The colour, `0xAARRGGBB`; its alpha is the shadow's darkest (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_gx_color_t
lh_gx_shadow_get_color(const lh_gx_shadow_t *self);

/**
 * @brief Store the colour (level 1).
 *
 * @param self  The shadow; checked by ::lh_assert_runtime_ref.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_shadow_set_color(lh_gx_shadow_t *self, lh_gx_color_t color);

/**
 * @brief How far the fade reaches either side of the edge (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_shadow_get_spread(const lh_gx_shadow_t *self);

/**
 * @brief Store how far the fade reaches (level 1).
 *
 * @param self   The shadow; checked by ::lh_assert_runtime_ref.
 * @param spread Pixels; `0` casts nothing.
 */
lh_void
lh_gx_shadow_set_spread(lh_gx_shadow_t *self, lh_gx_scalar_t spread);

/**
 * @brief How far right the shadow lies from the box (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_shadow_get_offset_x(const lh_gx_shadow_t *self);

/**
 * @brief Store how far right (level 1).
 *
 * @param self     The shadow; checked by ::lh_assert_runtime_ref.
 * @param offset_x Pixels.
 */
lh_void
lh_gx_shadow_set_offset_x(lh_gx_shadow_t *self, lh_gx_scalar_t offset_x);

/**
 * @brief How far down the shadow lies from the box (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_shadow_get_offset_y(const lh_gx_shadow_t *self);

/**
 * @brief Store how far down (level 1).
 *
 * @param self     The shadow; checked by ::lh_assert_runtime_ref.
 * @param offset_y Pixels.
 */
lh_void
lh_gx_shadow_set_offset_y(lh_gx_shadow_t *self, lh_gx_scalar_t offset_y);

/**
 * @brief A shadow of @p color, fading over @p spread, shifted by @p offset_x,
 *        @p offset_y.
 *
 * @param self     The shadow; checked by the setters.
 * @param color    `0xAARRGGBB`.
 * @param spread   Pixels.
 * @param offset_x Pixels right.
 * @param offset_y Pixels down.
 */
lh_void
lh_gx_shadow_init(lh_gx_shadow_t *self, lh_gx_color_t color, lh_gx_scalar_t spread,
                  lh_gx_scalar_t offset_x, lh_gx_scalar_t offset_y);

/**
 * @brief The rectangle the shadow of @p box can colour: the box shifted, grown by
 *        the spread and by a pixel more for the edge's rounding.
 *
 * @param self The shadow; must not be null.
 * @param box  The casting box, screen coordinates.
 * @param out  The rectangle; must not be null.
 */
lh_void
lh_gx_shadow_get_bounds(const lh_gx_shadow_t *self, const lh_gx_rect_t *box, lh_gx_rect_t *out);

/**
 * @brief The shadow of @p box with corners of @p radius as the core's
 *        (::lh_pixel_shadow_t): every length in ::LH_PIXEL_ROUND_SUBPIXEL units,
 *        the colour's alpha as the peak.
 *
 * @param self   The shadow; must not be null.
 * @param box    The casting box, screen coordinates.
 * @param radius Its corners' radius, pixels, at most half its shorter side.
 * @param out    The core's shadow; must not be null.
 */
lh_void
lh_gx_shadow_get_pixel_shadow(const lh_gx_shadow_t *self, const lh_gx_rect_t *box,
                              lh_gx_scalar_t radius, lh_pixel_shadow_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_SHADOW_H */
