/**
 * @file const.h
 * @brief Const-qualification cast macro for C and C++.
 *
 * Provides ::lh_cast_const — a unified wrapper around
 * C++ `const_cast` (when available) and a C-style cast in C mode.
 */

#ifndef LH_CAST_CONST_H
#define LH_CAST_CONST_H

/**
 * @def lh_cast_const(T, expr)
 * @brief Cast @p expr to type @p T using const-qualification cast semantics.
 *
 * In C++, expands to `const_cast<T>(expr)`.
 * In C, expands to `((T)(expr))`.
 *
 * Typical use: removing or adjusting cv-qualification of pointer/reference types
 * when the caller guarantees that mutation is valid for the underlying object.
 *
 * @param T    Target type.
 * @param expr Source expression.
 */
#if defined(__cplusplus)
#    define lh_cast_const(T, expr) const_cast<T>(expr)
#else
#    define lh_cast_const(T, expr) ((T)(expr))
#endif

#endif /* LH_CAST_CONST_H */
