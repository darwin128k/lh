/**
 * @file style.h
 * @brief How an entity is painted: ::lh_ui_style_t.
 *
 * Holds a fill ::lh_ui_paint_t by value, a corner radius, and how text is
 * drawn: a font (not owned) and a text paint. Entities point at a style, so
 * one style is shared by every entity that looks the same. Outline and the
 * rest come later.
 */

#ifndef LH_UI_STYLE_H
#define LH_UI_STYLE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/font.h>
#include <lh/ui/paint.h>
#include <lh/ui/radius.h>
#include <lh/ui/scalar.h>
#include <lh/ui/style/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_style
 * @typedef lh_ui_style_t
 * @brief Paint recipe for one entity.
 */
struct lh_ui_style
{
    lh_ui_style_fields(lh_ui_paint_t, lh_ui_scalar_t, lh_ui_font_t);
};
typedef struct lh_ui_style lh_ui_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the empty fill paint, square corners, the default
 *        font (::lh_ui_font_get_default) and opaque black text.
 */
lh_void
lh_ui_style_init(lh_ui_style_t *self);

/**
 * @brief Fill paint of @p self. Never ::lh_null; may be empty.
 */
const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self);

/**
 * @brief Replace the fill of @p self with a copy of @p fill.
 *
 * ::lh_null clears the fill to the empty paint.
 */
lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill);

/**
 * @brief Solid fill color of @p self, or ::lh_null when the fill is not solid.
 */
const lh_ui_color_t *
lh_ui_style_get_fill_color(const lh_ui_style_t *self);

/**
 * @brief Corner radius of @p self (`0` = square). Not clamped: the canvas
 *        clamps it to the rect it fills.
 */
lh_ui_scalar_t
lh_ui_style_get_radius(const lh_ui_style_t *self);

/**
 * @brief Replace the corner radius of @p self. @p radius must not be negative;
 *        ::LH_UI_RADIUS_CIRCLE asks for the largest one.
 */
lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_ui_scalar_t radius);

/**
 * @brief Inner space of @p self on every side (`0` = content touches the
 *        edge): where a label starts its text, and the room a container
 *        leaves after its last child.
 */
lh_ui_scalar_t
lh_ui_style_get_padding(const lh_ui_style_t *self);

/**
 * @brief Replace the padding of @p self. @p padding must not be negative.
 */
lh_void
lh_ui_style_set_padding(lh_ui_style_t *self, lh_ui_scalar_t padding);

/**
 * @brief Font text is drawn with, or ::lh_null (no text drawn).
 */
const lh_ui_font_t *
lh_ui_style_get_font(const lh_ui_style_t *self);

/**
 * @brief Point @p self at @p font. Not copied; ::lh_null means no font.
 */
lh_void
lh_ui_style_set_font(lh_ui_style_t *self, const lh_ui_font_t *font);

/**
 * @brief Text paint of @p self. Never ::lh_null; may be empty.
 */
const lh_ui_paint_t *
lh_ui_style_get_text(const lh_ui_style_t *self);

/**
 * @brief Replace the text paint of @p self with a copy of @p text.
 *
 * ::lh_null clears it to the empty paint (no text drawn).
 */
lh_void
lh_ui_style_set_text(lh_ui_style_t *self, const lh_ui_paint_t *text);

/**
 * @brief Solid text color of @p self, or ::lh_null when the text paint is not
 *        solid.
 */
const lh_ui_color_t *
lh_ui_style_get_text_color(const lh_ui_style_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
