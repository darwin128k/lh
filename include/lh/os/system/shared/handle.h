/**
 * @file handle.h
 * @brief Raw OS shared-image handle and its "no image" sentinel.
 *
 * Windows `HMODULE` and POSIX `dlopen`'s `void *` are both pointers.
 * Neither is a file handle: the empty value is null, not
 * `INVALID_HANDLE_VALUE`.
 */

#ifndef LH_OS_SYSTEM_SHARED_HANDLE_H
#define LH_OS_SYSTEM_SHARED_HANDLE_H

#include <lh/null.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_system_shared_handle_t
 * @brief Raw OS shared-image handle (`HMODULE` / `dlopen`).
 */
typedef lh_ptr lh_os_system_shared_handle_t;

/**
 * @def LH_OS_SYSTEM_SHARED_HANDLE_INVALID
 * @brief Sentinel for "no image".
 */
#define LH_OS_SYSTEM_SHARED_HANDLE_INVALID lh_null

#endif /* LH_OS_SYSTEM_SHARED_HANDLE_H */
