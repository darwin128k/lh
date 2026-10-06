/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_paint_t.
 */

#ifndef LH_UI_PAINT_FIELDS_H
#define LH_UI_PAINT_FIELDS_H

/**
 * @def lh_ui_paint_fields(color_type)
 * @brief The solid color of the paint. Not owned; may be shared.
 *
 * @param color_type Type of the color pointed at.
 */
#define lh_ui_paint_fields(color_type)                                                              \
    const color_type *color

#endif /* LH_UI_PAINT_FIELDS_H */
