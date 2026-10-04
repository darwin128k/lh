/**
 * @file theme.h
 * @brief Which style an entity starts with (LVGL's `lv_theme_t`).
 *
 * A theme owns a handful of ::lh_ui_style_t values. ::lh_ui_theme_init
 * fills them once from an accent color and a dark flag. ::lh_ui_theme_apply
 * points an entity at the style for its class. The flag is not read again
 * while drawing.
 *
 * The styles are not copied out. Anything that points at them needs the
 * theme to stay alive. Rewriting a theme that is already in use does not
 * redraw by itself, the same as changing a ::lh_ui_style_t.
 */

#ifndef LH_UI_THEME_H
#define LH_UI_THEME_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity.h>
#include <lh/ui/color.h>
#include <lh/ui/style.h>
#include <lh/ui/theme/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_theme
 * @brief Fields via ::lh_ui_theme_fields.
 */
struct lh_ui_theme
{
    lh_ui_theme_fields(lh_ui_style_t, lh_ui_color_t, lh_bool_t);
};
typedef struct lh_ui_theme lh_ui_theme_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from @p primary and @p dark.
 *
 * `dark` selects the neutral surface, text, and button. @p primary becomes
 * the accent style (::lh_ui_theme_get_primary_style). Alpha of @p primary
 * is kept.
 */
lh_void
lh_ui_theme_init(lh_ui_theme_t *self, lh_ui_color_t primary, lh_bool_t dark);

/**
 * @brief True when ::lh_ui_theme_init last filled @p self as a dark theme.
 */
lh_bool_t
lh_ui_theme_is_dark(const lh_ui_theme_t *self);

/**
 * @brief Accent color passed to ::lh_ui_theme_init.
 */
lh_ui_color_t
lh_ui_theme_get_primary(const lh_ui_theme_t *self);

/**
 * @brief Border color that matches the neutrals. For a window frame, not
 *        an entity fill.
 */
lh_ui_color_t
lh_ui_theme_get_border(const lh_ui_theme_t *self);

/**
 * @brief Surface style: screen background and its text color.
 */
const lh_ui_style_t *
lh_ui_theme_get_surface(const lh_ui_theme_t *self);

/**
 * @brief Text style: transparent background, glyph color of the neutrals.
 */
const lh_ui_style_t *
lh_ui_theme_get_text(const lh_ui_theme_t *self);

/**
 * @brief Button style while it is up.
 */
const lh_ui_style_t *
lh_ui_theme_get_button(const lh_ui_theme_t *self);

/**
 * @brief Button style while it is held.
 */
const lh_ui_style_t *
lh_ui_theme_get_button_pressed(const lh_ui_theme_t *self);

/**
 * @brief Accent style: background is ::lh_ui_theme_get_primary, text is
 *        light on a dark accent and dark on a light one.
 */
const lh_ui_style_t *
lh_ui_theme_get_primary_style(const lh_ui_theme_t *self);

/**
 * @brief Point @p entity at the style @p self has for its class.
 *
 * A screen gets the surface, a label the text style, a button the button
 * style and its pressed style. Other classes are left alone.
 */
lh_void
lh_ui_theme_apply(const lh_ui_theme_t *self, lh_entity_t *entity);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_THEME_H */
