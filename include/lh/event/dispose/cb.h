/**
 * @file cb.h
 * @brief Pointer to subscription context disposal.
 *
 * Pointer to subscription context disposal.
 */

#ifndef LH_EVENT_DISPOSE_CB_H
#define LH_EVENT_DISPOSE_CB_H

#include <lh/event/dispose/fn.h>
#include <lh/util/ptr.h>

/** @brief Optional pointer to ::lh_event_dispose_fn. */
#define lh_event_dispose_cb lh_ptr_of(lh_event_dispose_fn)

#endif /* LH_EVENT_DISPOSE_CB_H */
