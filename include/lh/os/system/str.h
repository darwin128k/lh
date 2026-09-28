/**
 * @file str.h
 * @brief Kernel: UTF-8 <-> OS text (::lh_os_str_t).
 *
 * The one place lh text becomes the kernel's and back. Windows converts
 * UTF-8 <-> UTF-16 (`MultiByteToWideChar` / `WideCharToMultiByte`); POSIX
 * copies, since its OS text already is UTF-8. The backend is picked by
 * CMake, not by `#if` at the call site.
 *
 * Invalid UTF-8 going in is an error, never silently replaced: a path that
 * does not round-trip must not name a different file. Coming out on
 * Windows, an unpaired UTF-16 surrogate (possible in NTFS names) becomes
 * U+FFFD — rejecting it needs `WC_ERR_INVALID_CHARS`, which XP lacks.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_STR_H
#define LH_OS_SYSTEM_STR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/str.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/str.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief NUL-terminated UTF-8 @p text as OS text, replacing @p out.
 *
 * @param out Initialized OS string; NUL-terminated afterwards.
 * @return ::lh_bool_false: out of memory (::lh_os_last_error) or invalid
 *         UTF-8 (::lh_os_system_last_error). @p out is then unchanged.
 */
lh_bool_t
lh_os_system_str_from_utf8(lh_os_str_t *out, lh_str_cptr text);

/**
 * @brief @p count code units of OS text @p text as UTF-8, replacing @p out.
 *
 * @param count Units to convert, not counting a NUL; `0` gives "".
 * @return ::lh_bool_false: out of memory (::lh_os_last_error) or the
 *         conversion failed (::lh_os_system_last_error). @p out is then
 *         unchanged.
 */
lh_bool_t
lh_os_system_str_to_utf8(lh_os_str_cptr text, lh_usize_t count, lh_str_t *out);

/**
 * @brief Initialize @p self as NUL-terminated UTF-8 @p text in OS text.
 *
 * @p self is initialized either way — empty on failure — so one
 * ::lh_os_str_deinit afterwards is always right:
 * @code{.c}
 * lh_os_str_t os_path;
 * const lh_bool_t ok = lh_os_system_str_init_by_utf8(&os_path, path);
 * // ... use lh_os_str_get_data(&os_path) only if ok ...
 * lh_os_str_deinit(&os_path);
 * @endcode
 *
 * @return As ::lh_os_system_str_from_utf8.
 */
lh_bool_t
lh_os_system_str_init_by_utf8(lh_os_str_t *self, lh_str_cptr text);

/**
 * @brief NUL-terminated OS text @p text as UTF-8, replacing @p out.
 *
 * ::lh_os_system_str_to_utf8 with the length counted up to the NUL.
 */
lh_bool_t
lh_os_system_str_ptr_to_utf8(lh_os_str_cptr text, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_STR_H */
