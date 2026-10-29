/**
 * @file fields.h
 * @brief Member declaration macro for ::lh_os_module_event_subscription_t.
 *
 * The type parameters follow the member order: the two list links, the owner tag, the
 * handler, the context release call, the context and the removed flag.
 */

#ifndef LH_OS_MODULE_EVENT_SUBSCRIPTION_FIELDS_H
#define LH_OS_MODULE_EVENT_SUBSCRIPTION_FIELDS_H

/** @brief Declare subscription state; type parameters follow the member order. */
#define lh_os_module_event_subscription_fields(node_type, retired_type, owner_type, callback_type, \
                                               dispose_type, context_type, dead_type)              \
    node_type node;                                                                                \
    retired_type retired;                                                                          \
    owner_type owner;                                                                              \
    callback_type callback;                                                                        \
    dispose_type dispose;                                                                          \
    context_type context;                                                                          \
    dead_type dead

#endif /* LH_OS_MODULE_EVENT_SUBSCRIPTION_FIELDS_H */
