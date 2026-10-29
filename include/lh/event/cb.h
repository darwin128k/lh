/**
 * @file cb.h
 * @brief Pointer to a synchronous event handler.
 *
 * Pointer to a synchronous event handler.
 */

#ifndef LH_EVENT_CB_H
#define LH_EVENT_CB_H

#include <lh/event/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to ::lh_event_fn. */
#define lh_event_cb lh_ptr_of(lh_event_fn)

#endif /* LH_EVENT_CB_H */
