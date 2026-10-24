/**
 * @file ed25519.h
 * @brief Ed25519 signatures (RFC 8032): key pair, sign, check.
 *
 * Deterministic signing, cofactorless verification. Pure computation — no OS,
 * no allocator. The arithmetic is Monocypher's (vendored in `lib/monocypher`,
 * see `licenses/Monocypher.txt`), so a program linking lh gets the `crypto_*`
 * symbols from it and must not link a second Monocypher.
 */

#ifndef LH_CRYPTO_ED25519_H
#define LH_CRYPTO_ED25519_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/void.h>

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
