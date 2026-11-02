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

TEST(ui_web_js_put_str_lit, escapes_special_bytes_and_preserves_long_spans)
{
    lh_byte_t mem[256];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), "alpha'bravo\\charlie\n\r\tend");
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "'alpha\\'bravo\\\\charlie\\n\\r\\tend'");
}

TEST(ui_web_js_put_str_lit, accepts_empty_and_null_input)
{
    lh_byte_t mem[16];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), nullptr);
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), "");
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "''''");
}

TEST(ui_web_js_put_str_lit, handles_consecutive_escapes_and_utf8)
{
    lh_byte_t mem[64];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), "'\\\n\t\r\xc3\xa9");
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "'\\'\\\\\\n\\t\\r\xc3\xa9'");
}

TEST(ui_web_js_put_str_lit, reports_insufficient_space)
{
    lh_byte_t mem[4];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), "abc");
    EXPECT_FALSE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_LT(lh_ui_web_buf_len(lh_addr_of(buf)), sizeof(mem));
}

TEST(ui_web_js_x, object_keys_are_escaped_computed_string_properties)
{
    lh_byte_t mem[256];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_x_t first, second, third, object;
    lh_ui_web_x_num(1, lh_addr_of(first));
    lh_ui_web_x_num(2, lh_addr_of(second));
    lh_ui_web_x_num(3, lh_addr_of(third));
    const lh_str_ptr keys[] = {"space and 'quote'", "__proto__", "</script>"};
    const lh_ui_web_x_t *values[] = {lh_addr_of(first), lh_addr_of(second), lh_addr_of(third)};
    lh_ui_web_x_obj(keys, values, 3, lh_addr_of(object));
    lh_ui_web_js_x(lh_addr_of(buf), lh_addr_of(object));
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "{['space and \\'quote\\'']:1,['__proto__']:2,['\\x3c/script>']:3}");
}

TEST(ui_web_js_x, uses_identifiers_only_when_they_preserve_property_semantics)
{
    lh_byte_t mem[256];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_x_t value, object;
    lh_ui_web_x_num(7, lh_addr_of(value));
    const lh_str_ptr keys[] = {"x", "null", "$a_9", "__proto__", "1x", "\xc3\xa9"};
    const lh_ui_web_x_t *values[] = {lh_addr_of(value), lh_addr_of(value), lh_addr_of(value),
                                     lh_addr_of(value), lh_addr_of(value), lh_addr_of(value)};
    lh_ui_web_x_obj(keys, values, 6, lh_addr_of(object));
    lh_ui_web_js_x(lh_addr_of(buf), lh_addr_of(object));
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "{x:7,null:7,$a_9:7,['__proto__']:7,['1x']:7,['\xc3\xa9']:7}");
}

TEST(ui_web_js_x, emits_an_empty_object_without_fields)
{
    lh_byte_t mem[16];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_x_t object;
    lh_ui_web_x_obj(nullptr, nullptr, 0, lh_addr_of(object));
    lh_ui_web_js_x(lh_addr_of(buf), lh_addr_of(object));
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "{}");
}

TEST(ui_web_js_put_str_lit, escapes_html_script_closing_sequences)
{
    lh_byte_t mem[64];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(lh_addr_of(buf), mem, sizeof(mem));
    lh_ui_web_js_put_str_lit(lh_addr_of(buf), "</script><");
    ASSERT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buf)));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(lh_addr_of(buf))),
                          lh_ui_web_buf_len(lh_addr_of(buf))),
              "'\\x3c/script>\\x3c'");
}

} // namespace
