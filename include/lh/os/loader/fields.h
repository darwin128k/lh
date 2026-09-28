/**
 * @file fields.h
 * @brief Member fields of ::lh_os_loader_t.
 *
 * The mechanism only: which modules were loaded, by whom, and how an image
 * names its methods. What a child does lives in the child
 * (::lh_os_module_ops_t), not here.
 */

#ifndef LH_OS_LOADER_FIELDS_H
#define LH_OS_LOADER_FIELDS_H

/**
 * @def lh_os_loader_fields(vector_type, module_type, entry_type)
 * @brief Children, owner, and the exported name of a child's method table.
 *
 * @param vector_type Type of `modules`: one `lh_os_module_t *` per child,
 *                    each allocated on its own so its address never moves
 *                    (::lh_vector_t).
 * @param module_type Type of `owner`, the module holding this privilege
 *                    (`lh_os_module_t *`).
 * @param entry_type  Type of `entry`, the symbol a child exports its
 *                    ::lh_os_module_ops_t under (::lh_str_t).
 */
#define lh_os_loader_fields(vector_type, module_type, entry_type)                                  \
    vector_type modules;                                                                           \
    module_type owner;                                                                             \
    entry_type entry

#endif /* LH_OS_LOADER_FIELDS_H */
