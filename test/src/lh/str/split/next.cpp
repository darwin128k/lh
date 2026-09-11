#include <gtest/gtest.h>

#include <string>

#include <lh/str/split/next.h>

namespace
{

TEST(str_ptr_split_next, walks_all_fields)
{
    lh_str_cptr str = "192.168.0.1";
    lh_usize_t pos = 0;
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    ASSERT_TRUE(lh_str_ptr_split_next(str, 11, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(std::string(field, field_size), "192");
    EXPECT_TRUE(had_delim);

    ASSERT_TRUE(lh_str_ptr_split_next(str, 11, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(std::string(field, field_size), "168");
    EXPECT_TRUE(had_delim);

    ASSERT_TRUE(lh_str_ptr_split_next(str, 11, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(std::string(field, field_size), "0");
    EXPECT_TRUE(had_delim);

    ASSERT_TRUE(lh_str_ptr_split_next(str, 11, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(std::string(field, field_size), "1");
    EXPECT_FALSE(had_delim);

    EXPECT_FALSE(lh_str_ptr_split_next(str, 11, '.', &pos, &field, &field_size, &had_delim));
}

TEST(str_ptr_split_next, splits_only_at_first_delim_when_called_once)
{
    lh_str_cptr str = "192.168.0.1:27015";
    lh_usize_t pos = 0;
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    ASSERT_TRUE(lh_str_ptr_split_next(str, 17, ':', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(std::string(field, field_size), "192.168.0.1");
    EXPECT_TRUE(had_delim);
    EXPECT_EQ(std::string(str + pos, 17 - pos), "27015");
}

TEST(str_ptr_split_next, reports_empty_field_between_two_delims)
{
    lh_str_cptr str = "a..b";
    lh_usize_t pos = 0;
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    ASSERT_TRUE(lh_str_ptr_split_next(str, 4, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(field_size, 1U);
    ASSERT_TRUE(lh_str_ptr_split_next(str, 4, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(field_size, 0U); /* empty field between the two dots */
    ASSERT_TRUE(lh_str_ptr_split_next(str, 4, '.', &pos, &field, &field_size, &had_delim));
    EXPECT_EQ(field_size, 1U);
    EXPECT_FALSE(had_delim);
}

TEST(str_ptr_split_next, returns_false_when_nothing_left)
{
    lh_str_cptr str = "x";
    lh_usize_t pos = 0;
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    ASSERT_TRUE(lh_str_ptr_split_next(str, 1, ',', &pos, &field, &field_size, &had_delim));
    EXPECT_FALSE(had_delim);
    EXPECT_FALSE(lh_str_ptr_split_next(str, 1, ',', &pos, &field, &field_size, &had_delim));
}

TEST(str_ptr_split_next, empty_input_returns_false)
{
    lh_str_cptr str = "";
    lh_usize_t pos = 0;
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    EXPECT_FALSE(lh_str_ptr_split_next(str, 0, '.', &pos, &field, &field_size, &had_delim));
}

} // namespace
