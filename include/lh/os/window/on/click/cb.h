/**
 * @file cb.h
 * @brief Pointer to ::lh_os_window_on_click_fn.
 */

#ifndef LH_OS_WINDOW_ON_CLICK_CB_H
#define LH_OS_WINDOW_ON_CLICK_CB_H

#include <lh/os/window/on/click/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_window_on_click_cb
 * @brief Pointer to ::lh_os_window_on_click_fn.
 */
#define lh_os_window_on_click_cb lh_ptr_of(lh_os_window_on_click_fn)

#endif /* LH_OS_WINDOW_ON_CLICK_CB_H */
