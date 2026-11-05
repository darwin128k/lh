/**
 * @file ed25519.h
 * @brief Ed25519 signatures (RFC 8032): key pair, sign, check.
 *
 * Deterministic signing; cofactored verification ([8]([S]B - [h]A - R) = 0,
 * which agrees with batch verification), refusing S >= L. Pure computation — no
 * OS, no allocator. Composed of LH's bricks: SHA-512 (lh/core/crypto/sha.h),
 * scalars modulo L (lh/core/crypto/ed25519/scalar.h) and curve points
 * (lh/core/crypto/ed25519/point.h), ported from Monocypher 4 (CC0 /
 * BSD-2-Clause, Loup Vaillant; see `licenses/Monocypher.txt`) and checked
 * against it and against RFC 8032.
 */

#ifndef LH_CRYPTO_ED25519_H
#define LH_CRYPTO_ED25519_H

#include <lh/core/bool.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/size.h>

/**
 * @def LH_CRYPTO_ED25519_SEED_SIZE
 * @brief Random seed a key pair is derived from (32).
 */
#define LH_CRYPTO_ED25519_SEED_SIZE 32U

/**
 * @def LH_CRYPTO_ED25519_SECRET_KEY_SIZE
 * @brief Expanded secret key: the seed followed by the public key (64).
 */
#define LH_CRYPTO_ED25519_SECRET_KEY_SIZE 64U

/**
 * @def LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE
 * @brief Public key length in bytes (32).
 */
#define LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE 32U

/**
 * @def LH_CRYPTO_ED25519_SIGNATURE_SIZE
 * @brief Signature length in bytes (64).
 */
#define LH_CRYPTO_ED25519_SIGNATURE_SIZE 64U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief SHA-512 of @p a, @p b and @p c one after another, reduced modulo L: the
 *        nonce r and the challenge h of RFC 8032 5.1.6.
 *
 * A part may be null when its size is 0.
 *
 * @param h      Destination of 32 bytes.
 * @param a      First part.
 * @param a_size Bytes of @p a.
 * @param b      Second part.
 * @param b_size Bytes of @p b.
 * @param c      Third part.
 * @param c_size Bytes of @p c.
 */
lh_void
lh_crypto_ed25519_hash_reduce(lh_byte_t h[32], const lh_byte_t *a, lh_usize_t a_size,
                              const lh_byte_t *b, lh_usize_t b_size, const lh_byte_t *c,
                              lh_usize_t c_size);

/**
 * @brief Derive a key pair from @p seed.
 *
 * The seed is wiped afterwards — keep a copy first if it has to be stored.
 * Null arguments leave everything untouched.
 */
lh_void
lh_crypto_ed25519_key_pair(lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE],
                           lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE],
                           lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE]);

/**
 * @brief Sign @p size bytes of @p message with @p secret_key.
 *
 * @p message may be null only when @p size is 0. Null arguments leave
 * @p signature untouched.
 */
lh_void
lh_crypto_ed25519_sign(lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE],
                       const lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE],
                       const lh_byte_t *message, lh_usize_t size);

/**
 * @brief Verify @p signature over @p size bytes of @p message.
 *
 * @return ::lh_bool_true when it verifies against @p public_key;
 *         ::lh_bool_false otherwise, or for null arguments.
 */
lh_bool_t
lh_crypto_ed25519_check(const lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE],
                        const lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE],
                        const lh_byte_t *message, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_ED25519_H */
