/**
 * @file timestamp.h
 * @brief The OS wall clock as ::lh_timestamp_t.
 *
 * ::lh_os_timestamp_now reads the system's real-time clock (UTC) and
 * converts it to ::lh_timestamp_t — the same "seconds since 1970-01-01
 * 00:00:00 UTC" model the rest of `lh` already builds on
 * (`lh/timestamp.h`), so callers get straight to ::lh_date_t /
 * ::lh_time_t / ::lh_datetime_t via the existing conversions, with no
 * separate OS-specific time type in between.
 *
 * Windows: `GetSystemTimeAsFileTime` (~15.6ms resolution, plenty for a
 * whole-second timestamp). POSIX: `time()`.
 *
 * Local wall-clock display (with a timezone/DST offset applied) is a
 * separate, later concern — this is UTC only.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_TIMESTAMP_H
#define LH_OS_TIMESTAMP_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/timestamp.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/timestamp.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the current UTC time as a scalar timestamp.
 *
 * On failure (POSIX `time()` returning `(time_t)-1` — essentially never on
 * a real system), the native error is snapshotted via
 * ::lh_os_system_error_capture (`lh/os/system/error/capture.h`, readable
 * through ::lh_os_system_last_error) and @p out is left untouched.
 * `GetSystemTimeAsFileTime` on Windows cannot fail, so this always
 * succeeds there.
 *
 * @param out Receives the current time on success; untouched on failure.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_timestamp_now(lh_timestamp_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TIMESTAMP_H */
