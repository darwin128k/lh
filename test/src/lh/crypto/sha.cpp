#include <gtest/gtest.h>

#include <string>

#include <lh/crypto/sha.h>
#include <lh/str/format/bytes.h>
#include <lh/util/addr.h>

namespace
{

static std::string
digest_hex(lh_crypto_sha_kind_t kind, const char *text)
{
    lh_uchar_t digest[LH_CRYPTO_SHA_DIGEST_SIZE_MAX];
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
    lh_uchar_t a[32];
    lh_uchar_t b[32];
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

} // namespace
