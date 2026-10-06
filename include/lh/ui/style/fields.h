/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

#include <lh/ui/paint.h>

/**
 * @def lh_ui_style_fields()
 * @brief Paint recipe for one entity.
 *
 * `fill` is not owned and may be shared; ::lh_null means no fill.
 */
#define lh_ui_style_fields()                                                                        \
    const lh_ui_paint_t *fill

#endif /* LH_UI_STYLE_FIELDS_H */
