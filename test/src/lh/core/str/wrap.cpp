/**
 * @file wrap.cpp
 * @brief Tests for `lh/core/str/wrap.h`.
 *
 * A monospace font of 10 pixels a code — 0 for '~', which has no glyph — breaks
 * texts the ways the line rules say: at the last space before the edge, at a line
 * feed wherever it is, a word wider than the width on a line of its own and cut,
 * a first letter wider than the width still taking its line, spaces hanging past
 * the edge and skipped before the next line, and no breaking at a width of 0.
 */

#include <gtest/gtest.h>

#include <lh/core/str/wrap.h>

#include <string>
#include <vector>

namespace
{

lh_s32_t
mono(lh_ptr context, lh_u32_t code)
{
    (void)context;
    return code == '~' ? 0 : 10;
}

/* Every line of @p text broken to @p width, as strings. */
std::vector<std::string>
lines_of(const char *text, lh_s32_t width)
{
    std::vector<std::string> out;
    lh_str_wrap_t line;

    for (const lh_char_t *at = text; lh_str_wrap_measure(&line, at, width, mono, nullptr);
         at = lh_str_wrap_get_next(&line))
    {
        out.emplace_back(lh_str_wrap_get_begin(&line), lh_str_wrap_get_end(&line));
    }
    return out;
}

TEST(str_wrap, a_line_feed_or_the_end_ends_a_line)
{
    EXPECT_TRUE(lh_str_wrap_is_line_end('\n'));
    EXPECT_TRUE(lh_str_wrap_is_line_end(0U));
    EXPECT_FALSE(lh_str_wrap_is_line_end('A'));
    EXPECT_FALSE(lh_str_wrap_is_line_end(' '));
}

TEST(str_wrap, words_break_at_the_last_space_before_the_edge)
{
    /* 6 codes a line. */
    EXPECT_EQ(lines_of("one two three four", 60),
              (std::vector<std::string>{"one", "two", "three", "four"}));
    EXPECT_EQ(lines_of("ab cd ef gh", 60), (std::vector<std::string>{"ab cd", "ef gh"}));
    /* It breaks at the last space; the spaces before that one stay on the line (they
       hang past the edge), and the next line starts past it. */
    EXPECT_EQ(lines_of("ab   cd", 30), (std::vector<std::string>{"ab  ", "cd"}));
}

TEST(str_wrap, a_line_feed_breaks_wherever_it_is)
{
    EXPECT_EQ(lines_of("ab\ncd\n\nef", 0), (std::vector<std::string>{"ab", "cd", "", "ef"}));
    EXPECT_EQ(lines_of("ab\n", 60), (std::vector<std::string>{"ab", ""}))
        << "a line feed at the end opens an empty last line";
}

TEST(str_wrap, a_long_word_is_cut_and_a_wide_first_letter_takes_its_line)
{
    EXPECT_EQ(lines_of("abcdefgh", 30), (std::vector<std::string>{"abc", "def", "gh"}));
    EXPECT_EQ(lines_of("ab", 5), (std::vector<std::string>{"a", "b"}))
        << "every line moves the text on";
}

TEST(str_wrap, a_space_hangs_and_a_code_with_no_glyph_takes_no_width)
{
    lh_str_wrap_t line;

    ASSERT_TRUE(lh_str_wrap_measure(&line, "abc defg", 30, mono, nullptr));
    EXPECT_EQ(lh_str_wrap_get_width(&line), 30) << "the space past the edge is not counted";
    EXPECT_STREQ(lh_str_wrap_get_next(&line), "defg");
    ASSERT_TRUE(lh_str_wrap_measure(&line, "a~~b", 20, mono, nullptr));
    EXPECT_EQ(lh_str_wrap_get_width(&line), 20);
    EXPECT_EQ(lh_str_wrap_get_next(&line), nullptr);
    EXPECT_FALSE(lh_str_wrap_measure(&line, nullptr, 20, mono, nullptr));
    EXPECT_EQ(lh_str_wrap_get_begin(&line), nullptr);
}

TEST(str_wrap, the_next_line_begins_past_the_break)
{
    const char *text = "ab  cd\nef";

    EXPECT_STREQ(lh_str_wrap_get_next_of(text + 2, 30), "cd\nef") << "past the spaces";
    EXPECT_STREQ(lh_str_wrap_get_next_of(text + 2, 0), "  cd\nef") << "no width: no skipping";
    EXPECT_STREQ(lh_str_wrap_get_next_of(text + 6, 30), "ef") << "past the line feed";
    EXPECT_EQ(lh_str_wrap_get_next_of(text + 9, 30), nullptr) << "the end";
}

} // namespace
