/**
 * @file fn.h
 * @brief A module's own lifecycle methods (::lh_os_module_ops_t).
 *
 * The module decides what starting and stopping means; whoever runs it
 * (an ::lh_os_loader_t, or the program for its own root module) only
 * calls these at the right moment. Same split as a kernel module's
 * `init` / `exit`.
 */

#ifndef LH_OS_MODULE_FN_H
#define LH_OS_MODULE_FN_H

#include <lh/bool.h>

struct lh_os_module;

/**
 * @typedef lh_os_module_start_fn
 * @brief Bring @p self up once its image is open.
 *
 * May keep private state with ::lh_os_module_set_data and ask for the
 * loader privilege with ::lh_os_module_grant_loader.
 *
 * @return ::lh_bool_false to refuse. ::lh_os_module_stop is then not called;
 *         whatever start acquired before refusing, it releases itself.
 */
typedef lh_bool_t(lh_os_module_start_fn)(struct lh_os_module *self);

/**
 * @typedef lh_os_module_stop_fn
 * @brief Release what ::lh_os_module_start_fn acquired.
 *
 * Runs after @p self's children (if it holds a loader) are already
 * unloaded, and before the image is closed.
 */
typedef void(lh_os_module_stop_fn)(struct lh_os_module *self);

#endif /* LH_OS_MODULE_FN_H */
