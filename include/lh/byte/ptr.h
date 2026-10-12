/**
 * @file ptr.h
 * @brief Pointer aliases for a raw byte buffer (::lh_byte_t).
 *
 * Same shape as ::lh_str_ptr / ::lh_str_cptr: macros, not typedefs, so that
 * `const lh_byte_ptr` expands to `const lh_byte_t *` (const pointee). A
 * typedef named `lh_byte_ptr_t` would make `const lh_byte_ptr_t` mean
 * "const pointer" instead — the clang-tidy warning that pulled these out of
 * typedef land in the first place.
 *
 * For an explicit read-only buffer in a signature, prefer ::lh_byte_cptr
 * over `const lh_byte_ptr`: it names the const pointee, and
 * `lh_byte_cptr a, b` declares two pointers (a macro ending in `*` does not
 * distribute across a declarator list the way a typedef would).
 *
 * @see lh_byte_t
 */

#ifndef LH_BYTE_PTR_H
#define LH_BYTE_PTR_H

#include <lh/byte.h>
#include <lh/util/ptr.h>

/**
 * @def lh_byte_ptr
 * @brief Pointer to ::lh_byte_t — mutable raw byte buffer.
 *
 * Expands to `lh_byte_t *`.
 * For a read-only buffer use ::lh_byte_cptr.
 */
#define lh_byte_ptr lh_ptr_of(lh_byte_t)

/**
 * @def lh_byte_cptr
 * @brief Pointer to `const` ::lh_byte_t — read-only raw byte buffer.
 *
 * Expands to `const lh_byte_t *`.
 * Prefer over `const lh_byte_ptr` when the const pointee should be obvious in
 * the type name.
 */
#define lh_byte_cptr lh_cptr_of(lh_byte_t)

#endif /* LH_BYTE_PTR_H */
