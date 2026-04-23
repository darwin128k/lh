/**
 * @file symbol.h
 * @brief Picks export, import, or nothing for public `lh` symbols.
 *
 * Shared libraries need the toolchain to know which side of the boundary
 * you are on: the TU that **implements** the API marks symbols for export;
 *
 * TUs that **only call** the API mark them as imported from that library.
 * Static linking has no such split, so no annotation is needed.
 *
 * The choice is made entirely by the preprocessor block below:
 * first ask whether this build is shared at all;
 *
 * if yes, ask whether we are compiling the library
 * (`LH_EXPORTS`) or only using its headers.
 *
 * The compiler-specific tokens (`dllexport` / `dllimport`, visibility, etc.) are
 * ::LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT and ::LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT.
 */

#ifndef LH_ATTRIBUTE_SYMBOL_H
#define LH_ATTRIBUTE_SYMBOL_H

#include <lh/compiler/attribute/symbol.h>
#include <lh/library/fallback.h>

/**
 * @def LH_ATTRIBUTE_SYMBOL
 * @brief Same structure as the `#if` / `#else` below (read from the outer `#if` inward).
 *
 * - Outer `#if`: `LH_LIBRARY_OPTION_SHARED_BUILD == LH_LIBRARY_OPTION_ON` (shared `lh`).
 *   - Inner `#if defined(LH_EXPORTS)`: building the library →
 *     ::LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT.
 *   - Inner `#else`: only consuming headers →
 *     ::LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT.
 * - Outer `#else`: static `lh` → macro is empty; symbols are ordinary external
 *   linkage inside your final link.
 *
 * @see LH_LIBRARY_OPTION_SHARED_BUILD
 * @see LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT
 * @see LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT
 */
#if (LH_LIBRARY_OPTION_SHARED_BUILD == LH_LIBRARY_OPTION_ON)
#    if defined(LH_EXPORTS)
#        define LH_ATTRIBUTE_SYMBOL LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT
#    else
#        define LH_ATTRIBUTE_SYMBOL LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT
#    endif
#else
#    define LH_ATTRIBUTE_SYMBOL
#endif

#endif // LH_ATTRIBUTE_SYMBOL_H
