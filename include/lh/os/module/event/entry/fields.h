/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_os_module_event_entry_t.
 *
 * The type parameters follow the member order: the owned name, its length and the
 * subscriber list.
 */

#ifndef LH_OS_MODULE_EVENT_ENTRY_FIELDS_H
#define LH_OS_MODULE_EVENT_ENTRY_FIELDS_H

/** @brief Declare entry state; type parameters follow the member order. */
#define lh_os_module_event_entry_fields(name_type, length_type, subscriptions_type)                \
    name_type name;                                                                                \
    length_type length;                                                                            \
    subscriptions_type subscriptions

#endif /* LH_OS_MODULE_EVENT_ENTRY_FIELDS_H */
