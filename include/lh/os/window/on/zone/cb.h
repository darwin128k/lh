/**
 * @file cb.h
 * @brief Pointer to ::lh_os_window_on_zone_fn.
 */

#ifndef LH_OS_WINDOW_ON_ZONE_CB_H
#define LH_OS_WINDOW_ON_ZONE_CB_H

#include <lh/os/window/on/zone/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_window_on_zone_cb
 * @brief Pointer to ::lh_os_window_on_zone_fn.
 */
#define lh_os_window_on_zone_cb lh_ptr_of(lh_os_window_on_zone_fn)

#endif /* LH_OS_WINDOW_ON_ZONE_CB_H */