/**
 * @file ptr.h
 * @brief String pointer type aliases.
 *
 * Provides type aliases for mutable and const string pointers.
 *
 * @see lh_str_cptr
 */

#ifndef LH_STR_PTR_H
#define LH_STR_PTR_H

#include <lh/char.h>
#include <lh/util/ptr.h>

/**
 * @def lh_str_ptr
 * @brief Pointer to ::lh_char_t — mutable string / text buffer view.
 *
 * Expands to `lh_char_t *` (plain `char *`).
 * For read-only text use ::lh_str_cptr.
 */
#define lh_str_ptr lh_ptr_of(lh_char_t)

/**
 * @def lh_str_cptr
 * @brief Pointer to `const` ::lh_char_t — read-only string / text view.
 *
 * Expands to `const lh_char_t *` (plain `const char *`).
 * Prefer over `const lh_str_ptr`
 * for an explicit const-pointee type in signatures and struct fields.
 */
#define lh_str_cptr lh_cptr_of(lh_char_t)

#endif /* LH_STR_PTR_H */