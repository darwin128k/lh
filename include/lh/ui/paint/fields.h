/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_paint_t.
 */

#ifndef LH_UI_PAINT_FIELDS_H
#define LH_UI_PAINT_FIELDS_H

/**
 * @def lh_ui_paint_fields(kind_type, color_type, gradient_type)
 * @brief A solid color or a gradient. @p kind says which one is live.
 *
 * @param kind_type     Type of the tag.
 * @param color_type    Type of the solid color.
 * @param gradient_type Type of the gradient.
 */
#define lh_ui_paint_fields(kind_type, color_type, gradient_type)                                    \
    kind_type kind;                                                                                 \
    color_type color;                                                                               \
    gradient_type gradient

#endif /* LH_UI_PAINT_FIELDS_H */
