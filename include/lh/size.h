/**
 * @file size.h
 * @brief Portable size and unsigned size type definitions.
 *
 * Provides ::lh_ssize_t (signed size) and ::lh_usize_t (unsigned size) with
 * platform-appropriate underlying types.
 *
 * Mapping:
 *   - Windows 64-bit: ::lh_sllong_t / ::lh_ullong_t
 *   - Windows 32-bit: ::lh_sint_t / ::lh_uint_t
 *   - Other platforms: ::lh_slong_t / ::lh_ulong_t
 *
 * Limits (::LH_SSIZE_T_MIN, ::LH_SSIZE_T_MAX, ::LH_USIZE_T_MIN,
 * ::LH_USIZE_T_MAX, ::LH_SIZE_T_SIZE, ::LH_SSIZE_T_SIZE, ::LH_USIZE_T_SIZE)
 * live in <lh/size/limits.h>.
 */

#ifndef LH_SIZE_H
#define LH_SIZE_H

#include <lh/compiler/arch.h>
#include <lh/compiler/os.h>
#include <lh/numeric/types.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (64-bit Windows).
 *
 * Alias for ::lh_sllong_t on 64-bit Windows.
 */
typedef lh_sllong_t lh_ssize_t;

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (64-bit Windows).
 *
 * Alias for ::lh_ullong_t on 64-bit Windows.
 */
typedef lh_ullong_t lh_usize_t;

#    else

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (32-bit Windows).
 *
 * Alias for ::lh_sint_t on 32-bit Windows.
 */
typedef lh_sint_t lh_ssize_t;

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (32-bit Windows).
 *
 * Alias for ::lh_uint_t on 32-bit Windows.
 */
typedef lh_uint_t lh_usize_t;

#    endif
#else

/**
 * @typedef lh_ssize_t
 * @brief Signed size type (non-Windows).
 *
 * Alias for ::lh_slong_t on non-Windows platforms (e.g. Linux, macOS).
 */
typedef lh_slong_t lh_ssize_t;

/**
 * @typedef lh_usize_t
 * @brief Unsigned size type (non-Windows).
 *
 * Alias for ::lh_ulong_t on non-Windows platforms (e.g. Linux, macOS).
 */
typedef lh_ulong_t lh_usize_t;

#endif

#endif /* LH_SIZE_H */
