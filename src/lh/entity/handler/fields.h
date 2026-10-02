/**
 * @file fields.h
 * @brief Library-private: member fields of an entity handler record.
 */

#ifndef LH_SRC_ENTITY_HANDLER_FIELDS_H
#define LH_SRC_ENTITY_HANDLER_FIELDS_H

/**
 * @def lh_entity_handler_fields(node_type, handler_type, data_type)
 * @brief One ::lh_entity_add_handler call: its place in the entity's
 *        `handlers` list, the function and its data.
 */
#define lh_entity_handler_fields(node_type, handler_type, data_type)                               \
    node_type node;                                                                                \
    handler_type *handler;                                                                         \
    data_type user_data

#endif /* LH_SRC_ENTITY_HANDLER_FIELDS_H */
