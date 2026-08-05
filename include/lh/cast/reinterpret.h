/**
 * @file reinterpret.h
 * @brief Portable reinterpret-cast macro.
 *
 * Provides ::lh_cast_reinterpret — a single macro
 * that reinterprets the bit pattern of an expression as a different type, * adapting to the
 * compilation language.
 */

#ifndef LH_CAST_REINTERPRET_H
#define LH_CAST_REINTERPRET_H

#include <lh/compiler/cxx.h>

/**
 * @def lh_cast_reinterpret(T, expr)
 * @brief Reinterpret the bit pattern of @p expr as type @p T.
 *
 * Expands to the appropriate cast for the compilation language:
 *
 * | Language | Expansion                      |
 * |----------|--------------------------------|
 * | C++      | `reinterpret_cast<T>(expr)`    |
 * | C        | `((T)(expr))`                  |
 *
 * @param T     Target type.
 * @param expr  Expression to cast.
 *
 * Example usage:
 * @code{.c}
 * unsigned int *p = lh_cast_reinterpret(unsigned int *, some_ptr);
 * @endcode
 */
#ifdef LH_COMPILER_CXX
#    define lh_cast_reinterpret(T, expr) reinterpret_cast<T>(expr)
#else
#    define lh_cast_reinterpret(T, expr) ((T)(expr))
#endif /* LH_COMPILER_CXX */

#endif /* LH_CAST_REINTERPRET_H */
