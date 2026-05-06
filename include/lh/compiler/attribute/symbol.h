/**
 * @file symbol.h
 * @brief Compiler-portable symbol visibility attribute macros.
 *
 * Provides three macros for controlling symbol visibility in shared
 * libraries (`.so` / `.dll`): ::LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT,
 * ::LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT, and ::LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN.
 */

#ifndef LH_COMPILER_ATTRIBUTE_SYMBOL_H
#define LH_COMPILER_ATTRIBUTE_SYMBOL_H

#include <lh/compiler/type.h>
#include <lh/compiler/os/subsys.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT
 * @brief Mark a symbol as exported from the shared library.
 *
 * Makes the symbol visible to consumers of the library.
 * Use on public API declarations when building a shared library.
 *
 * | Compiler    | Expansion                                  |
 * |-------------|--------------------------------------------|
 * | GCC / Clang | `__attribute__((visibility("default")))`   |
 * | MSVC        | `__declspec(dllexport)`                    |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT void lh_get_version(void);
 * @endcode
 */

/**
 * @def LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT
 * @brief Mark a symbol as imported from an external shared library.
 *
 * Declares that the symbol is defined in another shared library.
 * Use on public API declarations when consuming a prebuilt shared library.
 *
 * | Compiler    | Expansion                                  |
 * |-------------|--------------------------------------------|
 * | GCC / Clang | `__attribute__((visibility("default")))`   |
 * | MSVC        | `__declspec(dllimport)`                    |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT void lh_get_version(void);
 * @endcode
 */

/**
 * @def LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN
 * @brief Hide a symbol from external linkage.
 *
 * Restricts the symbol to the current shared library even if it has
 * external linkage.
 * On MSVC all symbols are hidden by default unless explicitly exported,
 * so this macro expands to nothing.
 *
 * | Compiler    | Expansion                                  |
 * |-------------|--------------------------------------------|
 * | GCC / Clang | `__attribute__((visibility("hidden")))`    |
 * | MSVC        | *(empty)*                                  |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN void internal_helper(void);
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS &&                                                \
        LH_COMPILER_OS_SUBSYS != LH_COMPILER_OS_SUBSYS_CYGWIN
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT __declspec(dllexport)
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT __declspec(dllimport)
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN
#    else
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT __attribute__((visibility("default")))
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT __attribute__((visibility("default")))
#        define LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN __attribute__((visibility("hidden")))
#    endif
#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT __declspec(dllexport)
#    define LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT __declspec(dllimport)
#    define LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN

#else
#    error "Symbol visibility attributes not supported for this compiler"
#endif

#endif // LH_COMPILER_ATTRIBUTE_SYMBOL_H
