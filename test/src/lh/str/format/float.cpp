#include <gtest/gtest.h>

#include <lh/str/format/float.h>

#include <cstdint>
#include <cstring>
#include <string>

namespace
{

std::string
format(std::uint32_t bits, lh_usize_t digits)
{
    float value;
    char out[64];
    std::memcpy(&value, &bits, sizeof value);
    const lh_usize_t n = lh_str_ptr_format_float(value, digits, out, sizeof out);
    return std::string(out, n);
}

TEST(str_format_float, writes_shortest_g_style_text)
{
    EXPECT_EQ(format(0x3f800000U, 0), "1");
    EXPECT_EQ(format(0x3dcccccdU, 0), "0.1");
    EXPECT_EQ(format(0x3dcccccdU, 9), "0.100000001");
    EXPECT_EQ(format(0x461c3c00U, 0), "9999");
    EXPECT_EQ(format(0x38d1b717U, 0), "0.0001");
    EXPECT_EQ(format(0xc0490fdbU, 6), "-3.14159");
}

TEST(str_format_float, switches_to_exponent_form_outside_the_precision)
{
    EXPECT_EQ(format(0x4e6e6b28U, 0), "1e+09");
    EXPECT_EQ(format(0x447a0000U, 3), "1e+03");
    EXPECT_EQ(format(0x7f7fffffU, 3), "3.4e+38");
    EXPECT_EQ(format(0x00000001U, 0), "1.4013e-45"); // the smallest subnormal
}

TEST(str_format_float, rounds_half_to_even_and_carries_into_the_exponent)
{
    EXPECT_EQ(format(0x3f7fffffU, 2), "1"); // 0.99999994 to two digits
}

TEST(str_format_float, writes_special_values)
{
    EXPECT_EQ(format(0x80000000U, 0), "-0");
    EXPECT_EQ(format(0xff800000U, 0), "-inf");
    EXPECT_EQ(format(0x7fc00000U, 0), "nan");
}

TEST(str_format_float, writes_nothing_when_it_does_not_fit)
{
    char out[4] = {'x', 'x', 'x', 'x'};
    EXPECT_EQ(lh_str_ptr_format_float(-3.14159f, 6, out, sizeof out), 0u);
    EXPECT_EQ(out[0], 'x');
}

} // namespace
