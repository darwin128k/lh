/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_event_t.
 */

#ifndef LH_ENTITY_EVENT_FIELDS_H
#define LH_ENTITY_EVENT_FIELDS_H

/**
 * @def lh_entity_event_fields(code_type, entity_type, param_type, flag_type)
 * @brief One event on its way up the tree.
 *
 * @param code_type   Type of `code`, what happened (::lh_uint_t).
 * @param entity_type Type that `target` (the entity it was sent to) and
 *                    `current` (the one being notified now) point to.
 * @param param_type  Type of `param`, the sender's extra data (::lh_ptr).
 * @param flag_type   Type of `stopped` (::lh_bool_t).
 */
#define lh_entity_event_fields(code_type, entity_type, param_type, flag_type)                      \
    code_type code;                                                                                \
    entity_type *target;                                                                           \
    entity_type *current;                                                                          \
    param_type param;                                                                              \
    flag_type stopped

#endif /* LH_ENTITY_EVENT_FIELDS_H */
