/**
 * @file handle.h
 * @brief Raw OS file handle type and its "no file" sentinel.
 *
 * A POSIX file descriptor (`int`, invalid = `-1`) and a Windows `HANDLE`
 * (`void *`, invalid = `INVALID_HANDLE_VALUE`) are different types, but
 * both sentinels are the all-ones bit pattern of a pointer-width integer.
 * Stored as ::lh_ssize_t so one sentinel is correct on every platform,
 * without this header naming `<windows.h>` or `<unistd.h>`.
 */

#ifndef LH_OS_FS_FILE_HANDLE_H
#define LH_OS_FS_FILE_HANDLE_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @typedef lh_os_fs_file_handle_t
 * @brief Raw OS file handle, stored by bit pattern.
 */
typedef lh_ssize_t lh_os_fs_file_handle_t;

/**
 * @def LH_OS_FS_FILE_HANDLE_INVALID
 * @brief Sentinel for "no file" (POSIX `-1` and Windows
 *        `INVALID_HANDLE_VALUE` as this type).
 */
#define LH_OS_FS_FILE_HANDLE_INVALID (lh_cast_static(lh_os_fs_file_handle_t, -1))

#endif /* LH_OS_FS_FILE_HANDLE_H */
