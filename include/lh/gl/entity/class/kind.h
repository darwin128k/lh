/**
 * @file kind.h
 * @brief What an entity is: a plain entity or a layer.
 *
 * Stored in the class of an entity (::lh_gl_entity_class_t), so a pointer to an
 * entity tells whether the object behind it starts a ::lh_gl_layer_t, without
 * any knowledge of the layer's type.
 */

#ifndef LH_GL_ENTITY_CLASS_KIND_H
#define LH_GL_ENTITY_CLASS_KIND_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_gl_entity_class_kind_t
 * @brief Discriminator for the class of an entity.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_gl_entity_class_kind_*`
 * constants below.
 */
typedef lh_u8_t lh_gl_entity_class_kind_t;

/**
 * @def lh_gl_entity_class_kind_entity
 * @brief A plain entity: a rectangle that draws itself and holds nothing.
 */
#define lh_gl_entity_class_kind_entity 0U

/**
 * @def lh_gl_entity_class_kind_layer
 * @brief A layer: the object starts with an entity and holds other entities.
 */
#define lh_gl_entity_class_kind_layer 1U

#endif /* LH_GL_ENTITY_CLASS_KIND_H */