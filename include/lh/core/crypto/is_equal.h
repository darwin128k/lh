/**
 * @file is_equal.h
 * @brief Compare secrets in constant time.
 */

#ifndef LH_CRYPTO_IS_EQUAL_H
#define LH_CRYPTO_IS_EQUAL_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>
#include <lh/core/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether the @p size bytes at @p a and @p b are the same, in a time
 *        that does not depend on their contents.
 *
 * Every byte is read, whatever the first difference: comparing a MAC, a key or
 * an encoded field element must not tell an attacker how many bytes matched.
 * Use ::lh_memory_is_equal for anything that is not secret; it is faster.
 *
 * @param a    First buffer of @p size bytes; must not be null when @p size > 0.
 * @param b    Second buffer of @p size bytes; must not be null when @p size > 0.
 * @param size Bytes to compare (0: equal).
 * @return ::lh_bool_true when every byte matches.
 */
lh_bool_t
lh_crypto_is_equal(const lh_ptr a, const lh_ptr b, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_IS_EQUAL_H */
