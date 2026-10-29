/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_event_subscription_t.
 *
 * Member declaration macro for ::lh_event_subscription_t.
 */

#ifndef LH_EVENT_SUBSCRIPTION_FIELDS_H
#define LH_EVENT_SUBSCRIPTION_FIELDS_H

/** @brief Declare subscription state; type parameters follow the member order. */
#define lh_event_subscription_fields(node_type, retired_type, owner_type, callback_type,           \
                                     dispose_type, context_type, dead_type)                        \
    node_type node;                                                                                \
    retired_type retired;                                                                          \
    owner_type owner;                                                                              \
    callback_type callback;                                                                        \
    dispose_type dispose;                                                                          \
    context_type context;                                                                          \
    dead_type dead

#endif /* LH_EVENT_SUBSCRIPTION_FIELDS_H */
