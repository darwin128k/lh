/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(brush_type, pen_type)
 * @brief The fill and the outline.
 *
 * @param brush_type Type of the fill.
 * @param pen_type   Type of the outline.
 */
#define lh_ui_style_fields(brush_type, pen_type)                                                    \
    brush_type brush;                                                                               \
    pen_type pen

#endif /* LH_UI_STYLE_FIELDS_H */
