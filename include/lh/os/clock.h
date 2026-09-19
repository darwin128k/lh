/**
 * @file clock.h
 * @brief Monotonic millisecond clock for the current process.
 *
 * OS-backed elapsed time, not a calendar. Use this for timeouts and rate
 * limits. Local wall clock is ::lh_os_datetime_now (`lh/os/datetime.h`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_CLOCK_H
#define LH_OS_CLOCK_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/clock.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Milliseconds since an unspecified epoch, monotonically increasing.
 *
 * Windows: `GetTickCount64`. POSIX: `CLOCK_MONOTONIC`.
 *
 * @return Elapsed milliseconds.
 */
LH_ATTRIBUTE_SYMBOL
lh_u64_t
lh_os_clock_ms(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_CLOCK_H */
