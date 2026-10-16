/**
 * @file ptr.h
 * @brief Pointer aliases for a raw byte buffer (::lh_byte_t).
 *
 * Same shape as ::lh_str_ptr: macros, not typedefs, so that `const lh_byte_ptr`
 * expands to `const lh_byte_t *` (const pointee). A typedef named
 * `lh_byte_ptr_t` would make `const lh_byte_ptr_t` mean "const pointer"
 * instead — the clang-tidy warning that pulled these out of typedef land in
 * the first place.
 *
 * Public headers write read-only buffers as `const lh_byte_ptr`. ::lh_byte_cptr
 * is for `.c` implementations.
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
 * For a read-only buffer in headers use `const lh_byte_ptr`.
 */
#define lh_byte_ptr lh_ptr_of(lh_byte_t)

/**
 * @def lh_byte_cptr
 * @brief Pointer to `const` ::lh_byte_t — read-only raw byte buffer.
 *
 * Expands to `const lh_byte_t *`.
 * For implementation files only; headers use `const lh_byte_ptr` instead.
 */
#define lh_byte_cptr lh_cptr_of(lh_byte_t)

#endif /* LH_BYTE_PTR_H */
