/**
 * @file handle.h
 * @brief Raw OS directory handle and its "not open" sentinel.
 *
 * POSIX `DIR *` and a Windows find-handle plus name cache are both stored
 * as ::lh_ptr. ::lh_null means not open.
 */

#ifndef LH_OS_SYSTEM_FS_DIR_HANDLE_H
#define LH_OS_SYSTEM_FS_DIR_HANDLE_H

#include <lh/null.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_system_fs_dir_handle_t
 * @brief Opaque OS directory iterator (`DIR *` / private find state).
 */
typedef lh_ptr lh_os_system_fs_dir_handle_t;

/**
 * @def LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID
 * @brief Sentinel for "not open".
 */
#define LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID lh_null

#endif /* LH_OS_SYSTEM_FS_DIR_HANDLE_H */
