/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_paint_t.
 */

#ifndef LH_UI_PAINT_FIELDS_H
#define LH_UI_PAINT_FIELDS_H

/**
 * @def lh_ui_paint_fields(kind_type, color_type, gradient_type)
 * @brief What the paint holds: its solid color (a copy), or the gradient it lays over
 *        the box it fills (pointed at, not owned -- a gradient is a table of stops,
 *        and a paint is copied by value into every style that uses it).
 *
 * @param kind_type     Type of the paint kind.
 * @param color_type    Type of the solid color.
 * @param gradient_type Type of the gradient pointed at.
 */
#define lh_ui_paint_fields(kind_type, color_type, gradient_type)                                   \
    kind_type kind;                                                                                \
    color_type color;                                                                              \
    const gradient_type *gradient

#endif /* LH_UI_PAINT_FIELDS_H */
