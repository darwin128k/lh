#include <lh/crypto/ed25519.h>
#include <lh/null.h>

#include <monocypher-ed25519.h>
#include <monocypher.h>

lh_void
lh_crypto_ed25519_key_pair(lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE],
                           lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE],
                           lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE])
{
    if (lh_null_eq(secret_key) || lh_null_eq(public_key) || lh_null_eq(seed))
    {
        return;
    }
    crypto_ed25519_key_pair(secret_key, public_key, seed);
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
    crypto_ed25519_sign(signature, secret_key, message, (size_t)size);
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
    return crypto_ed25519_check(signature, public_key, message, (size_t)size) == 0 ? lh_bool_true
                                                                                     : lh_bool_false;
}
