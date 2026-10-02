/**
 * @file result.h
 * @brief What an OS-layer call that returns a count gives back on failure.
 *
 * Reads, writes, sends, receives and directory reads return an
 * ::lh_ssize_t: a count `>= 0`, or ::LH_OS_RESULT_INVALID when the call
 * failed (the reason is in ::lh_os_last_error or ::lh_os_system_last_error).
 * Same spirit as the handle sentinels (::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID,
 * …): a named "no result", not a bare `-1`.
 *
 * Callers test failure with a negative check (::lh_math_is_negative), not
 * equality: these functions also feed `lh/io` readers and writers, whose
 * contract is "negative on failure".
 */

#ifndef LH_OS_RESULT_H
#define LH_OS_RESULT_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @def LH_OS_RESULT_INVALID
 * @brief Failed count-returning OS-layer call (`-1` as ::lh_ssize_t).
 */
#define LH_OS_RESULT_INVALID (lh_cast_static(lh_ssize_t, -1))

#endif /* LH_OS_RESULT_H */
