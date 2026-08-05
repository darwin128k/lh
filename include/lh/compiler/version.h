/**
 * @file version.h
 * @brief Macro definitions for compiler version detection.
 *
 * This header provides macros to detect compiler versions based on type (Clang, GCC, MSVC).
 * The macros extract major, minor and patch version numbers,
 * enabling version-based conditional compilation.
 *
 * Macros:
 * - `LH_COMPILER_VERSION_MAJOR`: Compiler major version
 * - `LH_COMPILER_VERSION_MINOR`: Compiler minor version
 * - `LH_COMPILER_VERSION_PATCH`: Compiler patch version
 *
 */

#ifndef LH_COMPILER_VERSION_H
#define LH_COMPILER_VERSION_H

#include <lh/compiler/type.h>

#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_CLANG)
/** @brief Major version number for Clang compiler */
#    define LH_COMPILER_VERSION_MAJOR __clang_major__
/** @brief Minor version number for Clang compiler */
#    define LH_COMPILER_VERSION_MINOR __clang_minor__
/** @brief Patch version number for Clang compiler */
#    define LH_COMPILER_VERSION_PATCH __clang_patchlevel__

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_GCC)
/** @brief Major version number for GCC compiler */
#    define LH_COMPILER_VERSION_MAJOR __GNUC__
/** @brief Minor version number for GCC compiler */
#    define LH_COMPILER_VERSION_MINOR __GNUC_MINOR__
/** @brief Patch version number for GCC compiler */
#    define LH_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
/** @brief Major version number for MSVC (derived from _MSC_VER) */
#    define LH_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
/** @brief Minor version number for MSVC (derived from _MSC_VER) */
#    define LH_COMPILER_VERSION_MINOR (_MSC_VER % 100)
/** @brief Patch version number for MSVC
 *  @details VS2005+ uses 9-digit _MSC_FULL_VER, VS6 and older use 8-digit */
#    if _MSC_VER < 1400
#        define LH_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
#    else
#        define LH_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#    endif

#else
#    error "Compiler version detection is not supported on this compiler"
#endif

#endif /* LH_COMPILER_VERSION_H */