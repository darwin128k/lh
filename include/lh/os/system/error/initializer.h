/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_system_error_t.
 */

#ifndef LH_OS_SYSTEM_ERROR_INITIALIZER_H
#define LH_OS_SYSTEM_ERROR_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/os/system/error/code.h>

/**
 * @def lh_os_system_error_initializer(code, ...)
 * @brief Produces a brace-enclosed initializer for ::lh_os_system_error_t.
 *
 * Expands to ::lh_initializer(@p code, __VA_ARGS__).
 * The first field is @c code; remaining tokens initialize
 * @c desc and any trailing members in struct order,
 * or you may use designated initializers in the variadic tail.
 *
 * @param code Native error code (::lh_os_system_error_code_t).
 * @param ...  Optional remainder of the initializer list
 *             (e.g. an ::lh_os_error_desc_t initializer or @c .desc = ...).
 *
 * @see lh_initializer
 * @see lh_os_system_error_t
 */
#define lh_os_system_error_initializer(code, ...) lh_initializer(code, __VA_ARGS__)

/**
 * @def lh_os_system_error_empty_initializer(...)
 * @brief Initializer for a cleared / success native error (::lh_os_system_error_code_ok).
 *
 * Expands to ::lh_os_system_error_initializer(::lh_os_system_error_code_ok, __VA_ARGS__).
 * With no arguments, @c desc is value-initialized (empty view).
 *
 * @param ... Optional tail passed to ::lh_os_system_error_initializer after the code.
 *
 * @see lh_os_system_error_initializer
 * @see lh_os_system_error_code_ok
 */
#define lh_os_system_error_empty_initializer(...)                                                  \
    lh_os_system_error_initializer(lh_os_system_error_code_ok, __VA_ARGS__)

#endif /* LH_OS_SYSTEM_ERROR_INITIALIZER_H */
