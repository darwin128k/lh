/**
 * @file size.h
 * @brief Portable size and unsigned size type definitions.
 *
 * Provides ::lh_ssize_t (signed size) and ::lh_usize_t (unsigned size) with
 * platform-appropriate underlying types and corresponding limit macros
 * (::LH_SSIZE_T_MIN, ::LH_SSIZE_T_MAX, ::LH_USIZE_T_MIN, ::LH_USIZE_T_MAX,
 * ::LH_SIZE_T_SIZE, ::LH_SSIZE_T_SIZE, ::LH_USIZE_T_SIZE).
 *
 * Mapping:
 *   - Windows 64-bit: ::lh_sllong_t / ::lh_ullong_t
 *   - Windows 32-bit: ::lh_sint_t / ::lh_uint_t
 *   - Other platforms: ::lh_slong_t / ::lh_ulong_t
 */

#ifndef LH_SIZE_H
#define LH_SIZE_H

#include <lh/numeric/limits.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    if LH_COMPILER_ARCH == 64

#        ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (64-bit Windows).
 */
#            define LH_SIZE_T_SIZE LH_SLLONG_T_SIZE
#        endif

#        ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (64-bit Windows).
 */
#            define LH_SSIZE_T_MIN LH_SLLONG_T_MIN
#        endif

#        ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (64-bit Windows).
 */
#            define LH_SSIZE_T_MAX LH_SLLONG_T_MAX
#        endif

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (64-bit Windows).
 *
 * Alias for ::lh_sllong_t on 64-bit Windows.
 */
typedef lh_sllong_t lh_ssize_t;

#        ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (64-bit Windows).
 */
#            define LH_USIZE_T_MIN LH_ULLONG_T_MIN
#        endif

#        ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (64-bit Windows).
 */
#            define LH_USIZE_T_MAX LH_ULLONG_T_MAX
#        endif

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (64-bit Windows).
 *
 * Alias for ::lh_ullong_t on 64-bit Windows.
 */
typedef lh_ullong_t lh_usize_t;
#    else

#        ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (32-bit Windows).
 */
#            define LH_SIZE_T_SIZE LH_SINT_T_SIZE
#        endif

#        ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (32-bit Windows).
 */
#            define LH_SSIZE_T_MIN LH_SINT_T_MIN
#        endif

#        ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (32-bit Windows).
 */
#            define LH_SSIZE_T_MAX LH_SINT_T_MAX
#        endif

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (32-bit Windows).
 *
 * Alias for ::lh_sint_t on 32-bit Windows.
 */
typedef lh_sint_t lh_ssize_t;

#        ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (32-bit Windows).
 */
#            define LH_USIZE_T_MIN LH_UINT_T_MIN
#        endif

#        ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (32-bit Windows).
 */
#            define LH_USIZE_T_MAX LH_UINT_T_MAX
#        endif

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (32-bit Windows).
 *
 * Alias for ::lh_uint_t on 32-bit Windows.
 */
typedef lh_uint_t lh_usize_t;
#    endif
#else

#    ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (non-Windows).
 */
#        define LH_SIZE_T_SIZE LH_SLONG_T_SIZE
#    endif

#    ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (non-Windows).
 */
#        define LH_SSIZE_T_MIN LH_SLONG_T_MIN
#    endif

#    ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (non-Windows).
 */
#        define LH_SSIZE_T_MAX LH_SLONG_T_MAX
#    endif

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (non-Windows).
 *
 * Alias for ::lh_slong_t on non-Windows platforms (e.g. Linux, macOS).
 */
typedef lh_slong_t lh_ssize_t;

#    ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (non-Windows).
 */
#        define LH_USIZE_T_MIN LH_ULONG_T_MIN
#    endif

#    ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (non-Windows).
 */
#        define LH_USIZE_T_MAX LH_ULONG_T_MAX
#    endif

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (non-Windows).
 *
 * Alias for ::lh_ulong_t on non-Windows platforms (e.g. Linux, macOS).
 */
typedef lh_ulong_t lh_usize_t;
#endif

#ifndef LH_SSIZE_T_SIZE
/**
 * @def LH_SSIZE_T_SIZE
 * @brief Size of ::lh_ssize_t in bytes.
 *
 * Equal to ::LH_SIZE_T_SIZE; ::lh_ssize_t and ::lh_usize_t always have the same width.
 */
#    define LH_SSIZE_T_SIZE LH_SIZE_T_SIZE
#endif // LH_SSIZE_T_SIZE

#ifndef LH_USIZE_T_SIZE
/**
 * @def LH_USIZE_T_SIZE
 * @brief Size of ::lh_usize_t in bytes.
 *
 * Equal to ::LH_SIZE_T_SIZE; ::lh_ssize_t and ::lh_usize_t always have the same width.
 */
#    define LH_USIZE_T_SIZE LH_SIZE_T_SIZE
#endif // LH_USIZE_T_SIZE

#endif // LH_SIZE_H
