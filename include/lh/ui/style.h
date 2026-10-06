/**
 * @file style.h
 * @brief How an entity is painted: ::lh_ui_style_t.
 *
 * Holds a fill paint (::lh_ui_paint_t pointer, not owned). Outline, font,
 * and the rest come later.
 */

#ifndef LH_UI_STYLE_H
#define LH_UI_STYLE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/paint.h>
#include <lh/ui/style/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_style
 * @typedef lh_ui_style_t
 * @brief Paint recipe for one entity.
 */
struct lh_ui_style
{
    lh_ui_style_fields();
};
typedef struct lh_ui_style lh_ui_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with no fill paint.
 */
lh_void
lh_ui_style_init(lh_ui_style_t *self);

/**
 * @brief Fill paint of @p self, or ::lh_null when none.
 */
const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self);

/**
 * @brief Point @p self at @p fill. The paint is not copied.
 *
 * ::lh_null clears the fill.
 */
lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
