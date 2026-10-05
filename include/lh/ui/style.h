/**
 * @file style.h
 * @brief How one draw paints a rectangle: ::lh_ui_style_t, a brush and a pen.
 *
 * It is not a shared object.
 */

#ifndef LH_UI_STYLE_H
#define LH_UI_STYLE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/brush.h>
#include <lh/void.h>
#include <lh/ui/pen.h>
#include <lh/ui/style/fields.h>

/**
 * @struct lh_ui_style
 * @typedef lh_ui_style_t
 * @brief A fill and an outline.
 */
struct lh_ui_style
{
    lh_ui_style_fields(lh_ui_brush_t, lh_ui_pen_t);
};
typedef struct lh_ui_style lh_ui_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with @p brush and @p pen.
 */
lh_void
lh_ui_style_init(lh_ui_style_t *self, const lh_ui_brush_t *brush, const lh_ui_pen_t *pen);

/**
 * @brief Fill of @p self.
 */
const lh_ui_brush_t *
lh_ui_style_get_brush(const lh_ui_style_t *self);

/**
 * @brief Outline of @p self.
 */
const lh_ui_pen_t *
lh_ui_style_get_pen(const lh_ui_style_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
