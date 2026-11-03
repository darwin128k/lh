/**
 * @file gzip.cpp
 * @brief Gzip member checks and independent compression vectors.
 */

#include <gtest/gtest.h>
#include <lh/compress/gzip.h>
#include <lh/expect/death.h>
#include <string>
#include <vector>

const lh_byte_t gzip_sample[] = {31,  139, 8,   0,   0,   0,   0,   0,   2,   255, 243,
                                 72,  205, 201, 201, 87,  72,  175, 202, 44,  80,  100,
                                 240, 24,  101, 143, 178, 71,  217, 163, 236, 65,  204,
                                 6,   0,   141, 195, 147, 162, 176, 4,   0,   0};

TEST(compress_gzip, decodes_concatenated_members_and_checks_trailer)
{
    std::vector<lh_byte_t> input(gzip_sample, gzip_sample + sizeof(gzip_sample));
    input.insert(input.end(), gzip_sample, gzip_sample + sizeof(gzip_sample));
    std::string expected;
    for (int i = 0; i < 200; ++i)
    {
        expected.append("Hello gzip!\0", 12);
    }
    std::vector<lh_byte_t> output(expected.size());
    lh_usize_t written = 0;
    ASSERT_TRUE(lh_compress_gzip_try_decode(input.data(), input.size(), output.data(),
                                            output.size(), &written));
    EXPECT_EQ(written, expected.size());
    EXPECT_EQ(std::string(reinterpret_cast<char *>(output.data()), output.size()), expected);
    input.back() ^= 1U;
    written = 42;
    EXPECT_FALSE(lh_compress_gzip_try_decode(input.data(), input.size(), output.data(),
                                             output.size(), &written));
    EXPECT_EQ(written, 42U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_decode(input.data(), input.size(), output.data(),
                                                  output.size(), &written));
    EXPECT_FALSE(lh_compress_gzip_try_decode(input.data(), input.size() - 1, output.data(),
                                             output.size(), &written));
}

TEST(compress_gzip, validates_optional_header_fields_and_header_crc)
{
    std::vector<lh_byte_t> wire = {31, 139, 8,   30,  0,   0,   0,   0,   2,   255, 3,   0,
                                   97, 98,  99,  102, 105, 108, 101, 110, 97,  109, 101, 0,
                                   99, 111, 109, 109, 101, 110, 116, 0,   121, 41,  203, 47,
                                   40, 201, 204, 207, 75,  204, 81,  200, 72,  77,  76,  73,
                                   45, 2,   0,   135, 48,  239, 52,  15,  0,   0,   0};
    lh_byte_t out[64];
    lh_usize_t written = 0;
    ASSERT_TRUE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
    EXPECT_EQ(std::string(reinterpret_cast<char *>(out), written), "optional header");
    wire[13] ^= 1U;
    EXPECT_FALSE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
    wire[3] |= 128U;
    EXPECT_FALSE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
}

TEST(compress_gzip_header, validates_and_preserves_result_on_failure)
{
    lh_usize_t size = 77U;
    lh_compress_gzip_header(gzip_sample, sizeof(gzip_sample), &size);
    EXPECT_EQ(size, 10U);
    size = 77U;
    EXPECT_FALSE(lh_compress_gzip_try_header(gzip_sample, 9U, &size));
    EXPECT_EQ(size, 77U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_header(gzip_sample, 9U, &size));
}

TEST(compress_gzip_trailer, verifies_content_and_length)
{
    std::string decoded;
    for (int i = 0; i < 100; ++i)
    {
        decoded.append("Hello gzip!\0", 12);
    }
    const lh_byte_t *trailer = gzip_sample + sizeof(gzip_sample) - 8U;
    lh_compress_gzip_trailer(trailer, 8U, reinterpret_cast<const lh_byte_t *>(decoded.data()),
                             decoded.size());
    EXPECT_FALSE(lh_compress_gzip_try_trailer(
        trailer, 7U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size()));
    EXPECT_FALSE(lh_compress_gzip_try_trailer(
        trailer, 8U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size() - 1U));
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_trailer(
        trailer, 7U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size()));
}
