/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_container_t.
 */

#ifndef LH_GL_CONTAINER_FIELDS_H
#define LH_GL_CONTAINER_FIELDS_H

/**
 * @def lh_gl_container_fields(layer_type, axis_type)
 * @brief A layer and the axes that shift its children.
 *
 * @param layer_type ::lh_gl_layer_t. The member is `layer`, borrowed. The
 *                   container does not own it and does not change its
 *                   rectangle.
 * @param axis_type  ::lh_gl_scroll_axis_t. `horizontal` and `vertical` are
 *                   borrowed and may be null. A null axis contributes no shift.
 */
#define lh_gl_container_fields(layer_type, axis_type)                                              \
    layer_type *layer;                                                                             \
    const axis_type *horizontal;                                                                   \
    const axis_type *vertical

#endif /* LH_GL_CONTAINER_FIELDS_H */
