/**
 * @file fields.h
 * @brief Member fields of ::lh_os_shared_module_t.
 *
 * Prefix of a derived module (C++ `class AuthumModule : SharedModule`):
 * handle, whether ::lh_os_shared_close owns it, then the child table.
 * The vector's element size is the derived type, so children may be larger.
 */

#ifndef LH_OS_SHARED_MODULE_FIELDS_H
#define LH_OS_SHARED_MODULE_FIELDS_H

/**
 * @def lh_os_shared_module_fields(handle_type, owned_type, vector_type)
 * @brief Injects handle, close-ownership, and the child table.
 *
 * @param handle_type Type of the OS handle (::lh_os_shared_handle_t).
 * @param owned_type  Type of the ownership flag (::lh_bool_t).
 * @param vector_type Type of the child table (::lh_vector_t).
 */
#define lh_os_shared_module_fields(handle_type, owned_type, vector_type)                           \
    handle_type handle;                                                                            \
    owned_type owned;                                                                              \
    vector_type modules

#endif /* LH_OS_SHARED_MODULE_FIELDS_H */
