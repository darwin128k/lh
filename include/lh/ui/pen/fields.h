/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_pen_t.
 */

#ifndef LH_UI_PEN_FIELDS_H
#define LH_UI_PEN_FIELDS_H

/**
 * @def lh_ui_pen_fields(paint_type, width_type)
 * @brief What the outline is painted with, and how wide it is.
 *
 * @param paint_type Type of the outline paint (held by value).
 * @param width_type Type of the line width.
 */
#define lh_ui_pen_fields(paint_type, width_type)                                                    \
    paint_type paint;                                                                               \
    width_type width

#endif /* LH_UI_PEN_FIELDS_H */
