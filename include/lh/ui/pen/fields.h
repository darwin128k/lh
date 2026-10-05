/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_pen_t.
 */

#ifndef LH_UI_PEN_FIELDS_H
#define LH_UI_PEN_FIELDS_H

/**
 * @def lh_ui_pen_fields(paint_type, coord_type)
 * @brief Outline paint and thickness in pixels.
 *
 * The paint is a solid color or a gradient.
 *
 * @param paint_type Type of the paint.
 * @param coord_type Type of the width.
 */
#define lh_ui_pen_fields(paint_type, coord_type)                                                    \
    paint_type paint;                                                                               \
    coord_type width

#endif /* LH_UI_PEN_FIELDS_H */
