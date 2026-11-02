/** @file request.cpp
 * @brief HTTP request target path tests.
 */

#include <gtest/gtest.h>

#include <lh/http/request.h>

TEST(http_request_get_path, keeps_the_path_before_the_first_query_delimiter)
{
    lh_http_request_t request;
    const lh_str_view_t cases[] = {
        lh_str_view_initializer_lit("/ui"), lh_str_view_initializer_lit("/ui?x=1?y=2"),
        lh_str_view_initializer_lit("?x=1"), lh_str_view_initializer_lit("/ui?")};
    const lh_str_view_t expected[] = {
        lh_str_view_initializer_lit("/ui"), lh_str_view_initializer_lit("/ui"),
        lh_str_view_initializer_lit(""), lh_str_view_initializer_lit("/ui")};

    for (lh_usize_t i = 0; i < 4; i = lh_math_add_one(i))
    {
        lh_http_request_set_target(lh_addr_of(request), cases[i]);
        const lh_str_view_t path = lh_http_request_get_path(lh_addr_of(request));
        EXPECT_TRUE(lh_str_view_equals(lh_addr_of(path), lh_addr_of(expected[i]), lh_bool_false));
        EXPECT_EQ(lh_str_view_get_data(lh_addr_of(path)),
                  lh_str_view_get_data(lh_addr_of(cases[i])));
    }
}
