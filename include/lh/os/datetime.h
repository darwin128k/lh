/**
 * @file datetime.h
 * @brief Local wall-clock calendar now, as ::lh_datetime_t.
 *
 * OS fills a value type. Formatting is ::lh_datetime_format — this header
 * does not write strings. Elapsed time (timeouts, flood) is ::lh_os_clock_ms,
 * not this.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_DATETIME_H
#define LH_OS_DATETIME_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/datetime.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/datetime.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Current local date and time of day into @p out.
 *
 * Windows: `GetLocalTime`. POSIX: `CLOCK_REALTIME` + `localtime_r`.
 * Leap seconds (`60`) are stored as ::LH_TIME_SECOND_MAX. No timezone field.
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 *
 * @param out Destination (not null). Untouched on failure.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_datetime_now(lh_datetime_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_DATETIME_H */
