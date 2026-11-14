/**
 * @file decl.h
 * @brief Forward declaration of ::lh_gl_layer_t.
 *
 * Only the name: an entity points at the layer that owns it, and a layer starts
 * with an entity, so neither header can include the other. Code that only needs
 * to hold or pass a layer pointer includes this file; the definition and the
 * methods are in `lh/gl/layer.h`.
 */

#ifndef LH_GL_LAYER_DECL_H
#define LH_GL_LAYER_DECL_H

/**
 * @struct lh_gl_layer
 * @brief An entity that holds other entities; defined in `lh/gl/layer.h`.
 */
struct lh_gl_layer;
/** @copybrief lh_gl_layer */
typedef struct lh_gl_layer lh_gl_layer_t;

#endif /* LH_GL_LAYER_DECL_H */