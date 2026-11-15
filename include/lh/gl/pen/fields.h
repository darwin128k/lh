/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_pen_t.
 */

#ifndef LH_GL_PEN_FIELDS_H
#define LH_GL_PEN_FIELDS_H

/**
 * @def lh_gl_pen_fields(paint_type, width_type)
 * @brief What the outline is painted with, and how wide it is.
 *
 * @param paint_type Type of the outline paint (held by value).
 * @param width_type Type of the line width.
 */
#define lh_gl_pen_fields(paint_type, width_type)                                                   \
    paint_type paint;                                                                              \
    width_type width

#endif /* LH_GL_PEN_FIELDS_H */
