/**
 * @file ops.h
 * @brief A module's method table (::lh_os_module_ops_t).
 *
 * A loaded image exports one object of this type under the name its
 * loader was given (see ::lh_os_loader_init) — the way a kernel module
 * carries its `struct module`. The root module (the program itself, via
 * ::lh_os_module_bind) is handed its table with ::lh_os_module_set_ops.
 *
 * Either member may be ::lh_null: that step is a no-op.
 */

#ifndef LH_OS_MODULE_OPS_H
#define LH_OS_MODULE_OPS_H

#include <lh/os/module/cb.h>

/**
 * @def lh_os_module_ops_fields(start_type, stop_type)
 * @brief The lifecycle methods.
 *
 * @param start_type Type of `start` (::lh_os_module_start_cb).
 * @param stop_type  Type of `stop` (::lh_os_module_stop_cb).
 */
#define lh_os_module_ops_fields(start_type, stop_type)                                             \
    start_type start;                                                                              \
    stop_type stop

/**
 * @struct lh_os_module_ops
 * @brief Start and stop. Fields via ::lh_os_module_ops_fields.
 */
typedef struct lh_os_module_ops
{
    lh_os_module_ops_fields(lh_os_module_start_cb, lh_os_module_stop_cb);
} lh_os_module_ops_t;

#endif /* LH_OS_MODULE_OPS_H */
