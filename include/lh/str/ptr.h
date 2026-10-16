/**
 * @file ptr.h
 * @brief String pointer type aliases.
 *
 * Public headers write read-only text as `const lh_str_ptr` so the const is
 * visible. ::lh_str_cptr is the same pointee type for `.c` implementations.
 */

#ifndef LH_STR_PTR_H
#define LH_STR_PTR_H

#include <lh/util/ptr.h>

/**
 * @def lh_str_ptr
 * @brief Pointer to ::lh_char_t — mutable string / text buffer view.
 *
 * Expands to `lh_char_t *` (plain `char *`).
 * For read-only text in headers use `const lh_str_ptr`.
 */
#define lh_str_ptr lh_ptr_of(lh_char_t)

/**
 * @def lh_str_cptr
 * @brief Pointer to `const` ::lh_char_t — read-only string / text view.
 *
 * Expands to `const lh_char_t *` (plain `const char *`).
 * For implementation files only; headers use `const lh_str_ptr` instead.
 */
#define lh_str_cptr lh_cptr_of(lh_char_t)

#endif /* LH_STR_PTR_H */
