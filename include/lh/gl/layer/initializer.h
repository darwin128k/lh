/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_layer_t.
 */

#ifndef LH_GL_LAYER_INITIALIZER_H
#define LH_GL_LAYER_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_layer_initializer(entity, items)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_layer_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_layer_fields. For a layer that is ready to use, prefer
 * ::lh_gl_layer_init, which also sets the class and an empty list; this macro
 * is for a zeroed or fully spelled-out layer.
 *
 * @param entity Initializer of the ::lh_gl_entity_t @c entity (::lh_gl_entity_initializer).
 * @param items  Initializer of the ::lh_list_t @c items.
 *
 * @see lh_initializer
 * @see lh_gl_layer_t
 */
#define lh_gl_layer_initializer(entity, items) lh_initializer(entity, items)

#endif /* LH_GL_LAYER_INITIALIZER_H */
