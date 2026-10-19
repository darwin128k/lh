#include <gtest/gtest.h>

#include <lh/str/format/base64.h>

#include <string>

namespace
{

std::string
base64(const char *text)
{
    const std::string in(text);
    char out[64];
    const lh_usize_t n =
        lh_str_ptr_format_base64(const_cast<char *>(in.data()), in.size(), out, sizeof(out));
    return std::string(out, n);
}

/* The RFC 4648 section 10 test vectors. */
TEST(str_ptr_format_base64, rfc4648_vectors)
{
    EXPECT_EQ(base64(""), "");
    EXPECT_EQ(base64("f"), "Zg==");
    EXPECT_EQ(base64("fo"), "Zm8=");
    EXPECT_EQ(base64("foo"), "Zm9v");
    EXPECT_EQ(base64("foob"), "Zm9vYg==");
    EXPECT_EQ(base64("fooba"), "Zm9vYmE=");
    EXPECT_EQ(base64("foobar"), "Zm9vYmFy");
}

TEST(str_ptr_format_base64, size_and_no_room)
{
    char out[4];
    EXPECT_EQ(lh_str_ptr_format_base64_size(0U), 0U);
    EXPECT_EQ(lh_str_ptr_format_base64_size(1U), 4U);
    EXPECT_EQ(lh_str_ptr_format_base64_size(20U), 28U);
    EXPECT_EQ(lh_str_ptr_format_base64(const_cast<char *>("foob"), 4U, out, sizeof(out)), 0U);
}

/* The RFC 6455 handshake example: the accept key of "dGhlIHNhbXBsZSBub25jZQ==". */
TEST(str_ptr_format_base64, websocket_accept_digest)
{
    const lh_byte_t digest[20] = {0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6,
                                  0x46, 0x06, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea};
    char out[28];
    ASSERT_EQ(lh_str_ptr_format_base64(const_cast<lh_byte_t *>(digest), 20U, out, sizeof(out)),
              28U);
    EXPECT_EQ(std::string(out, 28), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
}

} // namespace
