/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_layer_t.
 */

#ifndef LH_GL_LAYER_FIELDS_H
#define LH_GL_LAYER_FIELDS_H

/**
 * @def lh_gl_layer_fields(entity_type, items_type)
 * @brief The entity a layer starts with and the list of its children.
 *
 * The `entity` member comes first, so a pointer to a layer is also a pointer to
 * its entity, and a layer can be put into a layer.
 *
 * @param entity_type ::lh_gl_entity_t: `entity`, the layer as an entity (area,
 *                    owner, class, list node). Must be the first member.
 * @param items_type  ::lh_list_t: `items`, the intrusive list of children,
 *                    linked through each child's `node`. The layer does not
 *                    own the children themselves, only the links to them.
 */
#define lh_gl_layer_fields(entity_type, items_type)                                                \
    entity_type entity;                                                                            \
    items_type items

#endif /* LH_GL_LAYER_FIELDS_H */
