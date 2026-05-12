/**
 * @file static.h
 * @brief Compiler-portable internal-linkage function macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_STATIC, a single macro for declaring
 * translation-unit local functions.
 */

#ifndef LH_COMPILER_ATTRIBUTE_STATIC_H
#define LH_COMPILER_ATTRIBUTE_STATIC_H

/**
 * @def LH_COMPILER_ATTRIBUTE_STATIC
 * @brief Give the decorated function internal linkage.
 *
 * Expands to the C `static` storage-class specifier. Place it before the
 * function return type.
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_STATIC int helper(void) { return 0; }
 * @endcode
 */
#define LH_COMPILER_ATTRIBUTE_STATIC static

#endif // LH_COMPILER_ATTRIBUTE_STATIC_H
