#include <gtest/gtest.h>

#include <string>

#include <lh/crypto/sha.h>
#include <lh/str/format/bytes.h>

namespace
{

static std::string
digest_hex(lh_crypto_sha_kind_t kind, const char *text)
{
    lh_byte_t digest[LH_CRYPTO_SHA_DIGEST_SIZE_MAX];
    lh_char_t hex[LH_CRYPTO_SHA_DIGEST_SIZE_MAX * 2U];
    lh_usize_t n = lh_crypto_sha_hash(kind, const_cast<char *>(text),
                                      static_cast<lh_usize_t>(std::string(text).size()), digest,
                                      sizeof(digest));
    lh_usize_t hex_n = lh_str_ptr_format_bytes_hex(digest, n, lh_bool_false, hex, sizeof(hex));
    return std::string(hex, hex_n);
}

TEST(crypto_sha, digest_size_matches_kind)
{
    EXPECT_EQ(lh_crypto_sha_digest_size(lh_crypto_sha_kind_1), 20U);
    EXPECT_EQ(lh_crypto_sha_digest_size(lh_crypto_sha_kind_224), 28U);
    EXPECT_EQ(lh_crypto_sha_digest_size(lh_crypto_sha_kind_256), 32U);
    EXPECT_EQ(lh_crypto_sha_digest_size(lh_crypto_sha_kind_384), 48U);
    EXPECT_EQ(lh_crypto_sha_digest_size(lh_crypto_sha_kind_512), 64U);
}

TEST(crypto_sha, sha1_abc)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_1, "abc"), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

TEST(crypto_sha, sha256_abc)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_256, "abc"),
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(crypto_sha, sha256_empty)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_256, ""),
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(crypto_sha, sha224_abc)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_224, "abc"),
              "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7");
}

TEST(crypto_sha, sha384_abc)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_384, "abc"),
              "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"
              "8086072ba1e7cc2358baeca134c825a7");
}

TEST(crypto_sha, sha512_abc)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_512, "abc"),
              "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
              "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST(crypto_sha, incremental_matches_oneshot)
{
    const char *text = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    lh_byte_t a[32];
    lh_byte_t b[32];
    lh_crypto_sha_t hasher;

    ASSERT_EQ(lh_crypto_sha_hash(lh_crypto_sha_kind_256, const_cast<char *>(text), 56, a, sizeof(a)),
              32U);

    lh_crypto_sha_init(lh_addr_of(hasher), lh_crypto_sha_kind_256);
    lh_crypto_sha_update(lh_addr_of(hasher), const_cast<char *>(text), 10);
    lh_crypto_sha_update(lh_addr_of(hasher), const_cast<char *>(text) + 10, 46);
    ASSERT_EQ(lh_crypto_sha_finish(lh_addr_of(hasher), b, sizeof(b)), 32U);
    EXPECT_EQ(std::string(reinterpret_cast<char *>(a), 32),
              std::string(reinterpret_cast<char *>(b), 32));
}

/* 56 bytes leave no room for the 8-byte length in a 64-byte block: the padding
   spills into a second block (FIPS 180-2 examples). */
TEST(crypto_sha, padding_that_spills_into_a_second_block)
{
    const char *text = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_1, text), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_256, text),
              "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

/* 112 bytes leave no room for the 16-byte length in a 128-byte block. */
TEST(crypto_sha, sha512_padding_that_spills_into_a_second_block)
{
    EXPECT_EQ(digest_hex(lh_crypto_sha_kind_512,
                         "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
                         "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
              "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
              "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909");
}

TEST(crypto_sha, add_state_word_32_wraps_at_32_bits)
{
    lh_crypto_sha_t hasher;

    lh_crypto_sha_init(lh_addr_of(hasher), lh_crypto_sha_kind_256);
    lh_crypto_sha_set_state_word(lh_addr_of(hasher), 0, 0xFFFFFFFFU);
    lh_crypto_sha_add_state_word_32(lh_addr_of(hasher), 0, 2U);
    EXPECT_EQ(lh_crypto_sha_get_state_word(lh_addr_of(hasher), 0), 1U);

    lh_crypto_sha_set_state_word(lh_addr_of(hasher), 1, 0xFFFFFFFFU);
    lh_crypto_sha_add_state_word(lh_addr_of(hasher), 1, 2U);
    EXPECT_EQ(lh_crypto_sha_get_state_word(lh_addr_of(hasher), 1), 0x100000001ULL);
}

TEST(crypto_sha, fill_block_appends_and_moves_the_fill)
{
    lh_crypto_sha_t hasher;
    const lh_byte_t bytes[3] = {1, 2, 3};

    lh_crypto_sha_init(lh_addr_of(hasher), lh_crypto_sha_kind_256);
    lh_crypto_sha_fill_block(lh_addr_of(hasher), bytes, 2);
    lh_crypto_sha_fill_block(lh_addr_of(hasher), bytes + 2, 1);
    EXPECT_EQ(lh_crypto_sha_get_block_fill(lh_addr_of(hasher)), 3U);
    EXPECT_EQ(lh_crypto_sha_get_block(lh_addr_of(hasher))[2], 3U);
    EXPECT_EQ(lh_crypto_sha_get_block_tail_mut(lh_addr_of(hasher)),
              lh_crypto_sha_get_block_mut(lh_addr_of(hasher)) + 3);

    lh_crypto_sha_zero_block_to(lh_addr_of(hasher), 8);
    EXPECT_EQ(lh_crypto_sha_get_block(lh_addr_of(hasher))[7], 0U);
    EXPECT_EQ(lh_crypto_sha_get_block_fill(lh_addr_of(hasher)), 3U) << "zeroing does not move the fill";
}

} // namespace
