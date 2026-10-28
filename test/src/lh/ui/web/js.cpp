/**
 * @file js.cpp
 * @brief JavaScript text emission: integers.
 */

#include <gtest/gtest.h>

#include <lh/ui/web/js/x.h>

#include <cstdint>
#include <string>

namespace
{

std::string
put_s64(lh_s64_t n)
{
    lh_byte_t mem[32];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(&buf, mem, sizeof mem);
    lh_ui_web_js_put_s64(&buf, n);
    EXPECT_TRUE(lh_ui_web_buf_ok(&buf));
    return std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(&buf)),
                       lh_ui_web_buf_len(&buf));
}

TEST(ui_web_js_put_s64, writes_decimal_with_sign)
{
    EXPECT_EQ(put_s64(0), "0");
    EXPECT_EQ(put_s64(7), "7");
    EXPECT_EQ(put_s64(-42), "-42");
    EXPECT_EQ(put_s64(INT64_MAX), "9223372036854775807");
    EXPECT_EQ(put_s64(INT64_MIN), "-9223372036854775808");
}

} // namespace
