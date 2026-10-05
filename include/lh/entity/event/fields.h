/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_event_t.
 */

#ifndef LH_ENTITY_EVENT_FIELDS_H
#define LH_ENTITY_EVENT_FIELDS_H

/**
 * @def lh_entity_event_fields(code_type)
 * @brief Which event this is.
 *
 * @param code_type Type of the event code.
 */
#define lh_entity_event_fields(code_type)                                                           \
    code_type code

#endif /* LH_ENTITY_EVENT_FIELDS_H */
