/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_os_module_event_bus_t.
 *
 * The type parameters follow the member order: the entry table, the list of removed
 * subscriptions, the publication depth and the live subscription count.
 */

#ifndef LH_OS_MODULE_EVENT_BUS_FIELDS_H
#define LH_OS_MODULE_EVENT_BUS_FIELDS_H

/** @brief Declare bus state; type parameters follow the member order. */
#define lh_os_module_event_bus_fields(entries_type, retired_type, depth_type, count_type)          \
    entries_type entries;                                                                          \
    retired_type retired;                                                                          \
    depth_type depth;                                                                              \
    count_type count

#endif /* LH_OS_MODULE_EVENT_BUS_FIELDS_H */
