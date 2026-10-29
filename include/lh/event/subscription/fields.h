/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_event_subscription_t.
 *
 * Member declaration macro for ::lh_event_subscription_t.
 */

#ifndef LH_EVENT_SUBSCRIPTION_FIELDS_H
#define LH_EVENT_SUBSCRIPTION_FIELDS_H

/** @brief Declare subscription state; type parameters follow the member order. */
#define lh_event_subscription_fields(type_0, type_1, type_2, type_3, type_4, type_5, type_6) \
    type_0 node; \
    type_1 retired; \
    type_2 owner; \
    type_3 callback; \
    type_4 dispose; \
    type_5 context; \
    type_6 dead

#endif /* LH_EVENT_SUBSCRIPTION_FIELDS_H */
