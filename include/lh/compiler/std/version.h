/**
 * @file version.h
 * @brief Macro definitions for C language standard versions.
 *
 * This header provides macros representing different C language standard
 * versions (C99, C11, C17, C23) for compatibility control and feature
 * detection based on compiler-supported standards.
 *
 * Macros:
 * - `LH_COMPILER_STD_VERSION_C99`: C99 standard (ISO/IEC 9899:1999)
 * - `LH_COMPILER_STD_VERSION_C11`: C11 standard (ISO/IEC 9899:2011)
 * - `LH_COMPILER_STD_VERSION_C17`: C17/C18 standard (ISO/IEC 9899:2018)
 * - `LH_COMPILER_STD_VERSION_C23`: C23 standard (ISO/IEC 9899:2023)
 * - `LH_COMPILER_STD_VERSION_C`: Maps to compiler's __STDC_VERSION__ macro
 */

#ifndef LH_COMPILER_STD_VERSION_H
#define LH_COMPILER_STD_VERSION_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_STD_VERSION_C99
 * @brief Represents ISO/IEC 9899:1999 standard (C99)
 */
#define LH_COMPILER_STD_VERSION_C99 199901L

/**
 * @def LH_COMPILER_STD_VERSION_C11
 * @brief Represents ISO/IEC 9899:2011 standard (C11)
 */
#define LH_COMPILER_STD_VERSION_C11 201112L

/**
 * @def LH_COMPILER_STD_VERSION_C17
 * @brief Represents ISO/IEC 9899:2018 standard (C17/C18)
 */
#define LH_COMPILER_STD_VERSION_C17 201710L

/**
 * @def LH_COMPILER_STD_VERSION_C23
 * @brief Represents ISO/IEC 9899:2023 standard (C23)
 * @details Version value varies by compiler implementation:
 *          - Clang: 202311L
 *          - Other compilers: 202000L (provisional value)
 */
#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_CLANG)
#    define LH_COMPILER_STD_VERSION_C23 202311L
#else
#    define LH_COMPILER_STD_VERSION_C23 202000L
#endif

/**
 * @def LH_COMPILER_STD_VERSION_C
 * @brief Maps to compiler's __STDC_VERSION__ macro
 * @details Reflects the actual C standard version supported by the compiler.
 * @note `__STDC_VERSION__` is not defined in C89/C90; this macro must not be
 *       used when compiling without a C94 or later standard.
 */
#define LH_COMPILER_STD_VERSION_C __STDC_VERSION__

#endif /* LH_COMPILER_STD_VERSION_H */
