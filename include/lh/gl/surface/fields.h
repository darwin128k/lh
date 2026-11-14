/**
 * @file fields.h
 * @brief Storage fields of a surface; the surface owns no layer memory.
 */

#ifndef LH_GL_SURFACE_FIELDS_H
#define LH_GL_SURFACE_FIELDS_H

#include <lh/config.h>

/** @brief Size, borrowed root, fixed dirty rectangles and their count. */
#define lh_gl_surface_fields(size_type, layer_type, rect_type, count_type)                         \
    size_type size;                                                                                \
    layer_type *root;                                                                              \
    rect_type dirty[LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX];                                       \
    count_type dirty_count

#endif /* LH_GL_SURFACE_FIELDS_H */
