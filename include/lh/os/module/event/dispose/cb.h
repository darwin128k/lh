/**
 * @file cb.h
 * @brief Pointer to subscription context disposal.
 *
 * What a subscription stores; see ::lh_os_module_event_dispose_fn.
 */

#ifndef LH_OS_MODULE_EVENT_DISPOSE_CB_H
#define LH_OS_MODULE_EVENT_DISPOSE_CB_H

#include <lh/os/module/event/dispose/fn.h>
#include <lh/util/ptr.h>

/** @brief Optional pointer to ::lh_os_module_event_dispose_fn. */
#define lh_os_module_event_dispose_cb lh_ptr_of(lh_os_module_event_dispose_fn)

#endif /* LH_OS_MODULE_EVENT_DISPOSE_CB_H */
