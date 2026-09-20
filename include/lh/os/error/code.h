/**
 * @file code.h
 * @brief OS-layer codes on top of ::lh_error_code_t.
 *
 * Native failures go through ::lh_os_system_error_capture (`GetLastError` /
 * `errno`). These codes are the lh-side values stored by
 * ::lh_os_set_last_error when the check is ours, not the OS's.
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
 * @typedef lh_os_error_code_t
 * @brief Same representation as ::lh_error_code_t for OS-layer last-error codes.
 */
typedef lh_error_code_t lh_os_error_code_t;

#endif /* LH_OS_ERROR_CODE_H */
