/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_scroll_t.
 */

#ifndef LH_GL_SCROLL_FIELDS_H
#define LH_GL_SCROLL_FIELDS_H

/**
 * @def lh_gl_scroll_fields(layer_type)
 * @brief The layer whose direct children a scroll moves.
 *
 * @param layer_type ::lh_gl_layer_t. The member is `layer`, borrowed. Null
 *                   means a shift does nothing. The scroll does not own the
 *                   layer or its children.
 */
#define lh_gl_scroll_fields(layer_type)                                                            \
    layer_type *layer

#endif /* LH_GL_SCROLL_FIELDS_H */
