/**
 * @file paint.h
 * @brief Paint: ::lh_ui_paint_t, a pointer to a solid ::lh_ui_color_t.
 *
 * ::lh_ui_brush_t and ::lh_ui_pen_t are aliases of this type. The color is
 * not owned: it must outlive the paint, and several paints may share one.
 * Stop maps live on ::lh_ui_gradient_t; paint does not point at them yet.
 */

#ifndef LH_UI_PAINT_H
#define LH_UI_PAINT_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/paint/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_paint
 * @typedef lh_ui_paint_t
 * @brief A solid color pointer (also brush / pen).
 */
struct lh_ui_paint
{
    lh_ui_paint_fields(lh_ui_color_t);
};
typedef struct lh_ui_paint lh_ui_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The empty paint: no color (::lh_null).
 */
lh_ui_paint_t
lh_ui_paint_make_empty(void);

/**
 * @brief Make a paint that points at @p color. @p color is not copied.
 */
lh_ui_paint_t
lh_ui_paint_make(const lh_ui_color_t *color);

/**
 * @brief Fill @p self with the empty paint.
 */
lh_void
lh_ui_paint_init(lh_ui_paint_t *self);

/**
 * @brief Point @p self at @p color. The color is not copied.
 *
 * ::lh_null clears the color.
 */
lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color);

/**
 * @brief Color of @p self, or ::lh_null when it has none.
 */
const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PAINT_H */
