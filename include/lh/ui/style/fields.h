/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(paint_type)
 * @brief Paint recipe for one entity.
 *
 * `fill` is held by value; the empty paint means no fill. The style itself
 * is what several entities share.
 *
 * @param paint_type Type of the fill paint.
 */
#define lh_ui_style_fields(paint_type)                                                              \
    paint_type fill

#endif /* LH_UI_STYLE_FIELDS_H */
