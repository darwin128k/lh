/**
 * @file style.h
 * @brief A shareable description of how a box is painted (LVGL's `lv_style_t`).
 *
 * The color lives here, not on the entity. An entity only points at a style
 * (::lh_entity_2d_set_style); many entities may point at one. The screen
 * reads the background from that style when it draws.
 */

#ifndef LH_UI_STYLE_H
#define LH_UI_STYLE_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color.h>
#include <lh/ui/style/fields.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_ui_style
 * @typedef lh_ui_style_t
 * @brief Fields via ::lh_ui_style_fields.
 */
struct lh_ui_style
{
    lh_ui_style_fields(lh_ui_color_t, lh_int_t);
};
typedef struct lh_ui_style lh_ui_style_t;

/**
 * @brief Clear @p self to "paints nothing": every color transparent, every
 *        width and radius 0.
 *
 * A style is a plain record with no constructor, so a caller that writes one
 * on the stack and sets only the background leaves the rest of it as whatever
 * was on the stack, and the rasterizer reads all of it. Start from here.
 */
lh_void
lh_ui_style_init(lh_ui_style_t *self);

/**
 * @brief Background color (alpha 0: the style paints nothing).
 */
lh_ui_color_t
lh_ui_style_get_bg_color(const lh_ui_style_t *self);

/**
 * @brief Set the background color.
 *
 * Does not redraw by itself: a style does not know which entities point at
 * it. ::lh_entity_2d_set_style marks them, and so does ::lh_entity_invalidate
 * after the color of a style already in use changes.
 */
lh_void
lh_ui_style_set_bg_color(lh_ui_style_t *self, lh_ui_color_t bg_color);

/**
 * @brief Color of glyphs drawn over the background (alpha 0: no glyphs).
 */
lh_ui_color_t
lh_ui_style_get_text_color(const lh_ui_style_t *self);

/**
 * @brief Set the glyph color. Like ::lh_ui_style_set_bg_color, this does
 *        not redraw by itself.
 */
lh_void
lh_ui_style_set_text_color(lh_ui_style_t *self, lh_ui_color_t text_color);

/**
 * @brief Color of the outline (alpha 0: no outline).
 *
 * Meaningful only while ::lh_ui_style_get_border_width is not 0.
 */
lh_ui_color_t
lh_ui_style_get_border_color(const lh_ui_style_t *self);

/**
 * @brief Set the outline color. Like ::lh_ui_style_set_bg_color, this does
 *        not redraw by itself.
 */
lh_void
lh_ui_style_set_border_color(lh_ui_style_t *self, lh_ui_color_t border_color);

/**
 * @brief Outline thickness in pixels, centred on the edge. 0 is no outline.
 *
 * The box the border describes is the same box the background describes: a
 * border of 1 does not grow the entity, it eats a pixel of the background.
 */
lh_int_t
lh_ui_style_get_border_width(const lh_ui_style_t *self);

/**
 * @brief Set the outline thickness. Negative counts as 0.
 */
lh_void
lh_ui_style_set_border_width(lh_ui_style_t *self, lh_int_t border_width);

/**
 * @brief Corner radius in pixels. 0 is a square, and it is clamped to half
 *        the shorter side when the box is drawn.
 */
lh_int_t
lh_ui_style_get_radius(const lh_ui_style_t *self);

/**
 * @brief Set the corner radius. Negative counts as 0.
 */
lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_int_t radius);

/**
 * @brief Give the outline @p color at @p width and round the corners to
 *        @p radius, in one call.
 *
 * The same three setters for a caller who has the values already; a caller who
 * is describing a box rather than editing a style wants this.
 */
lh_void
lh_ui_style_set_border(lh_ui_style_t *self, lh_ui_color_t color, lh_int_t width);

/**
 * @brief Turn the outline off: color alpha 0 and width 0.
 */
lh_void
lh_ui_style_clear_border(lh_ui_style_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
