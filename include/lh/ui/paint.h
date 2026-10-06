/**
 * @file paint.h
 * @brief Paint: ::lh_ui_paint_t, nothing or one solid ::lh_ui_color_t.
 *
 * The color is held by value, so a paint never dangles. ::lh_ui_brush_t is an
 * alias of this type; ::lh_ui_pen_t embeds one. Stop maps live on
 * ::lh_ui_gradient_t; paint has no gradient kind yet.
 */

#ifndef LH_UI_PAINT_H
#define LH_UI_PAINT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/paint/fields.h>
#include <lh/ui/paint/kind.h>
#include <lh/void.h>

/**
 * @struct lh_ui_paint
 * @typedef lh_ui_paint_t
 * @brief Nothing, or a solid color (also brush, and the paint of a pen).
 */
struct lh_ui_paint
{
    lh_ui_paint_fields(lh_ui_paint_kind_t, lh_ui_color_t);
};
typedef struct lh_ui_paint lh_ui_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the empty paint (::lh_ui_paint_kind_none).
 */
lh_void
lh_ui_paint_init(lh_ui_paint_t *self);

/**
 * @brief Fill @p self with a copy of @p color (::lh_ui_paint_kind_solid).
 *
 * ::lh_null gives the empty paint.
 */
lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color);

/**
 * @brief What @p self holds.
 */
lh_ui_paint_kind_t
lh_ui_paint_get_kind(const lh_ui_paint_t *self);

/**
 * @brief True when @p self paints nothing.
 */
lh_bool_t
lh_ui_paint_is_empty(const lh_ui_paint_t *self);

/**
 * @brief Solid color of @p self, or ::lh_null when it is not solid.
 *
 * The pointer is into @p self and lives as long as it does.
 */
const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PAINT_H */
