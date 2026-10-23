/**
 * @file no_sanitize_address.h
 * @brief Compiler-portable attribute that exempts a function from AddressSanitizer.
 *
 * Some fast paths read bytes the sanitizer counts as out of bounds while the
 * hardware does not: a word-wide string scan reads up to a word past the
 * terminator, but an aligned word never crosses a page, so it cannot fault.
 * Marking such a function keeps the one fast implementation under ASan too,
 * instead of a second, byte-wise one only the sanitizer build runs.
 *
 * Outside an ASan build the attribute changes nothing.
 */

#ifndef LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS_H
#define LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS
 * @brief Do not instrument the memory accesses of the function it marks.
 *
 * | Compiler    | Expansion                                |
 * |-------------|------------------------------------------|
 * | GCC / Clang | `__attribute__((no_sanitize_address))`   |
 * | MSVC        | `__declspec(no_sanitize_address)`        |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS
 * size_t word_strlen(const char *s);
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)

#else
#    error "Compiler does not support a no-sanitize-address attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_NO_SANITIZE_ADDRESS_H */
