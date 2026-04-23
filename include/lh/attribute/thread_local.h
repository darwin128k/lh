/**
 * @file thread_local.h
 * @brief Public thread-local storage attribute macro.
 *
 * Provides ::LH_ATTRIBUTE_THREAD_LOCAL — a library-level alias
 * for ::LH_COMPILER_ATTRIBUTE_THREAD_LOCAL whose expansion
 * is controlled by ::LH_LIBRARY_OPTION_THREAD_LOCAL.
 *
 * @note Prefer this macro in user code over the compiler-specific variant.
 */

#ifndef LH_ATTRIBUTE_THREAD_LOCAL_H
#define LH_ATTRIBUTE_THREAD_LOCAL_H

#include <lh/compiler/attribute/thread_local.h>
#include <lh/library/fallback.h>

/**
 * @def LH_ATTRIBUTE_THREAD_LOCAL
 * @brief Declare a variable with thread-local storage duration.
 *
 * Each thread gets its own independent copy of the decorated variable.
 * The expansion depends on ::LH_LIBRARY_OPTION_THREAD_LOCAL:
 *
 * | `LH_LIBRARY_OPTION_THREAD_LOCAL` | Expansion                                    |
 * |----------------------------------|----------------------------------------------|
 * | `LH_LIBRARY_OPTION_ON`           | ::LH_COMPILER_ATTRIBUTE_THREAD_LOCAL         |
 * | `LH_LIBRARY_OPTION_OFF`          | *(empty — thread-local storage disabled)*    |
 *
 * Example usage:
 * @code{.c}
 * LH_ATTRIBUTE_THREAD_LOCAL int error_code;
 * @endcode
 */
#if (LH_LIBRARY_OPTION_THREAD_LOCAL == LH_LIBRARY_OPTION_ON)
#    define LH_ATTRIBUTE_THREAD_LOCAL LH_COMPILER_ATTRIBUTE_THREAD_LOCAL
#elif (LH_LIBRARY_OPTION_THREAD_LOCAL == LH_LIBRARY_OPTION_OFF)
#    define LH_ATTRIBUTE_THREAD_LOCAL
#endif

#endif // LH_ATTRIBUTE_THREAD_LOCAL_H