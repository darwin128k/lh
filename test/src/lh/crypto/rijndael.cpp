#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/crypto/rijndael.h>
#include <lh/null.h>
#include <lh/str/format/bytes.h>
#include <lh/util/addr.h>

namespace
{

static std::string
bytes_hex(const lh_uchar_t *data, lh_usize_t n)
{
    lh_char_t hex[128];
    lh_usize_t written = lh_str_ptr_format_bytes_hex(const_cast<lh_uchar_t *>(data), n,
                                                     lh_bool_false, hex, sizeof(hex));
    return std::string(hex, written);
}

TEST(crypto_rijndael, aes128_ecb_fips197)
{
    const lh_uchar_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    const lh_uchar_t pt[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                               0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    lh_uchar_t ct[16] = {};
    lh_uchar_t back[16] = {};
    lh_crypto_rijndael_t aes;

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), const_cast<lh_uchar_t *>(key), 16, lh_null,
                                        0, 16, lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), const_cast<lh_uchar_t *>(pt), ct));
    EXPECT_EQ(bytes_hex(ct, 16), "69c4e0d86a7b0430d8cdb78070b4c55a");

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), const_cast<lh_uchar_t *>(key), 16, lh_null,
                                        0, 16, lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_decrypt_block(lh_addr_of(aes), ct, back));
    EXPECT_EQ(std::memcmp(back, pt, 16), 0);
}

TEST(crypto_rijndael, aes256_ecb_round_trip)
{
    lh_uchar_t key[32];
    lh_uchar_t pt[16];
    lh_uchar_t ct[16];
    lh_uchar_t back[16];
    lh_crypto_rijndael_t aes;
    lh_usize_t i;

    for (i = 0; i < 32U; ++i)
    {
        key[i] = static_cast<lh_uchar_t>(i);
    }
    for (i = 0; i < 16U; ++i)
    {
        pt[i] = static_cast<lh_uchar_t>(0xA0U + i);
    }

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), key, 32, lh_null, 0, 16,
                                        lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), pt, ct));
    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), key, 32, lh_null, 0, 16,
                                        lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_decrypt_block(lh_addr_of(aes), ct, back));
    EXPECT_EQ(std::memcmp(back, pt, 16), 0);
}

TEST(crypto_rijndael, rijndael_32_byte_block_round_trip)
{
    lh_uchar_t key[32];
    lh_uchar_t pt[32];
    lh_uchar_t ct[32];
    lh_uchar_t back[32];
    lh_crypto_rijndael_t cipher;
    lh_usize_t i;

    for (i = 0; i < 32U; ++i)
    {
        key[i] = static_cast<lh_uchar_t>(i + 1U);
        pt[i] = static_cast<lh_uchar_t>(0x40U + i);
    }

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(cipher), key, 32, lh_null, 0, 32,
                                        lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_encrypt_block(lh_addr_of(cipher), pt, ct));
    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(cipher), key, 32, lh_null, 0, 32,
                                        lh_crypto_rijndael_mode_ecb));
    ASSERT_TRUE(lh_crypto_rijndael_decrypt_block(lh_addr_of(cipher), ct, back));
    EXPECT_EQ(std::memcmp(back, pt, 32), 0);
    EXPECT_NE(std::memcmp(ct, pt, 32), 0);
}

TEST(crypto_rijndael, cbc_round_trip)
{
    const lh_uchar_t key[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                                0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    const lh_uchar_t iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    const lh_uchar_t pt[16] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
                               0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};
    lh_uchar_t ct[16];
    lh_uchar_t back[16];
    lh_crypto_rijndael_t aes;

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), const_cast<lh_uchar_t *>(key), 16,
                                        const_cast<lh_uchar_t *>(iv), 16, 16,
                                        lh_crypto_rijndael_mode_cbc));
    ASSERT_TRUE(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), const_cast<lh_uchar_t *>(pt), ct));

    ASSERT_TRUE(lh_crypto_rijndael_init(lh_addr_of(aes), const_cast<lh_uchar_t *>(key), 16,
                                        const_cast<lh_uchar_t *>(iv), 16, 16,
                                        lh_crypto_rijndael_mode_cbc));
    ASSERT_TRUE(lh_crypto_rijndael_decrypt_block(lh_addr_of(aes), ct, back));
    EXPECT_EQ(std::memcmp(back, pt, 16), 0);
}

TEST(crypto_rijndael, rejects_bad_sizes)
{
    lh_uchar_t key[16] = {};
    lh_crypto_rijndael_t aes;
    EXPECT_FALSE(lh_crypto_rijndael_init(lh_addr_of(aes), key, 15, lh_null, 0, 16,
                                         lh_crypto_rijndael_mode_ecb));
}

} // namespace
