/**
 * @file c.h
 * @brief C linkage macros for mixed C/C++ headers.
 *
 * Provides ::LH_COMPILER_EXTERN_C, ::LH_COMPILER_EXTERN_C_BEGIN,
 * and ::LH_COMPILER_EXTERN_C_END — macros that suppress C++ name
 * mangling so that C headers can be included safely from C++ translation units.
 *
 * @note All three expand to nothing in pure C compilation.
 * @note All three can be overridden by defining them before including this header.
 */

#ifndef LH_COMPILER_EXTERN_C_H
#define LH_COMPILER_EXTERN_C_H

#include <lh/compiler/cxx.h>
#include <lh/compiler/extern.h>

#ifndef LH_COMPILER_EXTERN_C
/**
 * @def LH_COMPILER_EXTERN_C
 * @brief Apply C linkage to a single declaration.
 *
 * | Language | Expansion          |
 * |----------|--------------------|
 * | C++      | `extern "C"`       |
 * | C        | *(empty)*          |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_EXTERN_C void foo(void);
 * @endcode
 */
#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C LH_COMPILER_EXTERN "C"
#    else
#        define LH_COMPILER_EXTERN_C
#    endif
#endif // LH_COMPILER_EXTERN_C

#ifndef LH_COMPILER_EXTERN_C_BEGIN
/**
 * @def LH_COMPILER_EXTERN_C_BEGIN
 * @brief Open a C-linkage block.
 *
 * Place before a group of declarations to apply C linkage to all of them.
 * Must be paired with ::LH_COMPILER_EXTERN_C_END.
 *
 * | Language | Expansion          |
 * |----------|--------------------|
 * | C++      | `extern "C" {`     |
 * | C        | *(empty)*          |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_EXTERN_C_BEGIN
 * void foo(void);
 * void bar(int x);
 * LH_COMPILER_EXTERN_C_END
 * @endcode
 */
#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C_BEGIN                                                         \
            LH_COMPILER_EXTERN_C                                                                   \
            {
#    else
#        define LH_COMPILER_EXTERN_C_BEGIN
#    endif
#endif // LH_COMPILER_EXTERN_C_BEGIN

#ifndef LH_COMPILER_EXTERN_C_END
/**
 * @def LH_COMPILER_EXTERN_C_END
 * @brief Close a C-linkage block opened by ::LH_COMPILER_EXTERN_C_BEGIN.
 *
 * | Language | Expansion |
 * |----------|-----------|
 * | C++      | `}`       |
 * | C        | *(empty)* |
 */
#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C_END }
#    else
#        define LH_COMPILER_EXTERN_C_END
#    endif
#endif // LH_COMPILER_EXTERN_C_END

#endif // LH_COMPILER_EXTERN_C_H
