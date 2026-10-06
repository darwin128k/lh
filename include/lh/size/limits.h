/**
 * @file limits.h
 * @brief Min/max/size macros for ::lh_ssize_t / ::lh_usize_t.
 *
 * Platform mapping matches <lh/size.h>. Kept out of that header so the
 * typedef file stays free of <lh/numeric/limits.h>.
 *
 * Provides ::LH_SSIZE_T_MIN / ::LH_SSIZE_T_MAX / ::LH_SSIZE_T_SIZE,
 * ::LH_USIZE_T_MIN / ::LH_USIZE_T_MAX / ::LH_USIZE_T_SIZE, and
 * ::LH_SIZE_T_SIZE (common width of both).
 */

#ifndef LH_SIZE_LIMITS_H
#define LH_SIZE_LIMITS_H

#include <lh/compiler/arch.h>
#include <lh/compiler/os.h>
#include <lh/numeric/limits.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

#        ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (64-bit Windows).
 */
#            define LH_SIZE_T_SIZE LH_SLLONG_T_SIZE
#        endif /* LH_SIZE_T_SIZE */

#        ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (64-bit Windows).
 */
#            define LH_SSIZE_T_MIN LH_SLLONG_T_MIN
#        endif /* LH_SSIZE_T_MIN */

#        ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (64-bit Windows).
 */
#            define LH_SSIZE_T_MAX LH_SLLONG_T_MAX
#        endif /* LH_SSIZE_T_MAX */

#        ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (64-bit Windows).
 */
#            define LH_USIZE_T_MIN LH_ULLONG_T_MIN
#        endif /* LH_USIZE_T_MIN */

#        ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (64-bit Windows).
 */
#            define LH_USIZE_T_MAX LH_ULLONG_T_MAX
#        endif /* LH_USIZE_T_MAX */

#    else

#        ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (32-bit Windows).
 */
#            define LH_SIZE_T_SIZE LH_SINT_T_SIZE
#        endif /* LH_SIZE_T_SIZE */

#        ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (32-bit Windows).
 */
#            define LH_SSIZE_T_MIN LH_SINT_T_MIN
#        endif /* LH_SSIZE_T_MIN */

#        ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (32-bit Windows).
 */
#            define LH_SSIZE_T_MAX LH_SINT_T_MAX
#        endif /* LH_SSIZE_T_MAX */

#        ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (32-bit Windows).
 */
#            define LH_USIZE_T_MIN LH_UINT_T_MIN
#        endif /* LH_USIZE_T_MIN */

#        ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (32-bit Windows).
 */
#            define LH_USIZE_T_MAX LH_UINT_T_MAX
#        endif /* LH_USIZE_T_MAX */

#    endif
#else

#    ifndef LH_SIZE_T_SIZE
/**
 * @def LH_SIZE_T_SIZE
 * @brief Size in bytes of ::lh_ssize_t and ::lh_usize_t (non-Windows).
 */
#        define LH_SIZE_T_SIZE LH_SLONG_T_SIZE
#    endif /* LH_SIZE_T_SIZE */

#    ifndef LH_SSIZE_T_MIN
/**
 * @def LH_SSIZE_T_MIN
 * @brief Minimum value of ::lh_ssize_t (non-Windows).
 */
#        define LH_SSIZE_T_MIN LH_SLONG_T_MIN
#    endif /* LH_SSIZE_T_MIN */

#    ifndef LH_SSIZE_T_MAX
/**
 * @def LH_SSIZE_T_MAX
 * @brief Maximum value of ::lh_ssize_t (non-Windows).
 */
#        define LH_SSIZE_T_MAX LH_SLONG_T_MAX
#    endif /* LH_SSIZE_T_MAX */

#    ifndef LH_USIZE_T_MIN
/**
 * @def LH_USIZE_T_MIN
 * @brief Minimum value of ::lh_usize_t (non-Windows).
 */
#        define LH_USIZE_T_MIN LH_ULONG_T_MIN
#    endif /* LH_USIZE_T_MIN */

#    ifndef LH_USIZE_T_MAX
/**
 * @def LH_USIZE_T_MAX
 * @brief Maximum value of ::lh_usize_t (non-Windows).
 */
#        define LH_USIZE_T_MAX LH_ULONG_T_MAX
#    endif /* LH_USIZE_T_MAX */

#endif

#ifndef LH_SSIZE_T_SIZE
/**
 * @def LH_SSIZE_T_SIZE
 * @brief Size of ::lh_ssize_t in bytes.
 *
 * Equal to ::LH_SIZE_T_SIZE; ::lh_ssize_t and ::lh_usize_t always have the same width.
 */
#    define LH_SSIZE_T_SIZE LH_SIZE_T_SIZE
#endif /* LH_SSIZE_T_SIZE */

#ifndef LH_USIZE_T_SIZE
/**
 * @def LH_USIZE_T_SIZE
 * @brief Size of ::lh_usize_t in bytes.
 *
 * Equal to ::LH_SIZE_T_SIZE; ::lh_ssize_t and ::lh_usize_t always have the same width.
 */
#    define LH_USIZE_T_SIZE LH_SIZE_T_SIZE
#endif /* LH_USIZE_T_SIZE */

#endif /* LH_SIZE_LIMITS_H */
