/**
 * @file fields.h
 * @brief Canvas drawing state: origin, clip, and the borrowed target.
 */

#ifndef LH_GL_CANVAS_FIELDS_H
#define LH_GL_CANVAS_FIELDS_H

/**
 * @brief Origin and clip are in root coordinates. @p target_type is the opaque
 *        handle the backend draws into. @p backend_type is the call table;
 *        null means the canvas draws nothing.
 */
#define lh_gl_canvas_fields(point_type, rect_type, target_type, backend_type)                      \
    point_type origin;                                                                             \
    rect_type clip;                                                                                \
    target_type target;                                                                            \
    const backend_type *backend

#endif /* LH_GL_CANVAS_FIELDS_H */
