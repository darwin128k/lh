/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_os_app_backend_t.
 */

#ifndef LH_OS_APP_BACKEND_FIELDS_H
#define LH_OS_APP_BACKEND_FIELDS_H

/**
 * @def lh_os_app_backend_fields(now_fn, wait_fn, wake_fn)
 * @brief The slots of the contract: one pointer per call.
 *
 * Each parameter is a function type; the members are pointers to it. The
 * backend's context is not a field: whoever holds the table holds the context
 * beside it and passes it into every call.
 *
 * @param now_fn ::lh_timer_clock_fn.
 * @param wait_fn ::lh_os_app_backend_wait_fn.
 * @param wake_fn ::lh_os_app_backend_wake_fn.
 */
#define lh_os_app_backend_fields(now_fn, wait_fn, wake_fn)                                         \
    now_fn *now;                                                                                   \
    wait_fn *wait;                                                                                 \
    wake_fn *wake

#endif /* LH_OS_APP_BACKEND_FIELDS_H */
