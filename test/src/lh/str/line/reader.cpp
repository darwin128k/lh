#include <gtest/gtest.h>

#include <string>

#include <lh/str/line/reader.h>

namespace
{

std::string
text(const lh_str_view_t &v)
{
    return std::string(lh_str_view_get_data(&v), lh_str_view_get_size(&v));
}

TEST(str_line_reader, reads_lines_and_counts_them)
{
    lh_str_line_reader_t reader;
    lh_str_view_t line;

    lh_str_line_reader_init(&reader, lh_str_view_lit("a\r\n\nb"));
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 0u);

    ASSERT_TRUE(lh_str_line_reader_next(&reader, &line));
    EXPECT_EQ(text(line), "a");
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 1u);

    ASSERT_TRUE(lh_str_line_reader_next(&reader, &line));
    EXPECT_TRUE(lh_str_view_is_empty(&line));
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 2u);

    ASSERT_TRUE(lh_str_line_reader_next(&reader, &line));
    EXPECT_EQ(text(line), "b");
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 3u);

    EXPECT_FALSE(lh_str_line_reader_next(&reader, &line));
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 3u);
}

TEST(str_line_reader, empty_text_has_no_lines)
{
    lh_str_line_reader_t reader;
    lh_str_view_t line;

    lh_str_line_reader_init(&reader, lh_str_view_empty());
    EXPECT_FALSE(lh_str_line_reader_next(&reader, &line));
    EXPECT_EQ(lh_str_line_reader_get_number(&reader), 0u);
}

} // namespace
