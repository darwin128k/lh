/** @file request.cpp
 * @brief HTTP request target path tests.
 */

#include <gtest/gtest.h>

#include <lh/core/http/request.h>

TEST(http_request_get_path, keeps_the_path_before_the_first_query_delimiter)
{
    const lh_str_view_t cases[] = {
        lh_str_view_initializer_lit("/ui"), lh_str_view_initializer_lit("/ui?x=1?y=2"),
        lh_str_view_initializer_lit("?x=1"), lh_str_view_initializer_lit("/ui?")};
    const lh_str_view_t expected[] = {
        lh_str_view_initializer_lit("/ui"), lh_str_view_initializer_lit("/ui"),
        lh_str_view_initializer_lit(""), lh_str_view_initializer_lit("/ui")};

    lh_http_request_t request;
    for (lh_usize_t i = 0; i < 4; i = lh_math_add_one(i))
    {
        lh_http_request_set_target(lh_addr_of(request), cases[i]);
        const lh_str_view_t path = lh_http_request_get_path(lh_addr_of(request));
        EXPECT_TRUE(lh_str_view_equals(lh_addr_of(path), lh_addr_of(expected[i]), lh_bool_false));
        EXPECT_EQ(lh_str_view_get_data(lh_addr_of(path)),
                  lh_str_view_get_data(lh_addr_of(cases[i])));
    }
}

TEST(http_request_get_query_value_size, finds_first_field_prefix_and_preserves_empty_values)
{
    lh_http_request_t request;
    lh_http_request_set_target(lh_addr_of(request), lh_str_view_lit("/p?xx=bad&x=42&x=8&empty="));
    lh_str_cptr value = lh_null;
    EXPECT_EQ(
        lh_http_request_get_query_value_size(lh_addr_of(request), "x=", 2U, lh_addr_of(value)), 2U);
    EXPECT_EQ(value[0], '4');
    EXPECT_EQ(value[1], '2');
    EXPECT_EQ(
        lh_http_request_get_query_value_size(lh_addr_of(request), "empty=", 6U, lh_addr_of(value)),
        0U);
    EXPECT_NE(value, nullptr);
    lh_str_cptr previous = value;
    EXPECT_EQ(
        lh_http_request_get_query_value_size(lh_addr_of(request), "absent=", 7U, lh_addr_of(value)),
        0U);
    EXPECT_EQ(value, previous);
    EXPECT_EQ(lh_http_request_get_query_value_size(lh_addr_of(request), "", 0U, lh_addr_of(value)),
              0U);
}
