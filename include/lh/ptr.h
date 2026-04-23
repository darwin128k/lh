/**
 * @file ptr.h
 * @brief Compatibility alias layer for untyped pointer macros.
 *
 * This header maps legacy/general pointer names to the canonical
 * ::lh_void_ptr definitions from `lh/void/ptr.h`.
 */
#ifndef LH_PTR_H
#define LH_PTR_H

#include <lh/void/ptr.h>

/**
 * @def lh_ptr
 * @brief Untyped mutable pointer alias.
 *
 * Expands to ::lh_void_ptr.
 */
#define lh_ptr lh_void_ptr

#ifndef LH_PTR_T_MIN
/**
 * @def LH_PTR_T_MIN
 * @brief Smallest ::lh_ptr value (null pointer).
 *
 * Alias of ::LH_VOID_PTR_T_MIN.
 */
#    define LH_PTR_T_MIN LH_VOID_PTR_T_MIN
#endif // LH_PTR_T_MIN

#ifndef LH_PTR_T_MAX
/**
 * @def LH_PTR_T_MAX
 * @brief Largest representable ::lh_ptr bit-pattern value.
 *
 * Alias of ::LH_VOID_PTR_T_MAX.
 */
#    define LH_PTR_T_MAX LH_VOID_PTR_T_MAX
#endif // LH_PTR_T_MAX

#endif // LH_PTR_H
