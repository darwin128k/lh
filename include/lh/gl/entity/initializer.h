/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_entity_t.
 */

#ifndef LH_GL_ENTITY_INITIALIZER_H
#define LH_GL_ENTITY_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_entity_initializer(rect, parent, cls, node)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_entity_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_entity_fields. This does not leave `node` unlinked (self-linked);
 * for an entity that will be added to a layer, prefer ::lh_gl_entity_init,
 * which does.
 *
 * @param rect   Initializer of the ::lh_gl_rect_t @c rect (::lh_gl_rect_initializer).
 * @param parent The owning ::lh_gl_layer_t pointer, or ::lh_null for a root.
 * @param cls    Pointer to the `const` ::lh_gl_entity_class_t of the entity.
 * @param node   Initializer of the ::lh_list_node_t @c node.
 *
 * Example usage:
 * @code{.c}
 * lh_gl_entity_t entity = lh_gl_entity_initializer(
 *     lh_gl_rect_initializer(lh_gl_point_initializer(0, 0),
 *                            lh_gl_size_initializer(64, 32)),  // rect
 *     lh_null,                                                 // parent
 *     &label_class,                                            // cls
 *     {0});                                                    // node
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_entity_t
 */
#define lh_gl_entity_initializer(rect, parent, cls, node) lh_initializer(rect, parent, cls, node)

#endif /* LH_GL_ENTITY_INITIALIZER_H */
