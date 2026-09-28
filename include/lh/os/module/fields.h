/**
 * @file fields.h
 * @brief Member fields of ::lh_os_module_t.
 *
 * The module is the main object: an image, its own methods, its state, its
 * place in the tree. The image itself is an ::lh_os_shared_t — opening it
 * and holding its OS reference is that type's job, not the module's.
 * Loading other modules is a privilege it may hold (`loader`), not what it
 * is — a leaf module has none.
 */

#ifndef LH_OS_MODULE_FIELDS_H
#define LH_OS_MODULE_FIELDS_H

/**
 * @def lh_os_module_fields(image_type, flag_type, ops_type, data_type, loader_type, node_type)
 * @brief Image, lifecycle, state, and place in the tree.
 *
 * @param image_type  Type of `image`, the open shared image (::lh_os_shared_t).
 * @param flag_type   Type of `started` (stop must run) (::lh_bool_t).
 * @param ops_type    Type of `ops`, the module's own methods
 *                    (`const ::lh_os_module_ops_t *`).
 * @param data_type   Type of `data`, the module's private state (::lh_ptr).
 * @param loader_type Type of `owner`, the loader holding this module (null
 *                    for a root), and of `loader`, the privilege to load
 *                    children (`struct lh_os_loader *`).
 * @param node_type   Type of `node`, the link among the owner's children
 *                    (::lh_list_node_t). Unlinked for a root.
 */
#define lh_os_module_fields(image_type, flag_type, ops_type, data_type, loader_type, node_type)    \
    image_type image;                                                                              \
    flag_type started;                                                                             \
    ops_type ops;                                                                                  \
    data_type data;                                                                                \
    loader_type owner;                                                                             \
    loader_type loader;                                                                            \
    node_type node

#endif /* LH_OS_MODULE_FIELDS_H */
