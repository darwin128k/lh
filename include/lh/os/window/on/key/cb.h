/**
 * @file cb.h
 * @brief Pointer to ::lh_os_window_on_key_fn.
 */

#ifndef LH_OS_WINDOW_ON_KEY_CB_H
#define LH_OS_WINDOW_ON_KEY_CB_H

#include <lh/os/window/on/key/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_window_on_key_cb
 * @brief Pointer to ::lh_os_window_on_key_fn.
 */
#define lh_os_window_on_key_cb lh_ptr_of(lh_os_window_on_key_fn)

#endif /* LH_OS_WINDOW_ON_KEY_CB_H */
