/**
 * @file static.h
 * @brief Library alias: mark a function as translation-unit local.
 *
 * ::LH_ATTRIBUTE_STATIC is the `lh` naming layer; it expands to the same
 * tokens as ::LH_COMPILER_ATTRIBUTE_STATIC.
 */

#ifndef LH_ATTRIBUTE_STATIC_H
#define LH_ATTRIBUTE_STATIC_H

#include <lh/compiler/attribute/static.h>

/**
 * @def LH_ATTRIBUTE_STATIC
 * @brief Give the decorated function internal linkage.
 *
 * Place it before the function return type.
 *
 * @see LH_COMPILER_ATTRIBUTE_STATIC
 */
#define LH_ATTRIBUTE_STATIC LH_COMPILER_ATTRIBUTE_STATIC

#endif // LH_ATTRIBUTE_STATIC_H
