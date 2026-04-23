/**
 * @file ref.h
 * @brief Lightweight reference helper macro (::lh_ref).
 */

#ifndef LH_REF_H
#define LH_REF_H

/**
 * @def lh_ref(expr)
 * @brief Address-of wrapper: expands to @c &expr.
 * @param expr Lvalue (or other expression valid for unary @c &) whose address is required.
 */
#define lh_ref(expr) &expr

#endif // LH_REF_H
