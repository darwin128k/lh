/**
 * @file input.cpp
 * @brief Tests for `lh/gx/web/input.h`.
 *
 * The pointer lines of the page: each kind is read with its point, a
 * coordinate may be negative, and a line that is not exactly a pointer line is
 * refused without touching the event.
 */

#include <gtest/gtest.h>

#include <lh/gx/web/input.h>

#include <cstring>

namespace
{

bool
parse(const char *line, lh_gx_event_t *out)
{
    return lh_gx_web_input_try_parse(line, std::strlen(line), out) == lh_bool_true;
}

TEST(gx_web_input, a_letter_names_a_kind)
{
    lh_gx_event_kind_t kind = lh_gx_event_kind_move;

    EXPECT_TRUE(lh_gx_web_input_try_get_kind('p', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_press);
    EXPECT_TRUE(lh_gx_web_input_try_get_kind('m', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_move);
    EXPECT_TRUE(lh_gx_web_input_try_get_kind('r', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_release);
    EXPECT_TRUE(lh_gx_web_input_try_get_kind('c', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_cancel);
    EXPECT_TRUE(lh_gx_web_input_try_get_kind('k', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_key);
    EXPECT_TRUE(lh_gx_web_input_try_get_kind('t', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_text);
    lh_gx_web_input_try_get_kind('c', &kind);
    EXPECT_FALSE(lh_gx_web_input_try_get_kind('P', &kind));
    EXPECT_EQ(kind, lh_gx_event_kind_cancel) << "untouched on failure";
}

TEST(gx_web_input, a_coordinate_is_a_signed_32_bit_number)
{
    lh_gx_scalar_t value = 7;

    EXPECT_TRUE(lh_gx_web_input_try_parse_scalar("-30", 3U, &value));
    EXPECT_EQ(value, -30);
    EXPECT_FALSE(lh_gx_web_input_try_parse_scalar("2147483648", 10U, &value));
    EXPECT_FALSE(lh_gx_web_input_try_parse_scalar("1.5", 3U, &value));
    EXPECT_FALSE(lh_gx_web_input_try_parse_scalar("", 0U, &value));
    EXPECT_EQ(value, -30) << "untouched on failure";
}

TEST(gx_web_input, each_line_is_read_with_its_point)
{
    lh_gx_event_t event;

    ASSERT_TRUE(parse("p 20 30", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_press);
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_event_get_at(&event)), 20);
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_event_get_at(&event)), 30);
    ASSERT_TRUE(parse("m 21 31", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_move);
    ASSERT_TRUE(parse("r 495 -30", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_release);
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_event_get_at(&event)), -30) << "outside the canvas";
    ASSERT_TRUE(parse("c", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_cancel);
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_event_get_at(&event)), 0);
}

TEST(gx_web_input, a_key_is_read_by_its_browser_name_and_a_text_by_its_code_point)
{
    lh_gx_event_t event;
    lh_gx_key_t key = lh_gx_key_none;
    lh_u32_t code = 0U;

    EXPECT_TRUE(lh_gx_web_input_try_get_key("Backspace", 9U, &key));
    EXPECT_EQ(key, lh_gx_key_backspace);
    EXPECT_TRUE(lh_gx_web_input_try_get_key("Tab", 3U, &key));
    EXPECT_EQ(key, lh_gx_key_tab);
    EXPECT_FALSE(lh_gx_web_input_try_get_key("tab", 3U, &key)) << "case counts";
    EXPECT_FALSE(lh_gx_web_input_try_get_key("F5", 2U, &key));
    EXPECT_TRUE(lh_gx_web_input_try_parse_code("1071", 4U, &code));
    EXPECT_EQ(code, 0x42FU);
    EXPECT_FALSE(lh_gx_web_input_try_parse_code("13", 2U, &code)) << "a control: Enter is a key";
    EXPECT_FALSE(lh_gx_web_input_try_parse_code("55296", 5U, &code)) << "half a surrogate pair";
    EXPECT_FALSE(lh_gx_web_input_try_parse_code("1114112", 7U, &code)) << "past U+10FFFF";

    ASSERT_TRUE(parse("k ArrowLeft", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_key);
    EXPECT_EQ(lh_gx_event_get_key(&event), lh_gx_key_left);
    ASSERT_TRUE(parse("t 128512", &event));
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_text);
    EXPECT_EQ(lh_gx_event_get_code(&event), 0x1F600U);
    EXPECT_FALSE(parse("k F5", &event)) << "a key the list does not name";
    EXPECT_FALSE(parse("k", &event));
    EXPECT_FALSE(parse("k Home End", &event));
    EXPECT_FALSE(parse("t 97 98", &event));
    EXPECT_FALSE(parse("t a", &event));
}

TEST(gx_web_input, anything_else_is_refused)
{
    lh_gx_event_t event;

    lh_gx_event_init(&event, lh_gx_event_kind_move, lh_gx_point_t{});
    EXPECT_FALSE(lh_gx_web_input_try_parse(nullptr, 0U, &event));
    EXPECT_FALSE(parse("x 1 2", &event)) << "unknown letter";
    EXPECT_FALSE(parse("press 1 2", &event)) << "a word, not a letter";
    EXPECT_FALSE(parse("p 1", &event)) << "a missing field";
    EXPECT_FALSE(parse("p 1 2 3", &event)) << "an extra field";
    EXPECT_FALSE(parse("p 1  2", &event)) << "an empty field";
    EXPECT_FALSE(parse("p a 2", &event)) << "not a number";
    EXPECT_FALSE(parse("c 1 2", &event)) << "a cancel has no point";
    EXPECT_FALSE(parse("p 1 2 ", &event)) << "a trailing space";
    EXPECT_EQ(lh_gx_event_get_kind(&event), lh_gx_event_kind_move) << "untouched";
}

} // namespace
