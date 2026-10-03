/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_t.
 */

#ifndef LH_ENTITY_FIELDS_H
#define LH_ENTITY_FIELDS_H

/**
 * @def lh_entity_fields(class_type, node_type, list_type, flags_type)
 * @brief The fields every entity starts with; a derived class's instance
 *        struct expands this first, then adds its own.
 *
 * The parent is not stored here: an entity is a block of an ownership tree
 * (::lh_memory_tree_alloc_child), whose parent is the parent entity.
 *
 * - `entity_class`: what kind of entity it is.
 * - `sibling`: this entity's node in the parent's `children`.
 * - `children`: the child entities, oldest first.
 * - `handlers`: the handlers added with ::lh_entity_add_handler, in order.
 * - `flags`: `lh_entity_flags_*` bits.
 *
 * @param class_type Type `entity_class` points to (::lh_entity_class_t).
 * @param node_type  Type of `sibling` (::lh_list_node_t).
 * @param list_type  Type of `children` and `handlers` (::lh_list_t).
 * @param flags_type Type of `flags` (::lh_entity_flags_t).
 */
#define lh_entity_fields(class_type, node_type, list_type, flags_type)                             \
    const class_type *entity_class;                                                                \
    node_type sibling;                                                                             \
    list_type children;                                                                            \
    list_type handlers;                                                                            \
    flags_type flags

#endif /* LH_ENTITY_FIELDS_H */
