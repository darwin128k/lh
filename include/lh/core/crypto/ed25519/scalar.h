/**
 * @file scalar.h
 * @brief Ed25519 scalars: numbers modulo the group order
 *        L = 2^252 + 27742317777372353535851937790883648493.
 *
 * A scalar is 32 little-endian bytes; the big-number steps below work on it as
 * 32-bit words, lowest first (lh_bit_unpack_le32_words). Signing reduces a
 * 64-byte hash modulo L (::lh_crypto_ed25519_scalar_reduce) and computes
 * `r + h * a` (::lh_crypto_ed25519_scalar_mul_add); a secret key becomes a
 * scalar by trimming (::lh_crypto_ed25519_scalar_trim).
 *
 * Constant time: no branch and no index depends on a scalar's value, apart from
 * ::lh_crypto_ed25519_scalar_get_bit's index, which the caller chooses.
 * Temporaries are wiped. Ported from Monocypher 4 (CC0 / BSD-2-Clause, Loup
 * Vaillant): the same Barrett reduction and the same final conditional
 * subtraction.
 */

#ifndef LH_CRYPTO_ED25519_SCALAR_H
#define LH_CRYPTO_ED25519_SCALAR_H

#include <lh/core/bool.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/compiler/extern/c.h>

/**
 * @def LH_CRYPTO_ED25519_SCALAR_SIZE
 * @brief Bytes of a scalar (32, little-endian).
 */
#define LH_CRYPTO_ED25519_SCALAR_SIZE 32U

/**
 * @def LH_CRYPTO_ED25519_SCALAR_WORDS
 * @brief 32-bit words of a scalar (8).
 */
#define LH_CRYPTO_ED25519_SCALAR_WORDS 8U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The group order L as ::LH_CRYPTO_ED25519_SCALAR_WORDS words, lowest first.
 * @return A constant array.
 */
const lh_u32_t *
lh_crypto_ed25519_scalar_get_l(void);

/**
 * @brief Trim 32 bytes into a scalar for multiplication: clear the three lowest
 *        bits and the top bit, set bit 254 (RFC 8032 5.1.5).
 * @param out Destination of 32 bytes; may be @p in.
 * @param in  32 bytes.
 */
lh_void
lh_crypto_ed25519_scalar_trim(lh_byte_t out[LH_CRYPTO_ED25519_SCALAR_SIZE],
                              const lh_byte_t in[LH_CRYPTO_ED25519_SCALAR_SIZE]);

/**
 * @brief Bit @p i of the scalar @p s; bit -1 is 0 (sliding windows read one below).
 * @param s Scalar of 32 bytes.
 * @param i -1 .. 255.
 * @return 0 or 1.
 */
lh_s32_t
lh_crypto_ed25519_scalar_get_bit(const lh_byte_t s[LH_CRYPTO_ED25519_SCALAR_SIZE], lh_s32_t i);

/**
 * @brief @p p += @p a * @p b, 256 x 256 bits into 512.
 * @param p Sixteen words, lowest first. On entry the lower eight are the addend
 *          and the upper eight are ignored (written before they are read).
 * @param a Eight words.
 * @param b Eight words.
 */
lh_void
lh_crypto_ed25519_scalar_multiply(lh_u32_t p[2U * LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t a[LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t b[LH_CRYPTO_ED25519_SCALAR_WORDS]);

/**
 * @brief Whether the 256-bit number @p x is L or more.
 * @param x Eight words, lowest first.
 * @return ::lh_bool_true when x >= L.
 */
lh_bool_t
lh_crypto_ed25519_scalar_is_above_l(const lh_u32_t x[LH_CRYPTO_ED25519_SCALAR_WORDS]);

/**
 * @brief @p r = @p x - L when x >= L, else x: the final reduction of a number
 *        below 2L. Constant time.
 * @param r Destination of eight words; may be @p x.
 * @param x Eight words below 2L.
 */
lh_void
lh_crypto_ed25519_scalar_remove_l(lh_u32_t r[LH_CRYPTO_ED25519_SCALAR_WORDS],
                                  const lh_u32_t x[LH_CRYPTO_ED25519_SCALAR_WORDS]);

/**
 * @brief Reduce a 512-bit number modulo L (Barrett) into a 32-byte scalar.
 * @param reduced Destination of 32 bytes.
 * @param x       Sixteen words, lowest first.
 */
lh_void
lh_crypto_ed25519_scalar_mod_l(lh_byte_t reduced[LH_CRYPTO_ED25519_SCALAR_SIZE],
                               const lh_u32_t x[2U * LH_CRYPTO_ED25519_SCALAR_WORDS]);

/**
 * @brief Reduce 64 little-endian bytes (a SHA-512 hash) modulo L.
 * @param reduced  Destination of 32 bytes.
 * @param expanded 64 bytes.
 */
lh_void
lh_crypto_ed25519_scalar_reduce(lh_byte_t reduced[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                const lh_byte_t expanded[2U * LH_CRYPTO_ED25519_SCALAR_SIZE]);

/**
 * @brief @p r = (@p a * @p b + @p c) mod L: the S of a signature.
 * @param r Destination of 32 bytes; may be any input.
 * @param a 32 bytes (any 256-bit number).
 * @param b 32 bytes (any 256-bit number).
 * @param c 32 bytes (any 256-bit number).
 */
lh_void
lh_crypto_ed25519_scalar_mul_add(lh_byte_t r[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t a[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t b[LH_CRYPTO_ED25519_SCALAR_SIZE],
                                 const lh_byte_t c[LH_CRYPTO_ED25519_SCALAR_SIZE]);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_ED25519_SCALAR_H */
