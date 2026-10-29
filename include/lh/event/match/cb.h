/**
 * @file cb.h
 * @brief Pointer to a subscription context matching function.
 *
 * The bus borrows this pointer only during a subscription search.
 */

#ifndef LH_EVENT_MATCH_CB_H
#define LH_EVENT_MATCH_CB_H

#include <lh/event/match/fn.h>
#include <lh/util/ptr.h>

/** @brief Optional context matcher; null matches every context. */
#define lh_event_match_cb lh_ptr_of(lh_event_match_fn)

#endif /* LH_EVENT_MATCH_CB_H */
