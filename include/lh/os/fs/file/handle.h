/**
 * @file handle.h
 * @brief Raw OS file handle type and its "no file" sentinel.
 *
 * A POSIX file descriptor (`int`, invalid = `-1`) and a Windows `HANDLE`
 * (`void *`, invalid = `INVALID_HANDLE_VALUE` = `(HANDLE)(LONG_PTR)-1`) are
 * different types, but both sentinels are the all-ones bit pattern of a
 * pointer-width integer. Storing the handle as ::lh_ssize_t makes one
 * sentinel (::LH_OS_FS_FILE_HANDLE_INVALID, `-1`) correct on every
 * platform, without this header ever naming `<windows.h>` or `<unistd.h>`.
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
 * @brief Sentinel for "no file" (matches POSIX `-1` and Windows
 *        `INVALID_HANDLE_VALUE` once both are read back as this type).
 */
#define LH_OS_FS_FILE_HANDLE_INVALID (lh_cast_static(lh_os_fs_file_handle_t, -1))

#endif /* LH_OS_FS_FILE_HANDLE_H */
