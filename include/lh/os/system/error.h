/**
 * @file error.h
 * @brief Capture of a native OS last-error into the lh last-error slot.
 *
 * Owns the description buffer. ::lh_os_system_error_capture snapshots
 * `GetLastError` / `errno` plus the system message, then
 * ::lh_os_set_last_error. ::lh_os_system_error_set_cstr does the same
 * for a narrow C string (`dlerror`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_ERROR_H
#define LH_OS_SYSTEM_ERROR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/error/code.h>
#include <lh/os/error/desc.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/error.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write the OS message for @p code into a narrow C string @p dest.
 *
 * Windows: `FormatMessageA`. POSIX: `strerror_r`. Independent of
 * ::LH_LIBRARY_OPTION_OS_WERROR. @p dest_size is the buffer capacity,
 * including the terminator. On failure @p dest is cleared.
 * Does not touch the last-error slot.
 *
 * @return @p dest.
 *
 * @see lh_os_system_error_format
 */
LH_ATTRIBUTE_SYMBOL
lh_str_ptr
lh_os_system_error_format_cstr(lh_error_code_t code, lh_str_ptr dest, lh_usize_t dest_size);

/**
 * @brief Write the OS message for @p code into @p dest.
 *
 * @p dest is ::lh_os_error_desc_ptr: narrow via ::lh_os_system_error_format_cstr,
 * or wide (`FormatMessageW` / convert). @p dest_size is the buffer capacity
 * in code units, including the terminator. Does not touch the last-error slot.
 *
 * @return @p dest.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_ptr
lh_os_system_error_format(lh_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size);

/**
 * @brief Snapshot the native thread last error into ::lh_os_set_last_error.
 *
 * Windows: `GetLastError`. POSIX: `errno`. Text via ::lh_os_system_error_format
 * into the system buffer, then ::lh_os_set_last_error.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_error_capture(void);

/**
 * @brief Copy a narrow C string into the system buffer and store it
 *        as the last error with @p code.
 *
 * Uses ::lh_os_error_desc_from_cstr on the system buffer, then
 * ::lh_os_set_last_error. @p text may be ::lh_null (no description).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_error_set_cstr(lh_error_code_t code, lh_str_cptr text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_ERROR_H */
