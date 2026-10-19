#include <gtest/gtest.h>

#include <string>

#include <lh/str/split/once.h>

namespace
{

std::string
text(const lh_str_view_t &v)
{
    return std::string(lh_str_view_get_data(&v), lh_str_view_get_size(&v));
}

TEST(str_view_split_once, cuts_at_first_delim_only)
{
    const lh_str_view_t v = lh_str_view_lit("a=b=c");
    lh_str_view_t head;
    lh_str_view_t tail;

    ASSERT_TRUE(lh_str_view_split_once(&v, '=', &head, &tail));
    EXPECT_EQ(text(head), "a");
    EXPECT_EQ(text(tail), "b=c");
}

TEST(str_view_split_once, delim_at_edges_gives_empty_sides)
{
    const lh_str_view_t leading = lh_str_view_lit("=v");
    const lh_str_view_t trailing = lh_str_view_lit("k=");
    const lh_str_view_t only = lh_str_view_lit("=");
    lh_str_view_t head;
    lh_str_view_t tail;

    ASSERT_TRUE(lh_str_view_split_once(&leading, '=', &head, &tail));
    EXPECT_TRUE(lh_str_view_is_empty(&head));
    EXPECT_EQ(text(tail), "v");

    ASSERT_TRUE(lh_str_view_split_once(&trailing, '=', &head, &tail));
    EXPECT_EQ(text(head), "k");
    EXPECT_TRUE(lh_str_view_is_empty(&tail));

    ASSERT_TRUE(lh_str_view_split_once(&only, '=', &head, &tail));
    EXPECT_TRUE(lh_str_view_is_empty(&head));
    EXPECT_TRUE(lh_str_view_is_empty(&tail));
}

TEST(str_view_split_once, no_delim_leaves_outputs_untouched)
{
    const lh_str_view_t v = lh_str_view_lit("abc");
    const lh_str_view_t empty = lh_str_view_empty();
    const lh_str_view_t marker = lh_str_view_lit("marker");
    lh_str_view_t head = marker;
    lh_str_view_t tail = marker;

    EXPECT_FALSE(lh_str_view_split_once(&v, '=', &head, &tail));
    EXPECT_FALSE(lh_str_view_split_once(&empty, '=', &head, &tail));
    EXPECT_EQ(text(head), "marker");
    EXPECT_EQ(text(tail), "marker");
}

} // namespace
