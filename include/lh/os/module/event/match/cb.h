/**
 * @file cb.h
 * @brief Pointer to a subscription context matching function.
 *
 * The bus borrows this pointer only during a subscription search.
 */

#ifndef LH_OS_MODULE_EVENT_MATCH_CB_H
#define LH_OS_MODULE_EVENT_MATCH_CB_H

#include <lh/os/module/event/match/fn.h>
#include <lh/util/ptr.h>

/** @brief Optional context matcher; null matches every context. */
#define lh_os_module_event_match_cb lh_ptr_of(lh_os_module_event_match_fn)

#endif /* LH_OS_MODULE_EVENT_MATCH_CB_H */
