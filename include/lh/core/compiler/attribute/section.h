/**
 * @file section.h
 * @brief Compiler-portable attribute that places an object in a named section.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_SECTION — the object it decorates goes to the
 * linker section named by a string literal, where the linker script decides
 * which memory it lands in (flash, RAM, a fast RAM bank).
 */

#ifndef LH_COMPILER_ATTRIBUTE_SECTION_H
#define LH_COMPILER_ATTRIBUTE_SECTION_H

#include <lh/core/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_SECTION(name)
 * @brief Put the decorated object in section @p name (a string literal).
 *
 * | Compiler    | Expansion                         |
 * |-------------|-----------------------------------|
 * | GCC / Clang | `__attribute__((section(name)))`  |
 * | other       | nothing: the object stays where the compiler puts it |
 *
 * Every object in one section must agree on being constant or not, or GCC
 * reports a section type conflict.
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_SECTION(name) __attribute__((section(name)))
#else
#    define LH_COMPILER_ATTRIBUTE_SECTION(name)
#endif

#endif /* LH_COMPILER_ATTRIBUTE_SECTION_H */
