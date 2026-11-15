/**
 * @file fields.h
 * @brief Canvas drawing state: the borrowed target and the call table.
 */

#ifndef LH_GL_CANVAS_FIELDS_H
#define LH_GL_CANVAS_FIELDS_H

/**
 * @brief @p target_type is the opaque handle the backend draws into.
 *        @p backend_type is the call table; null means the canvas draws nothing.
 *        Origin, clip and mask are not stored: each call receives a ::lh_gl_place_t.
 */
#define lh_gl_canvas_fields(target_type, backend_type)                                             \
    target_type target;                                                                           \
    const backend_type *backend

#endif /* LH_GL_CANVAS_FIELDS_H */
