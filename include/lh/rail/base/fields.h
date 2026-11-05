/**
 * @file fields.h
 * @brief Member fields of ::lh_rail_base_t.
 */

#ifndef LH_RAIL_BASE_FIELDS_H
#define LH_RAIL_BASE_FIELDS_H

/**
 * @def lh_rail_base_fields(now_fn_type, open_fn_type, handle_fn_type, wait_fn_type,
 *                          start_fn_type, sleep_fn_type)
 * @brief The calls a platform gives rail, in four groups.
 *
 * The clock: `now_cb`. The lock: `lock_open_cb`, `lock_close_cb`, `lock_cb`,
 * `unlock_cb`. The signal a thread sleeps on: `signal_open_cb`,
 * `signal_close_cb`, `signal_wait_cb`, `signal_wake_cb`. The thread:
 * `thread_start_cb`, `thread_join_cb`, `sleep_cb`. A null member means the
 * platform has no such call.
 *
 * @param now_fn_type    ::lh_rail_base_now_fn.
 * @param open_fn_type   ::lh_rail_base_open_fn: the two opens.
 * @param handle_fn_type ::lh_rail_base_handle_fn: closes, lock, unlock, wake,
 *                       join.
 * @param wait_fn_type   ::lh_rail_base_wait_fn.
 * @param start_fn_type  ::lh_rail_base_start_fn.
 * @param sleep_fn_type  ::lh_rail_base_sleep_fn.
 */
#define lh_rail_base_fields(now_fn_type, open_fn_type, handle_fn_type, wait_fn_type,               \
                            start_fn_type, sleep_fn_type)                                          \
    now_fn_type *now_cb;                                                                           \
    open_fn_type *lock_open_cb;                                                                    \
    handle_fn_type *lock_close_cb;                                                                 \
    handle_fn_type *lock_cb;                                                                       \
    handle_fn_type *unlock_cb;                                                                     \
    open_fn_type *signal_open_cb;                                                                  \
    handle_fn_type *signal_close_cb;                                                               \
    wait_fn_type *signal_wait_cb;                                                                  \
    handle_fn_type *signal_wake_cb;                                                                \
    start_fn_type *thread_start_cb;                                                                \
    handle_fn_type *thread_join_cb;                                                                \
    sleep_fn_type *sleep_cb

#endif /* LH_RAIL_BASE_FIELDS_H */
