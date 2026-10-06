/**
 * @file reason.h
 * @brief Why a window closed: ::lh_os_window_close_reason_t.
 */

#ifndef LH_OS_WINDOW_CLOSE_REASON_H
#define LH_OS_WINDOW_CLOSE_REASON_H

/**
 * @enum lh_os_window_close_reason
 * @brief Who tore the native window down.
 */
typedef enum lh_os_window_close_reason
{
    /** ::lh_os_window_close / ::lh_os_window_deinit. */
    lh_os_window_close_reason_api = 0,
    /** User or OS (caption close, task manager, owner destroyed, …). */
    lh_os_window_close_reason_os = 1
} lh_os_window_close_reason_t;

#endif /* LH_OS_WINDOW_CLOSE_REASON_H */
