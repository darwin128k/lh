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

} // namespace
