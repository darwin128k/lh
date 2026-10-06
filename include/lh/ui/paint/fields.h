/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_paint_t.
 */

#ifndef LH_UI_PAINT_FIELDS_H
#define LH_UI_PAINT_FIELDS_H

/**
 * @def lh_ui_paint_fields(kind_type, color_type)
 * @brief What the paint holds, and its solid color. The color is a copy.
 *
 * @param kind_type  Type of the paint kind.
 * @param color_type Type of the solid color.
 */
#define lh_ui_paint_fields(kind_type, color_type)                                                   \
    kind_type kind;                                                                                 \
    color_type color

#endif /* LH_UI_PAINT_FIELDS_H */
