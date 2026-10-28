#include <gtest/gtest.h>

#include <lh/str/list/span.h>

namespace
{

TEST(str_list_span, stores_offset_and_size)
{
    lh_str_list_span_t span;
    lh_str_list_span_init(&span, 7, 3);
    EXPECT_EQ(lh_str_list_span_get_offset(&span), 7u);
    EXPECT_EQ(lh_str_list_span_get_size(&span), 3u);
    lh_str_list_span_set_offset(&span, 9);
    lh_str_list_span_set_size(&span, 0);
    EXPECT_EQ(lh_str_list_span_get_offset(&span), 9u);
    EXPECT_EQ(lh_str_list_span_get_size(&span), 0u);
}

} // namespace
