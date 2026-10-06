/**
 * @file style.h
 * @brief How an entity is painted: ::lh_ui_style_t.
 *
 * Placeholder. Fill, outline, font, and the rest are not decided yet; the
 * type exists so UI code can hold a style before those pieces return.
 */

#ifndef LH_UI_STYLE_H
#define LH_UI_STYLE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/style/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_style
 * @typedef lh_ui_style_t
 * @brief Paint recipe for one entity. Empty for now.
 */
struct lh_ui_style
{
    lh_ui_style_fields();
};
typedef struct lh_ui_style lh_ui_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the empty style.
 */
lh_void
lh_ui_style_init(lh_ui_style_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_STYLE_H */
