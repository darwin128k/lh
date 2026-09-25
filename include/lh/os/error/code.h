/**
 * @file code.h
 * @brief OS-layer codes on top of ::lh_error_code_t.
 *
 * Native failures go through ::lh_os_system_error_capture
 * (`GetLastError` / `errno`) into the *separate* ::lh_os_system_error_t
 * slot — these codes are only the lh-side values raised by our own checks
 * (see `lh/os.h` / ::lh_os_set_last_error), not anything the OS reported.
 */

#ifndef LH_OS_ERROR_CODE_H
#define LH_OS_ERROR_CODE_H

#include <lh/error/code.h>

/**
 * @def lh_os_error_code_ok
 * @brief OS alias for the non-error sentinel (::lh_error_code_ok).
 */
#define lh_os_error_code_ok lh_error_code_ok

/**
 * @def lh_os_error_code_path_empty
 * @brief The path has no root and no segments.
 */
#define lh_os_error_code_path_empty 0x01

/**
 * @def lh_os_error_code_negative_size
 * @brief The OS reported a negative file size.
 */
#define lh_os_error_code_negative_size 0x02

/**
 * @def lh_os_error_code_out_of_memory
 * @brief ::lh_runtime_allocator_alloc returned ::lh_null.
 */
#define lh_os_error_code_out_of_memory 0x03

/**
 * @def lh_os_error_code_name_too_long
 * @brief A directory entry name exceeds ::lh_os_system_fs_dir_name_max.
 */
#define lh_os_error_code_name_too_long 0x04

/**
 * @def lh_os_error_code_not_open
 * @brief The call requires an open handle, but none is held.
 */
#define lh_os_error_code_not_open 0x05

/**
 * @typedef lh_os_error_code_t
 * @brief Same representation as ::lh_error_code_t for OS-layer last-error codes.
 */
typedef lh_error_code_t lh_os_error_code_t;

#endif /* LH_OS_ERROR_CODE_H */
