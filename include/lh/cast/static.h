/**
 * @file static.h
 * @brief Portable static-cast macro.
 *
 * Provides ::lh_cast_static — a single macro
 * that performs a well-defined type conversion,
 * adapting to the compilation language.
 */

#ifndef LH_CAST_STATIC_H
#define LH_CAST_STATIC_H

#include <lh/compiler/cxx.h>

/**
 * @def lh_cast_static(T, expr)
 * @brief Convert @p expr to type @p T with a static cast.
 *
 * Expands to the appropriate cast for the compilation language:
 *
 * | Language | Expansion                  |
 * |----------|----------------------------|
 * | C++      | `static_cast<T>(expr)`     |
 * | C        | `((T)(expr))`              |
 *
 * @param T     Target type.
 * @param expr  Expression to cast.
 *
 * Example usage:
 * @code{.c}
 * int n = lh_cast_static(int, 3.14);
 * @endcode
 */
#ifdef LH_COMPILER_CXX
#    define lh_cast_static(T, expr) static_cast<T>(expr)
#else
#    define lh_cast_static(T, expr) ((T)(expr))
#endif // LH_COMPILER_CXX

#endif // LH_CAST_STATIC_H
