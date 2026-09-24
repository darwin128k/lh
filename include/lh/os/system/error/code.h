/**
 * @file code.h
 * @brief Native OS error code — a genuinely different namespace from ::lh_error_code_t.
 *
 * ::lh_os_error_code_t (`lh/os/error/code.h`) is OUR namespace: values lh
 * itself defines and raises, before ever asking the OS anything.
 *
 * ::lh_os_system_error_code_t is the OS's own namespace: whatever
 * `GetLastError()` (Windows `DWORD`) or `errno` (POSIX `int`) returns. The
 * two must never share a slot or a code space — a numeric value from one
 * has no defined meaning in the other, and small values collide (POSIX
 * `errno` 1..~140 overlaps exactly where lh might otherwise put its own
 * small codes).
 *
 * Represented as ::lh_u32_t regardless of platform: wide enough to hold a
 * Windows `DWORD` (unsigned 32-bit) and a POSIX `errno` (small, always
 * non-negative in practice) without loss, so callers don't need
 * platform-conditional code just to hold the value.
 */

#ifndef LH_OS_SYSTEM_ERROR_CODE_H
#define LH_OS_SYSTEM_ERROR_CODE_H

#include <lh/numeric/fixed/types.h>

/**
 * @def lh_os_system_error_code_ok
 * @brief Native sentinel for "no error" — `0` on both Windows (`ERROR_SUCCESS`)
 *        and POSIX (`errno == 0`).
 */
#define lh_os_system_error_code_ok 0U

/**
 * @typedef lh_os_system_error_code_t
 * @brief Raw native error code, as `GetLastError()` / `errno` reports it.
 */
typedef lh_u32_t lh_os_system_error_code_t;

#endif /* LH_OS_SYSTEM_ERROR_CODE_H */
