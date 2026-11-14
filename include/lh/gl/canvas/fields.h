/**
 * @file fields.h
 * @brief The canvas drawing origin and clip, both in root coordinates.
 */

#ifndef LH_GL_CANVAS_FIELDS_H
#define LH_GL_CANVAS_FIELDS_H

/** @brief Members of ::lh_gl_canvas_t: origin and clip. */
#define lh_gl_canvas_fields(point_type, rect_type)                                                 \
    point_type origin;                                                                             \
    rect_type clip

#endif /* LH_GL_CANVAS_FIELDS_H */
