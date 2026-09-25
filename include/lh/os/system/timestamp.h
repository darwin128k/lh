/**
 * @file timestamp.h
 * @brief Kernel wall clock: the system's UTC time as ::lh_timestamp_t.
 *
 * The only place that talks to `GetSystemTimeAsFileTime` (Windows) or
 * `time()` (POSIX). The backend is picked by CMake, not by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_TIMESTAMP_H
#define LH_OS_SYSTEM_TIMESTAMP_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/timestamp.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/timestamp.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the current UTC time into @p out.
 * @return ::lh_bool_true on success; ::lh_bool_false if the native clock
 *         failed (@p out untouched, reason in ::lh_os_system_last_error).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_TIMESTAMP_H */
