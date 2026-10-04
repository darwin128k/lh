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
    lh_ui_style_fields(lh_ui_color_t);
};
typedef struct lh_ui_style lh_ui_style_t;

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
