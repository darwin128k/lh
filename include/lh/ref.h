/**
 * @file ref.h
 * @brief Address-of shorthand macro.
 */

#ifndef LH_REF_H
#define LH_REF_H

/**
 * @def lh_ref(expr)
 * @brief Produce a pointer to @p expr.
 *
 * Shorthand for the address-of operator @c &.
 *
 * @param expr An lvalue expression whose address is taken.
 * @return Pointer to @p expr.
 *
 * Example usage:
 * @code{.c}
 * lh_error_t error;
 * lh_error_init(lh_ref(error), code, desc);
 * @endcode
 */
#define lh_ref(expr) &expr

#endif // LH_REF_H