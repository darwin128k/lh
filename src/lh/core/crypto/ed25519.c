#include <lh/core/crypto/ed25519.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/crypto/ed25519/point.h>
#include <lh/core/crypto/sha.h>
#include <lh/core/crypto/wipe.h>
#include <lh/core/memory/std.h>

/* RFC 8032 Ed25519 over LH's bricks, in the order Monocypher 4 does it
   (CC0 / BSD-2-Clause, Loup Vaillant). */

lh_void
lh_crypto_ed25519_hash_reduce(lh_byte_t h[32], const lh_byte_t *a, lh_usize_t a_size,
                              const lh_byte_t *b, lh_usize_t b_size, const lh_byte_t *c,
                              lh_usize_t c_size)
{
    lh_crypto_sha_t sha;
    lh_crypto_sha_init(lh_addr_of(sha), lh_crypto_sha_kind_512);
    lh_crypto_sha_update(lh_addr_of(sha), a, a_size);
    lh_crypto_sha_update(lh_addr_of(sha), b, b_size);
    lh_crypto_sha_update(lh_addr_of(sha), c, c_size);
    lh_byte_t hash[LH_CRYPTO_SHA512_DIGEST_SIZE];
    LH_ATTRIBUTE_UNUSED(lh_crypto_sha_finish(lh_addr_of(sha), hash, sizeof(hash)));
    lh_crypto_ed25519_scalar_reduce(h, hash);
    lh_crypto_wipe(hash, sizeof(hash));
    lh_crypto_wipe(lh_addr_of(sha), sizeof(sha));
}

lh_void
lh_crypto_ed25519_key_pair(lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE],
                           lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE],
                           lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE])
{
    if (lh_null_eq(secret_key) || lh_null_eq(public_key) || lh_null_eq(seed))
    {
        return;
    }
    /* a[0..31] = the seed, then the trimmed scalar; the seed is wiped at once. */
    lh_byte_t a[LH_CRYPTO_SHA512_DIGEST_SIZE];
    lh_memory_std_move(a, seed, LH_CRYPTO_ED25519_SEED_SIZE);
    lh_crypto_wipe(seed, LH_CRYPTO_ED25519_SEED_SIZE);
    lh_memory_std_move(secret_key, a, LH_CRYPTO_ED25519_SEED_SIZE);
    LH_ATTRIBUTE_UNUSED(
        lh_crypto_sha_hash(lh_crypto_sha_kind_512, a, LH_CRYPTO_ED25519_SEED_SIZE, a, sizeof(a)));
    lh_crypto_ed25519_scalar_trim(a, a);
    lh_crypto_ed25519_point_scalarbase(public_key, a);
    /* The secret key carries its public half. */
    lh_memory_std_move(secret_key + LH_CRYPTO_ED25519_SEED_SIZE, public_key,
                       LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE);
    lh_crypto_wipe(a, sizeof(a));
}

lh_void
lh_crypto_ed25519_sign(lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE],
                       const lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE],
                       const lh_byte_t *message, lh_usize_t size)
{
    if (lh_null_eq(signature) || lh_null_eq(secret_key) || (lh_null_eq(message) && size > 0U))
    {
        return;
    }
    const lh_byte_t *public_key = secret_key + LH_CRYPTO_ED25519_SEED_SIZE;
    /* a = trimmed scalar, a + 32 = the nonce prefix. */
    lh_byte_t a[LH_CRYPTO_SHA512_DIGEST_SIZE];
    LH_ATTRIBUTE_UNUSED(lh_crypto_sha_hash(lh_crypto_sha_kind_512, secret_key,
                                           LH_CRYPTO_ED25519_SEED_SIZE, a, sizeof(a)));
    lh_crypto_ed25519_scalar_trim(a, a);
    /* r = SHA-512(prefix || M) mod L: the secret, deterministic nonce. */
    lh_byte_t r[LH_CRYPTO_ED25519_SCALAR_SIZE];
    lh_crypto_ed25519_hash_reduce(r, a + LH_CRYPTO_ED25519_SCALAR_SIZE,
                                  LH_CRYPTO_ED25519_SCALAR_SIZE, message, size, lh_null, 0U);
    /* R = [r]B; kept apart so the signature may overlap the inputs. */
    lh_byte_t big_r[LH_CRYPTO_ED25519_POINT_SIZE];
    lh_crypto_ed25519_point_scalarbase(big_r, r);
    /* h = SHA-512(R || A || M) mod L: public. */
    lh_byte_t h[LH_CRYPTO_ED25519_SCALAR_SIZE];
    lh_crypto_ed25519_hash_reduce(h, big_r, LH_CRYPTO_ED25519_POINT_SIZE, public_key,
                                  LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE, message, size);
    lh_memory_std_move(signature, big_r, LH_CRYPTO_ED25519_POINT_SIZE);
    /* S = h a + r mod L */
    lh_crypto_ed25519_scalar_mul_add(signature + LH_CRYPTO_ED25519_POINT_SIZE, h, a, r);
    lh_crypto_wipe(a, sizeof(a));
    lh_crypto_wipe(r, sizeof(r));
}

lh_bool_t
lh_crypto_ed25519_check(const lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE],
                        const lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE],
                        const lh_byte_t *message, lh_usize_t size)
{
    if (lh_null_eq(signature) || lh_null_eq(public_key) || (lh_null_eq(message) && size > 0U))
    {
        return lh_bool_false;
    }
    /* h = SHA-512(R || A || M) mod L */
    lh_byte_t h[LH_CRYPTO_ED25519_SCALAR_SIZE];
    lh_crypto_ed25519_hash_reduce(h, signature, LH_CRYPTO_ED25519_POINT_SIZE, public_key,
                                  LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE, message, size);
    return lh_crypto_ed25519_point_check_equation(signature, public_key, h);
}
