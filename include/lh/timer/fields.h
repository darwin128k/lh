/**
 * @file fields.h
 * @brief Member fields of ::lh_timer_t.
 */

#ifndef LH_TIMER_FIELDS_H
#define LH_TIMER_FIELDS_H

#include <lh/bool.h>
#include <lh/list/node.h>
#include <lh/ptr.h>
#include <lh/timer/cb.h>
#include <lh/timer/tick.h>

/**
 * @def lh_timer_fields(tick_type, cb_type)
 * @brief Period, last fire, flags, callback, and list link.
 *
 * @param tick_type Type of period / last (::lh_tick_t).
 * @param cb_type   Type of the callback pointer.
 */
#define lh_timer_fields(tick_type, cb_type)                                                        \
    lh_list_node_t link;                                                                            \
    tick_type period;                                                                               \
    tick_type last;                                                                                 \
    lh_bool_t repeat;                                                                               \
    lh_bool_t paused;                                                                               \
    cb_type cb;                                                                                     \
    lh_ptr context

#endif /* LH_TIMER_FIELDS_H */
