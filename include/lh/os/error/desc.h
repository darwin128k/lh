/**
 * @file desc.h
 * @brief Description pointer for ::lh_os_error_t — str or wstr, by build option.
 *
 * ::LH_LIBRARY_OPTION_OS_WERROR picks which text representation OS-layer
 * errors carry, so the rest of `lh/os` never hardcodes wide vs. narrow:
 *
 *   ON  — ::lh_wstr_ptr (matches `FormatMessageW`, wide OS APIs).
 *   OFF — ::lh_str_ptr (matches `FormatMessageA` / `strerror_r`).
 *
 * Either way the description is one NUL-terminated pointer, so
 * ::lh_os_error_t stays binary-compatible with ::lh_error_t / ::lh_werror_t —
 * see `lh/os/error.h`.
 */

#ifndef LH_OS_ERROR_DESC_H
#define LH_OS_ERROR_DESC_H

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/util/wstr/ptr/empty.h>

/**
 * @def lh_os_error_desc_ptr
 * @brief Pointer to one description character. ::lh_wstr_ptr or
 *        ::lh_str_ptr, by ::LH_LIBRARY_OPTION_OS_WERROR.
 *
 * For read-only text in headers use `const lh_os_error_desc_ptr`.
 */
#    define lh_os_error_desc_ptr lh_wstr_ptr

/**
 * @def lh_os_error_desc_lit(s)
 * @brief Narrow string literal @p s as an ::lh_os_error_desc_ptr literal.
 */
#    define lh_os_error_desc_lit(s) lh_wstr_ptr_cat(s)

/**
 * @def lh_os_error_desc_is_empty(text)
 * @brief Test whether description @p text is null or empty.
 */
#    define lh_os_error_desc_is_empty(text) lh_wstr_ptr_is_empty(text)
#else
#    include <lh/util/str/ptr/empty.h>

/**
 * @def lh_os_error_desc_ptr
 * @brief Pointer to one description character. ::lh_wstr_ptr or
 *        ::lh_str_ptr, by ::LH_LIBRARY_OPTION_OS_WERROR.
 *
 * For read-only text in headers use `const lh_os_error_desc_ptr`.
 */
#    define lh_os_error_desc_ptr lh_str_ptr

/**
 * @def lh_os_error_desc_lit(s)
 * @brief Narrow string literal @p s as an ::lh_os_error_desc_ptr literal.
 */
#    define lh_os_error_desc_lit(s) s

/**
 * @def lh_os_error_desc_is_empty(text)
 * @brief Test whether description @p text is null or empty.
 */
#    define lh_os_error_desc_is_empty(text) lh_str_ptr_is_empty(text)
#endif

#endif /* LH_OS_ERROR_DESC_H */
