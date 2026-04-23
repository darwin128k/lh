/**
 * @file wchar.h
 * @brief Portable wide-character type alias and limit macros.
 *
 * Wraps the standard @c <wchar.h> and exposes ::lh_wchar_t together
 * with ::LH_WCHAR_T_MIN, ::LH_WCHAR_T_MAX, and ::LH_WCHAR_T_SIZE.
 *
 * The limit and size macros default to the platform @c WCHAR_MIN / @c WCHAR_MAX
 * and @c sizeof(::lh_wchar_t) but may be overridden by defining them
 * before including this header.
 */

#ifndef LH_WCHAR_H
#define LH_WCHAR_H

#include <lh/compiler/cxx.h>

#ifdef LH_COMPILER_CXX
#    include <cwchar>
#else
#    include <wchar.h>
#endif

/**
 * @def LH_WCHAR_T_MIN
 * @brief Minimum value representable by ::lh_wchar_t.
 *
 * Defaults to @c WCHAR_MIN.
 * Define before including this header to override.
 */
#ifndef LH_WCHAR_T_MIN
#    ifdef WCHAR_MIN
#        define LH_WCHAR_T_MIN WCHAR_MIN
#    else
#        error "LH_WCHAR_T_MIN: WCHAR_MIN is not defined"
#    endif
#endif // LH_WCHAR_T_MIN

/**
 * @def LH_WCHAR_T_MAX
 * @brief Maximum value representable by ::lh_wchar_t.
 *
 * Defaults to @c WCHAR_MAX.
 * Define before including this header to override.
 */
#ifndef LH_WCHAR_T_MAX
#    ifdef WCHAR_MAX
#        define LH_WCHAR_T_MAX WCHAR_MAX
#    else
#        error "LH_WCHAR_T_MAX: WCHAR_MAX is not defined"
#    endif
#endif // LH_WCHAR_T_MAX

/**
 * @def LH_WCHAR_T_SIZE
 * @brief Size of ::lh_wchar_t in bytes (same as @c sizeof(::lh_wchar_t)).
 *
 * Defaults to @c sizeof(::lh_wchar_t).
 * Define before including this header to override
 * (for example if ::lh_wchar_t is redefined to a different underlying type).
 */
#ifndef LH_WCHAR_T_SIZE
#    define LH_WCHAR_T_SIZE sizeof(lh_wchar_t)
#endif // LH_WCHAR_T_SIZE

/**
 * @typedef lh_wchar_t
 * @brief Portable alias for @c wchar_t.
 *
 * Use this type throughout the library instead of @c wchar_t directly,
 * keeping the underlying type replaceable in one place if needed.
 */
#define lh_wchar_t wchar_t

#endif // LH_WCHAR_H