/**
 * @file os.h
 * @brief OS last-error slot (::lh_os_get_last_error), like `GetLastError`.
 *
 * One thread-local (or process-wide, if TLS is off) last failure: numeric
 * code + copied message. Encoding of the message is selected at compile time
 * by ::LH_LIBRARY_OPTION_OS_WERROR:
 *
 * - OFF — ::lh_error_t / ::lh_str_view_t, Windows `FormatMessageA`.
 * - ON  — ::lh_werror_t / ::lh_wstr_view_t, Windows `FormatMessageW`.
 *
 * Shared libraries, sockets, and the rest of `lh/os/` write this slot on
 * failure; the caller reads it after a `false` / null return.
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_H
#define LH_OS_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/error/code.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @typedef lh_os_error_t
 * @brief Last-error payload stored by `lh/os`.
 *
 * ::lh_werror_t when ::LH_LIBRARY_OPTION_OS_WERROR is on, otherwise ::lh_error_t.
 */

/**
 * @typedef lh_os_error_desc_t
 * @brief Description view of the last-error slot (wide or narrow, same option).
 */

/**
 * @def lh_os_error_desc_lit(s)
 * @brief Compile-time description view over a narrow string literal @p s.
 *
 * When WERROR is on, expands to ::lh_wstr_view_lit with an @c L prefix on @p s.
 * When off, expands to ::lh_str_view_lit(@p s). Pass a quoted literal, not a pointer.
 *
 * Example usage:
 * @code{.c}
 * lh_os_set_last_error(1, lh_os_error_desc_lit("path is null"));
 * @endcode
 */
#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/werror.h>
typedef lh_werror_t lh_os_error_t;
typedef lh_wstr_view_t lh_os_error_desc_t;
#    define lh_os_error_desc_lit(s) lh_wstr_view_lit(L##s)
#else
#    include <lh/error.h>
typedef lh_error_t lh_os_error_t;
typedef lh_str_view_t lh_os_error_desc_t;
#    define lh_os_error_desc_lit(s) lh_str_view_lit(s)
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Store @p code and a copy of @p desc as the last OS-layer error.
 *
 * @p desc may be empty (no message). Text is copied into the slot; the
 * returned view from ::lh_os_get_last_error_desc is valid until the next
 * ::lh_os_set_last_error / ::lh_os_set_last_error_cstr /
 * ::lh_os_capture_last_error.
 *
 * @param code Native or lh-side code stored in the slot.
 * @param desc View over the message in the slot encoding (narrow or wide).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_set_last_error(lh_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Store @p code and a copy of a narrow C string as the last error.
 *
 * When ::LH_LIBRARY_OPTION_OS_WERROR is on, @p text is converted into the
 * wide slot (Windows: ACP via `MultiByteToWideChar`; POSIX: `mbstowcs`).
 * When off, this is a copy of a ::lh_str_view_make(@p text) into the char slot.
 *
 * @param code Native or lh-side code stored in the slot.
 * @param text NUL-terminated narrow string, or ::lh_null (no description).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_set_last_error_cstr(lh_error_code_t code, lh_str_cptr text);

/**
 * @brief Snapshot the native thread last error into the slot.
 *
 * Windows: `GetLastError` + `FormatMessageA` or `FormatMessageW`
 * (see ::LH_LIBRARY_OPTION_OS_WERROR). POSIX: `errno` + `strerror`
 * (converted to wide when WERROR is on).
 * Call immediately after a failed OS API (not `dlopen` — that uses
 * `dlerror`; ::lh_os_shared_* does that via ::lh_os_set_last_error_cstr).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_capture_last_error(void);

/**
 * @brief Last error code (native OS code, or a small lh-side code).
 * @return Code from the most recent set / capture. Zero if nothing was stored.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_os_get_last_error_code(void);

/**
 * @brief Last error message as a view into the slot.
 *
 * Valid until the next set/capture. Empty view means no message.
 *
 * @return ::lh_os_error_desc_t over the internal buffer (does not own it).
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_get_last_error_desc(void);

/**
 * @brief Code + description as ::lh_os_error_t.
 *
 * The description field is a view into the slot, same lifetime as
 * ::lh_os_get_last_error_desc.
 *
 * @return Snapshot of the current last-error pair.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_t
lh_os_get_last_error(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_H */
