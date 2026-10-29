/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_event_bus_t.
 *
 * Member declaration macro for ::lh_event_bus_t.
 */

#ifndef LH_EVENT_BUS_FIELDS_H
#define LH_EVENT_BUS_FIELDS_H

/** @brief Declare bus state; type parameters follow the member order. */
#define lh_event_bus_fields(entries_type, retired_type, depth_type, count_type)                    \
    entries_type entries;                                                                          \
    retired_type retired;                                                                          \
    depth_type depth;                                                                              \
    count_type count

#endif /* LH_EVENT_BUS_FIELDS_H */
