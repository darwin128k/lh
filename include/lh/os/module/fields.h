/**
 * @file fields.h
 * @brief Member fields of ::lh_os_module_t.
 *
 * The module is the main object: its image, its own methods, its state.
 * Loading other modules is a privilege it may hold (`loader`), not what it
 * is — a leaf module has none.
 */

#ifndef LH_OS_MODULE_FIELDS_H
#define LH_OS_MODULE_FIELDS_H

/**
 * @def lh_os_module_fields(path_type, handle_type, flag_type, ops_type, data_type, loader_type)
 * @brief Image, lifecycle, state, and place in the tree.
 *
 * @param path_type   Type of `path` (::lh_fs_path_t).
 * @param handle_type Type of `handle` (::lh_os_system_shared_handle_t).
 * @param flag_type   Type of `owned` (close must release the handle) and
 *                    `started` (stop must run) (::lh_bool_t).
 * @param ops_type    Type of `ops`, the module's own methods
 *                    (`const ::lh_os_module_ops_t *`).
 * @param data_type   Type of `data`, the module's private state (::lh_ptr).
 * @param loader_type Type of `owner`, the loader holding this module (null
 *                    for a root), and of `loader`, the privilege to load
 *                    children (`struct lh_os_loader *`).
 */
#define lh_os_module_fields(path_type, handle_type, flag_type, ops_type, data_type, loader_type)   \
    path_type path;                                                                                \
    handle_type handle;                                                                            \
    flag_type owned;                                                                               \
    flag_type started;                                                                             \
    ops_type ops;                                                                                  \
    data_type data;                                                                                \
    loader_type owner;                                                                             \
    loader_type loader

#endif /* LH_OS_MODULE_FIELDS_H */
