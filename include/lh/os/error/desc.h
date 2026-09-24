/**
 * @file desc.h
 * @brief Description type for ::lh_os_error_t — str or wstr, by build option.
 *
 * ::LH_LIBRARY_OPTION_OS_WERROR picks which text representation OS-layer
 * errors carry, so the rest of `lh/os` never hardcodes wide vs. narrow:
 *
 *   ON  — ::lh_wstr_view_t (matches `FormatMessageW`, wide OS APIs).
 *   OFF — ::lh_str_view_t (matches `FormatMessageA` / `strerror_r`).
 *
 * Both are layout-compatible with ::lh_memory_view_t, so ::lh_os_error_t
 * stays binary-compatible with ::lh_error_t / ::lh_werror_t either way —
 * see `lh/os/error.h`.
 */

#ifndef LH_OS_ERROR_DESC_H
#define LH_OS_ERROR_DESC_H

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/wstr/ptr.h>
#    include <lh/wstr/view.h>
#    include <lh/wstr/view/initializer.h>

/**
 * @typedef lh_os_error_desc_t
 * @brief Description type selected by ::LH_LIBRARY_OPTION_OS_WERROR.
 */
typedef lh_wstr_view_t lh_os_error_desc_t;

/**
 * @def lh_os_error_desc_ptr
 * @brief Mutable pointer to one description character. ::lh_wstr_ptr or
 *        ::lh_str_ptr, matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_ptr lh_wstr_ptr

/**
 * @def lh_os_error_desc_lit(s)
 * @brief Literal-text view constructor matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_lit(s) lh_wstr_view_lit(s)
#else
#    include <lh/str/ptr.h>
#    include <lh/str/view.h>
#    include <lh/str/view/initializer.h>

/**
 * @typedef lh_os_error_desc_t
 * @brief Description type selected by ::LH_LIBRARY_OPTION_OS_WERROR.
 */
typedef lh_str_view_t lh_os_error_desc_t;

/**
 * @def lh_os_error_desc_ptr
 * @brief Mutable pointer to one description character. ::lh_wstr_ptr or
 *        ::lh_str_ptr, matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_ptr lh_str_ptr

/**
 * @def lh_os_error_desc_lit(s)
 * @brief Literal-text view constructor matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_lit(s) lh_str_view_lit(s)
#endif

#endif /* LH_OS_ERROR_DESC_H */
