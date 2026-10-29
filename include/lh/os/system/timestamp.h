/**
 * @file timestamp.h
 * @brief Kernel wall clock: the system's UTC time as ::lh_timestamp_t, and the local
 *        zone's offset from it.
 *
 * The only place that talks to `GetSystemTimeAsFileTime` and
 * `SystemTimeToTzSpecificLocalTime` (Windows) or `time()` and `localtime_r`
 * (POSIX). The backend is picked by CMake, not by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_TIMESTAMP_H
#define LH_OS_SYSTEM_TIMESTAMP_H

#include <lh/config.h>
#include <lh/timestamp.h>

#if !LH_LIBRARY_OPTION_OS
#    error                                                                                         \
        "lh/os/system/timestamp.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the current UTC time into @p out.
 * @return ::lh_bool_true on success; ::lh_bool_false if the native clock
 *         failed (@p out untouched, reason in ::lh_os_system_last_error).
 */
lh_bool_t
lh_os_system_timestamp_try_get_now(lh_timestamp_t *out);

/**
 * @brief The local zone's offset from UTC at @p utc, daylight saving included.
 *
 * @param utc The moment, in UTC.
 * @param out Receives local minus UTC, in seconds (`+10800` for UTC+3).
 * @return ::lh_bool_true on success; ::lh_bool_false if the moment is out of the
 *         native range or the system could not tell (@p out untouched, reason in
 *         ::lh_os_system_last_error).
 */
lh_bool_t
lh_os_system_timestamp_try_get_zone_offset(lh_timestamp_t utc, lh_s32_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_TIMESTAMP_H */
