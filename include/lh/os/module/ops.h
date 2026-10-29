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
#include <lh/compiler/extern/c.h>

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
struct lh_os_module_ops
{
    /** Members: ::lh_os_module_ops_fields. */
    lh_os_module_ops_fields(lh_os_module_start_cb, lh_os_module_stop_cb);
};
/** @copybrief lh_os_module_ops */
typedef struct lh_os_module_ops lh_os_module_ops_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The start method, called by ::lh_os_module_start (level 1).
 *
 * ::lh_null makes starting a no-op.
 *
 * @param self The method table; checked by ::lh_assert_runtime_ref.
 */
lh_os_module_start_cb
lh_os_module_ops_get_start_fn(const lh_os_module_ops_t *self);

/**
 * @brief Store the start method, called by ::lh_os_module_start (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The method table; checked by ::lh_assert_runtime_ref.
 * @param start The new value.
 */
lh_void
lh_os_module_ops_set_start_fn(lh_os_module_ops_t *self, lh_os_module_start_cb start);

/**
 * @brief The stop method, called by ::lh_os_module_stop (level 1).
 *
 * ::lh_null makes stopping a no-op.
 *
 * @param self The method table; checked by ::lh_assert_runtime_ref.
 */
lh_os_module_stop_cb
lh_os_module_ops_get_stop_fn(const lh_os_module_ops_t *self);

/**
 * @brief Store the stop method, called by ::lh_os_module_stop (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The method table; checked by ::lh_assert_runtime_ref.
 * @param stop The new value.
 */
lh_void
lh_os_module_ops_set_stop_fn(lh_os_module_ops_t *self, lh_os_module_stop_cb stop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_OPS_H */
