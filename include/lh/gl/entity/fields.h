/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_entity_t.
 */

#ifndef LH_GL_ENTITY_FIELDS_H
#define LH_GL_ENTITY_FIELDS_H

/**
 * @def lh_gl_entity_fields(rect_type, layer_type, class_type, node_type)
 * @brief The area of an entity, its owner, its class and its list node.
 *
 * @param rect_type  ::lh_gl_rect_t: `rect`, the area of the entity, relative to
 *                   the origin of its parent.
 * @param layer_type ::lh_gl_layer_t; the member is `parent`, a pointer to the
 *                   layer that owns the entity (::lh_null for a root).
 * @param class_type ::lh_gl_entity_class_t; the member is `cls`, a pointer to
 *                   the `const` table of the entity's class. The member is not
 *                   called `class`, which is a keyword in C++.
 * @param node_type  ::lh_list_node_t: `node`, the intrusive link of the entity
 *                   in its parent's list of children. An entity is in at most
 *                   one list.
 */
#define lh_gl_entity_fields(rect_type, layer_type, class_type, node_type)                          \
    rect_type rect;                                                                                \
    layer_type *parent;                                                                            \
    const class_type *cls;                                                                         \
    node_type node

#endif /* LH_GL_ENTITY_FIELDS_H */
