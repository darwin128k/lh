/**
 * @file ptr.h
 * @brief Canonical untyped pointer alias (::lh_void_ptr) and pointer-limit macros.
 *
 * Provides:
 * - ::lh_void_ptr as the library alias for `void *`
 * - ::LH_VOID_PTR_T_MIN as the null pointer value for ::lh_void_ptr
 * - ::LH_VOID_PTR_T_MAX as the pointer value obtained by reinterpreting
 *   ::LH_UADDR_T_MAX (all-ones address bit pattern for the configured target)
 *
 * These macros are low-level pointer boundary helpers used by pointer utilities
 * and null abstractions across the library.
 */
#ifndef LH_VOID_PTR_H
#define LH_VOID_PTR_H

#include <lh/addr.h>
#include <lh/void.h>
#include <lh/cast/reinterpret.h>

/**
 * @def lh_void_ptr
 * @brief Alias for `void *`.
 */
#define lh_void_ptr void *

#ifndef LH_VOID_PTR_T_MIN
/**
 * @def LH_VOID_PTR_T_MIN
 * @brief Smallest ::lh_void_ptr value (null pointer).
 *
 * Expands to `lh_numeric_limit_umin(lh_void_ptr)`.
 */
#    define LH_VOID_PTR_T_MIN lh_numeric_limit_umin(lh_void_ptr)
#endif // LH_VOID_PTR_T_MIN

#ifndef LH_VOID_PTR_T_MAX
/**
 * @def LH_VOID_PTR_T_MAX
 * @brief ::lh_void_ptr obtained by reinterpreting ::LH_UADDR_T_MAX.
 *
 * Represents the high end of the address integer range for the current target.
 */
#    define LH_VOID_PTR_T_MAX lh_cast_reinterpret(lh_void_ptr, LH_UADDR_T_MAX)
#endif // LH_VOID_PTR_T_MAX

#endif // LH_VOID_PTR_H