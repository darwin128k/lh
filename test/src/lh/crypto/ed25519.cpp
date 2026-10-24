#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <string>

#include <lh/crypto/ed25519.h>
#include <lh/crypto/wipe.h>
#include <lh/str/format/bytes.h>

namespace
{

static std::string
hex(const lh_byte_t *bytes, lh_usize_t size)
{
    lh_char_t text[LH_CRYPTO_ED25519_SIGNATURE_SIZE * 2U];
    lh_usize_t n =
        lh_str_ptr_format_bytes_hex(const_cast<lh_byte_t *>(bytes), size, lh_bool_false, text, sizeof(text));
    return std::string(text, n);
}

static void
unhex(const char *text, lh_byte_t *out, lh_usize_t size)
{
    for (lh_usize_t i = 0U; i < size; ++i)
    {
        unsigned value = 0U;
        ASSERT_EQ(std::sscanf(text + i * 2U, "%2x", &value), 1);
        out[i] = static_cast<lh_byte_t>(value);
    }
}

/* RFC 8032, section 7.1: seed, public key, message, signature. */
struct vector
{
    const char *seed;
    const char *public_key;
    const char *message;
    const char *signature;
};

static const vector rfc8032[] = {
    {"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60",
     "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a", "",
     "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46b"
     "d25bf5f0595bbe24655141438e7a100b"},
    {"4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb",
     "3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c", "72",
     "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c"
     "387b2eaeb4302aeeb00d291612bb0c00"},
    {"c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7",
     "fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025", "af82",
     "6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc659"
     "4a7c15e9716ed28dc027beceea1ec40a"},
};

TEST(crypto_ed25519, rfc8032_vectors)
{
    for (const vector &v : rfc8032)
    {
        lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE];
        lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE];
        lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE];
        lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE];
        lh_byte_t message[8];
        lh_usize_t message_size = static_cast<lh_usize_t>(std::strlen(v.message) / 2U);

        unhex(v.seed, seed, sizeof(seed));
        unhex(v.message, message, message_size);
        lh_crypto_ed25519_key_pair(secret_key, public_key, seed);
        EXPECT_EQ(hex(public_key, sizeof(public_key)), v.public_key);

        lh_crypto_ed25519_sign(signature, secret_key, message_size > 0U ? message : nullptr, message_size);
        EXPECT_EQ(hex(signature, sizeof(signature)), v.signature);
        EXPECT_TRUE(lh_crypto_ed25519_check(signature, public_key, message_size > 0U ? message : nullptr,
                                            message_size));
    }
}

TEST(crypto_ed25519, key_pair_wipes_the_seed)
{
    lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE];
    lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE];
    lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE];
    lh_byte_t zero[LH_CRYPTO_ED25519_SEED_SIZE] = {};

    std::memset(seed, 0x5A, sizeof(seed));
    lh_crypto_ed25519_key_pair(secret_key, public_key, seed);
    EXPECT_EQ(std::memcmp(seed, zero, sizeof(seed)), 0);
}

TEST(crypto_ed25519, check_rejects_tampering)
{
    lh_byte_t seed[LH_CRYPTO_ED25519_SEED_SIZE];
    lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE];
    lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE];
    lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE];
    lh_byte_t message[] = {'l', 'h'};

    std::memset(seed, 0x11, sizeof(seed));
    lh_crypto_ed25519_key_pair(secret_key, public_key, seed);
    lh_crypto_ed25519_sign(signature, secret_key, message, sizeof(message));
    ASSERT_TRUE(lh_crypto_ed25519_check(signature, public_key, message, sizeof(message)));

    message[0] ^= 1U;
    EXPECT_FALSE(lh_crypto_ed25519_check(signature, public_key, message, sizeof(message)));
    message[0] ^= 1U;
    signature[0] ^= 1U;
    EXPECT_FALSE(lh_crypto_ed25519_check(signature, public_key, message, sizeof(message)));
    signature[0] ^= 1U;
    public_key[0] ^= 1U;
    EXPECT_FALSE(lh_crypto_ed25519_check(signature, public_key, message, sizeof(message)));
}

TEST(crypto_ed25519, null_arguments)
{
    lh_byte_t secret_key[LH_CRYPTO_ED25519_SECRET_KEY_SIZE] = {};
    lh_byte_t public_key[LH_CRYPTO_ED25519_PUBLIC_KEY_SIZE] = {};
    lh_byte_t signature[LH_CRYPTO_ED25519_SIGNATURE_SIZE];
    lh_byte_t untouched[LH_CRYPTO_ED25519_SIGNATURE_SIZE];

    std::memset(signature, 0xA5, sizeof(signature));
    std::memcpy(untouched, signature, sizeof(signature));
    lh_crypto_ed25519_key_pair(nullptr, public_key, nullptr);
    lh_crypto_ed25519_sign(signature, secret_key, nullptr, 1U);
    EXPECT_EQ(std::memcmp(signature, untouched, sizeof(signature)), 0);
    EXPECT_FALSE(lh_crypto_ed25519_check(nullptr, public_key, nullptr, 0U));
    EXPECT_FALSE(lh_crypto_ed25519_check(signature, public_key, nullptr, 1U));
}

TEST(crypto_wipe, zeroes_the_bytes)
{
    lh_byte_t secret[17];
    lh_byte_t zero[17] = {};

    std::memset(secret, 0xFF, sizeof(secret));
    lh_crypto_wipe(secret, sizeof(secret));
    EXPECT_EQ(std::memcmp(secret, zero, sizeof(secret)), 0);
    lh_crypto_wipe(nullptr, 4U);
}

} // namespace
