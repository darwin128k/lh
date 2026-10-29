/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_event_entry_t.
 *
 * Member declaration macro for ::lh_event_entry_t.
 */

#ifndef LH_EVENT_ENTRY_FIELDS_H
#define LH_EVENT_ENTRY_FIELDS_H

/** @brief Declare entry state; type parameters follow the member order. */
#define lh_event_entry_fields(type_0, type_1, type_2) \
    type_0 name; \
    type_1 length; \
    type_2 subscriptions

#endif /* LH_EVENT_ENTRY_FIELDS_H */
