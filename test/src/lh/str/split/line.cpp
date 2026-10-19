#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <lh/str/split/line.h>

namespace
{

std::vector<std::string>
split_lines(const std::string &text)
{
    std::vector<std::string> lines;
    lh_usize_t pos = 0;
    lh_str_cptr line;
    lh_usize_t line_size;

    while (lh_str_ptr_split_next_line(text.data(), text.size(), &pos, &line, &line_size))
    {
        lines.emplace_back(line, line_size);
    }
    return lines;
}

using lines_t = std::vector<std::string>;

TEST(str_ptr_split_next_line, splits_at_lf)
{
    EXPECT_EQ(split_lines("a\nbc\nd"), (lines_t{"a", "bc", "d"}));
}

TEST(str_ptr_split_next_line, treats_crlf_as_one_end)
{
    EXPECT_EQ(split_lines("a\r\nbc\r\nd"), (lines_t{"a", "bc", "d"}));
}

TEST(str_ptr_split_next_line, splits_at_lone_cr)
{
    EXPECT_EQ(split_lines("a\rbc\rd"), (lines_t{"a", "bc", "d"}));
}

TEST(str_ptr_split_next_line, handles_mixed_ends)
{
    EXPECT_EQ(split_lines("a\r\nb\nc\rd"), (lines_t{"a", "b", "c", "d"}));
}

TEST(str_ptr_split_next_line, trailing_end_adds_no_line)
{
    EXPECT_EQ(split_lines("a\n"), (lines_t{"a"}));
    EXPECT_EQ(split_lines("a\r\n"), (lines_t{"a"}));
    EXPECT_EQ(split_lines("a\r"), (lines_t{"a"}));
}

TEST(str_ptr_split_next_line, keeps_empty_lines)
{
    EXPECT_EQ(split_lines("a\n\nb"), (lines_t{"a", "", "b"}));
    EXPECT_EQ(split_lines("a\r\n\r\nb"), (lines_t{"a", "", "b"}));
    EXPECT_EQ(split_lines("\n"), (lines_t{""}));
    EXPECT_EQ(split_lines("a\n\n"), (lines_t{"a", ""}));
}

TEST(str_ptr_split_next_line, cr_then_cr_lf_is_two_ends)
{
    EXPECT_EQ(split_lines("a\r\r\nb"), (lines_t{"a", "", "b"}));
}

TEST(str_ptr_split_next_line, lf_then_cr_is_two_ends)
{
    EXPECT_EQ(split_lines("a\n\rb"), (lines_t{"a", "", "b"}));
}

TEST(str_ptr_split_next_line, empty_text_has_no_lines)
{
    EXPECT_EQ(split_lines(""), lines_t{});
}

TEST(str_view_split_next_line, walks_lines_as_views)
{
    const lh_str_view_t text = lh_str_view_lit("key\r\n\r\nvalue");
    lh_usize_t pos = 0;
    lh_str_view_t line;

    ASSERT_TRUE(lh_str_view_split_next_line(&text, &pos, &line));
    EXPECT_EQ(std::string(lh_str_view_get_data(&line), lh_str_view_get_size(&line)), "key");

    ASSERT_TRUE(lh_str_view_split_next_line(&text, &pos, &line));
    EXPECT_TRUE(lh_str_view_is_empty(&line));

    ASSERT_TRUE(lh_str_view_split_next_line(&text, &pos, &line));
    EXPECT_EQ(std::string(lh_str_view_get_data(&line), lh_str_view_get_size(&line)), "value");

    EXPECT_FALSE(lh_str_view_split_next_line(&text, &pos, &line));
}

TEST(str_view_split_next_line, empty_view_has_no_lines)
{
    const lh_str_view_t text = lh_str_view_empty();
    lh_usize_t pos = 0;
    lh_str_view_t line;

    EXPECT_FALSE(lh_str_view_split_next_line(&text, &pos, &line));
}

} // namespace
