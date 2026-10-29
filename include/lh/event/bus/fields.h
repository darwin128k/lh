/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_event_bus_t.
 *
 * Member declaration macro for ::lh_event_bus_t.
 */

#ifndef LH_EVENT_BUS_FIELDS_H
#define LH_EVENT_BUS_FIELDS_H

/** @brief Declare bus state; type parameters follow the member order. */
#define lh_event_bus_fields(type_0, type_1, type_2, type_3) \
    type_0 entries; \
    type_1 retired; \
    type_2 depth; \
    type_3 count

#endif /* LH_EVENT_BUS_FIELDS_H */
