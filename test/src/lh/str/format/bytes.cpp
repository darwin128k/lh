#include <gtest/gtest.h>

#include <string>

#include <lh/str/format/bytes.h>

namespace
{

TEST(str_ptr_format_bytes_hex, formats_lowercase)
{
    const lh_uchar_t data[] = {0xDE, 0xAD};
    lh_char_t buf[8];
    lh_usize_t written = lh_str_ptr_format_bytes_hex(const_cast<lh_uchar_t *>(data), 2, lh_bool_false,
                                                     buf, sizeof(buf));
    ASSERT_EQ(written, 4U);
    EXPECT_EQ(std::string(buf, written), "dead");
}

TEST(str_ptr_format_bytes_hex, formats_uppercase)
{
    const lh_uchar_t data[] = {0xDE, 0xAD};
    lh_char_t buf[8];
    lh_usize_t written = lh_str_ptr_format_bytes_hex(const_cast<lh_uchar_t *>(data), 2, lh_bool_true,
                                                     buf, sizeof(buf));
    ASSERT_EQ(written, 4U);
    EXPECT_EQ(std::string(buf, written), "DEAD");
}

TEST(str_ptr_format_bytes_hex, returns_zero_when_buffer_too_small)
{
    const lh_uchar_t data[] = {0x00};
    lh_char_t buf[8];
    EXPECT_EQ(lh_str_ptr_format_bytes_hex(const_cast<lh_uchar_t *>(data), 1, lh_bool_false, buf, 1),
              0U);
}

TEST(str_ptr_format_bytes_hex_dump, writes_offset_and_bytes)
{
    const lh_uchar_t data[] = {0x00, 0x11};
    lh_char_t buf[64];
    lh_usize_t written = lh_str_ptr_format_bytes_hex_dump(const_cast<lh_uchar_t *>(data), 2,
                                                          lh_bool_false, buf, sizeof(buf));
    ASSERT_NE(written, 0U);
    EXPECT_EQ(std::string(buf, written), "00000000: 00 11 \n");
}

TEST(str_ptr_format_bytes_hex_dump_size, matches_what_the_dump_writes)
{
    lh_uchar_t data[40];
    char str[256];
    for (lh_usize_t n = 1; n <= sizeof(data); ++n)
    {
        for (lh_usize_t i = 0; i < n; ++i)
        {
            data[i] = static_cast<lh_uchar_t>(i);
        }
        const lh_usize_t size = lh_str_ptr_format_bytes_hex_dump_size(n);
        EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump(data, n, lh_bool_false, str, sizeof(str)), size)
            << "n = " << n;
        EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump(data, n, lh_bool_false, str, size), size)
            << "exact capacity, n = " << n;
        EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump(data, n, lh_bool_false, str, size - 1), 0u)
            << "one short, n = " << n;
    }
}

TEST(str_ptr_format_bytes_hex_dump_size, known_values)
{
    EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump_size(0), 0u);
    EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump_size(1), 8u + 2u + 3u + 1u);
    EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump_size(16), 8u + 2u + 48u + 1u);
    EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump_size(17),
              (8u + 2u + 48u + 1u) + (8u + 2u + 3u + 1u));
}

TEST(str_ptr_format_bytes_hex_dump_size, overflow_is_zero)
{
    EXPECT_EQ(lh_str_ptr_format_bytes_hex_dump_size(LH_USIZE_T_MAX), 0u);
}

} // namespace
