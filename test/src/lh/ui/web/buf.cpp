/**
 * @file buf.cpp
 * @brief Web serialization buffer boundary tests.
 */

#include <gtest/gtest.h>

#include <lh/size/limits.h>
#include <lh/ui/web/buf.h>

TEST(ui_web_buf_get_mem, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_byte_t bytes[1];
    lh_ui_web_buf_set_mem(lh_addr_of(buffer), lh_addr_of(bytes[0]));
    EXPECT_EQ(lh_ui_web_buf_get_mem(lh_addr_of(buffer)), lh_addr_of(bytes[0]));
}

TEST(ui_web_buf_set_mem, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_byte_t bytes[1];
    lh_ui_web_buf_set_mem(lh_addr_of(buffer), lh_addr_of(bytes[0]));
    EXPECT_EQ(lh_ui_web_buf_get_mem(lh_addr_of(buffer)), lh_addr_of(bytes[0]));
}

TEST(ui_web_buf_get_cap, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_cap(lh_addr_of(buffer), 7u);
    EXPECT_EQ(lh_ui_web_buf_get_cap(lh_addr_of(buffer)), 7u);
}

TEST(ui_web_buf_set_cap, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_cap(lh_addr_of(buffer), 7u);
    EXPECT_EQ(lh_ui_web_buf_get_cap(lh_addr_of(buffer)), 7u);
}

TEST(ui_web_buf_get_len, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_len(lh_addr_of(buffer), 3u);
    EXPECT_EQ(lh_ui_web_buf_get_len(lh_addr_of(buffer)), 3u);
}

TEST(ui_web_buf_set_len, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_len(lh_addr_of(buffer), 3u);
    EXPECT_EQ(lh_ui_web_buf_get_len(lh_addr_of(buffer)), 3u);
}

TEST(ui_web_buf_is_ok, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_ok(lh_addr_of(buffer), lh_bool_true);
    EXPECT_EQ(lh_ui_web_buf_is_ok(lh_addr_of(buffer)), lh_bool_true);
}

TEST(ui_web_buf_set_ok, stores_or_reads_the_field)
{
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_set_ok(lh_addr_of(buffer), lh_bool_true);
    EXPECT_EQ(lh_ui_web_buf_is_ok(lh_addr_of(buffer)), lh_bool_true);
}

TEST(ui_web_buf_lit, appends_and_reserves_a_terminator)
{
    lh_byte_t bytes[5] = {};
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_init(lh_addr_of(buffer), bytes, sizeof(bytes));
    lh_ui_web_buf_lit(lh_addr_of(buffer), "abcd", 4);
    EXPECT_TRUE(lh_ui_web_buf_ok(lh_addr_of(buffer)));
    EXPECT_EQ(lh_ui_web_buf_len(lh_addr_of(buffer)), 4u);
    lh_ui_web_buf_finish(lh_addr_of(buffer));
    EXPECT_EQ(bytes[4], 0);
    lh_ui_web_buf_lit(lh_addr_of(buffer), "x", 1);
    EXPECT_FALSE(lh_ui_web_buf_ok(lh_addr_of(buffer)));
    EXPECT_EQ(lh_ui_web_buf_len(lh_addr_of(buffer)), 4u);
    EXPECT_EQ(bytes[4], 0);
}

TEST(ui_web_buf_lit, rejects_unrepresentable_size_without_writing)
{
    lh_byte_t bytes[4] = {1, 2, 3, 4};
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_init(lh_addr_of(buffer), bytes, sizeof(bytes));
    lh_ui_web_buf_lit(lh_addr_of(buffer), "x", LH_USIZE_T_MAX);
    EXPECT_FALSE(lh_ui_web_buf_ok(lh_addr_of(buffer)));
    EXPECT_EQ(lh_ui_web_buf_len(lh_addr_of(buffer)), 0u);
    EXPECT_EQ(bytes[0], 1);
}

TEST(ui_web_buf_lit, refuses_an_invalid_length_without_underflow)
{
    lh_byte_t bytes[4] = {1, 2, 3, 4};
    lh_ui_web_buf_t buffer;
    lh_ui_web_buf_init(lh_addr_of(buffer), bytes, sizeof(bytes));
    lh_ui_web_buf_set_len(lh_addr_of(buffer), 5);
    lh_ui_web_buf_lit(lh_addr_of(buffer), "x", 1);
    EXPECT_FALSE(lh_ui_web_buf_ok(lh_addr_of(buffer)));
    EXPECT_EQ(lh_ui_web_buf_len(lh_addr_of(buffer)), 5u);
    EXPECT_EQ(bytes[0], 1);
}
