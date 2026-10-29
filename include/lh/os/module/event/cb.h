/**
 * @file cb.h
 * @brief Pointer to a synchronous event handler.
 *
 * What a subscription stores; see ::lh_os_module_event_fn.
 */

#ifndef LH_OS_MODULE_EVENT_CB_H
#define LH_OS_MODULE_EVENT_CB_H

#include <lh/os/module/event/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to ::lh_os_module_event_fn. */
#define lh_os_module_event_cb lh_ptr_of(lh_os_module_event_fn)

#endif /* LH_OS_MODULE_EVENT_CB_H */
