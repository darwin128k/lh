/**
 * @file ref.h
 * @brief Conditional execution guard for nullable pointers.
 */

#ifndef LH_OPTIONAL_REF_H
#define LH_OPTIONAL_REF_H

#include <lh/null.h>

/**
 * @def lh_optional_ref(ptr)
 * @brief Execute the following statement or block only if @p ptr is not null.
 *
 * Expands to an `if` condition that guards execution against a null pointer.
 * Intended to be used as a statement prefix, similar to a null-check idiom.
 *
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * lh_optional_ref(out_major) *out_major = self->major;
 * lh_optional_ref(out_minor) { *out_minor = self->minor; }
 * @endcode
 */
#define lh_optional_ref(ptr) if (lh_null_ne(ptr))

#endif /* LH_OPTIONAL_REF_H */
