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

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/werror.h>
typedef lh_werror_t lh_os_error_t;
typedef lh_werror_desc_t lh_os_error_desc_t;
#    define lh_os_error_desc_lit(s) lh_wstr_view_lit(L##s)
#else
#    include <lh/error.h>
typedef lh_error_t lh_os_error_t;
typedef lh_error_desc_t lh_os_error_desc_t;
#    define lh_os_error_desc_lit(s) lh_str_view_lit(s)
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Store @p code and a copy of @p desc as the last OS-layer error.
 *
 * @p desc may be empty (no message). The copy is valid until the next
 * ::lh_os_set_last_error / ::lh_os_set_last_error_cstr /
 * ::lh_os_capture_last_error.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_set_last_error(lh_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Store @p code and a copy of a narrow C string as the last error.
 *
 * When ::LH_LIBRARY_OPTION_OS_WERROR is on, @p text is converted into the
 * wide slot (Windows: ACP via `MultiByteToWideChar`; POSIX: `mbstowcs`).
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
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_os_get_last_error_code(void);

/**
 * @brief Last error message as a view into the slot. Valid until the next set/capture.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_get_last_error_desc(void);

/**
 * @brief Code + description as ::lh_os_error_t.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_t
lh_os_get_last_error(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_H */
