/**
 * @file initializer.h
 * @brief Brace-enclosed initializers for ::lh_rail_base_t.
 */

#ifndef LH_RAIL_BASE_INITIALIZER_H
#define LH_RAIL_BASE_INITIALIZER_H

#include <lh/rail/base/handle/cb.h>
#include <lh/rail/base/now/cb.h>
#include <lh/rail/base/open/cb.h>
#include <lh/rail/base/sleep/cb.h>
#include <lh/rail/base/start/cb.h>
#include <lh/rail/base/wait/cb.h>

/**
 * @def lh_rail_base_initializer(now, lock_open, lock_close, lock, unlock, signal_open,
 *                               signal_close, signal_wait, signal_wake, thread_start,
 *                               thread_join, sleep)
 * @brief Produces a brace-enclosed initializer for ::lh_rail_base_t, every call
 *        in field order (lh/rail/base/fields.h).
 *
 * A platform fills one constant with it and the host copies that into the slot
 * (lh/rail/runtime/base.h). Pass ::lh_null for a call the platform has not; a
 * group with any null call is missing as a whole.
 */
#define lh_rail_base_initializer(now, lock_open, lock_close, lock, unlock, signal_open,            \
                                 signal_close, signal_wait, signal_wake, thread_start,             \
                                 thread_join, sleep)                                               \
    lh_initializer(lh_cast_static(lh_rail_base_now_cb, now),                                       \
                   lh_cast_static(lh_rail_base_open_cb, lock_open),                                \
                   lh_cast_static(lh_rail_base_handle_cb, lock_close),                             \
                   lh_cast_static(lh_rail_base_handle_cb, lock),                                   \
                   lh_cast_static(lh_rail_base_handle_cb, unlock),                                 \
                   lh_cast_static(lh_rail_base_open_cb, signal_open),                              \
                   lh_cast_static(lh_rail_base_handle_cb, signal_close),                           \
                   lh_cast_static(lh_rail_base_wait_cb, signal_wait),                              \
                   lh_cast_static(lh_rail_base_handle_cb, signal_wake),                            \
                   lh_cast_static(lh_rail_base_start_cb, thread_start),                            \
                   lh_cast_static(lh_rail_base_handle_cb, thread_join),                            \
                   lh_cast_static(lh_rail_base_sleep_cb, sleep))

/**
 * @def lh_rail_base_empty_initializer()
 * @brief A base with no calls at all: every group is missing.
 *
 * The slot starts so, until the host fills it.
 */
#define lh_rail_base_empty_initializer()                                                           \
    lh_rail_base_initializer(lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null,        \
                             lh_null, lh_null, lh_null, lh_null, lh_null)

#endif /* LH_RAIL_BASE_INITIALIZER_H */
