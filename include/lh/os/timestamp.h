/**
 * @file timestamp.h
 * @brief The OS wall clock as ::lh_timestamp_t, in UTC and in the local zone.
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
 * A timestamp is always UTC. A *local* timestamp is the local wall clock read as
 * if it were UTC — the zone's offset added — so ::lh_timestamp_to_datetime and the
 * other conversions give the local date and time of day, and a caller formats
 * them without knowing the zone or the OS. The offset is the system's, for that
 * very moment: daylight saving changes it through the year.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_TIMESTAMP_H
#define LH_OS_TIMESTAMP_H

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
lh_os_timestamp_try_get_now(lh_timestamp_t *out);

/**
 * @brief The local zone's offset from UTC at @p utc, daylight saving included.
 *
 * @param utc The moment, in UTC.
 * @param out Receives local minus UTC, in seconds (`+10800` for UTC+3); untouched on
 *            failure.
 * @return ::lh_bool_true on success, ::lh_bool_false when the system cannot tell.
 */
lh_bool_t
lh_os_timestamp_try_get_zone_offset(lh_timestamp_t utc, lh_s32_t *out);

/**
 * @brief The local timestamp of @p utc: the local wall clock as if it were UTC.
 *
 * @param utc The moment, in UTC.
 * @param out Receives @p utc plus the zone's offset at it; untouched on failure.
 * @return ::lh_bool_true on success, ::lh_bool_false when the offset is unknown.
 */
lh_bool_t
lh_os_timestamp_try_get_local(lh_timestamp_t utc, lh_timestamp_t *out);

/**
 * @brief The UTC moment of the local timestamp @p local, the inverse of
 *        ::lh_os_timestamp_try_get_local.
 *
 * A wall-clock time the spring change skips is moved forward by it (02:30 becomes
 * 03:30); one the autumn change repeats is taken the second time, in standard
 * time.
 *
 * @param local The local wall clock, read as if it were UTC.
 * @param out   Receives the UTC moment; untouched on failure.
 * @return ::lh_bool_true on success, ::lh_bool_false when the offset is unknown.
 */
lh_bool_t
lh_os_timestamp_try_get_utc(lh_timestamp_t local, lh_timestamp_t *out);

/**
 * @brief The current local timestamp (::lh_os_timestamp_try_get_now, then
 *        ::lh_os_timestamp_try_get_local).
 *
 * @param out Receives it; untouched on failure.
 * @return ::lh_bool_true on success.
 */
lh_bool_t
lh_os_timestamp_try_get_local_now(lh_timestamp_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TIMESTAMP_H */
