#include <gtest/gtest.h>

#include <string>

#include <lh/core/str/scanf/next.h>

namespace
{

TEST(str_ptr_scanf_next, walks_literals_and_conversions)
{
    lh_str_cptr fmt = "%u.%u";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_uint);

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_literal);
    EXPECT_EQ(std::string(lh_str_scanf_spec_get_literal(&spec),
                          lh_str_scanf_spec_get_literal_size(&spec)),
              ".");

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_uint);

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

TEST(str_ptr_scanf_next, parses_flags_width_precision)
{
    lh_str_cptr fmt = "%-05.3s";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_str);
    EXPECT_TRUE(lh_str_scanf_spec_get_left_justify(&spec));
    EXPECT_TRUE(lh_str_scanf_spec_get_zero_pad(&spec));
    EXPECT_EQ(lh_str_scanf_spec_get_width(&spec), 5U);
    EXPECT_TRUE(lh_str_scanf_spec_get_have_precision(&spec));
    EXPECT_EQ(lh_str_scanf_spec_get_precision(&spec), 3U);
}

TEST(str_ptr_scanf_next, distinguishes_hex_case)
{
    lh_str_cptr fmt = "%x%X";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_hex);
    EXPECT_FALSE(lh_str_scanf_spec_get_uppercase(&spec));

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_hex);
    EXPECT_TRUE(lh_str_scanf_spec_get_uppercase(&spec));
}

TEST(str_ptr_scanf_next, percent_literal)
{
    lh_str_cptr fmt = "100%%";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_literal);

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_percent);

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

TEST(str_ptr_scanf_next, unknown_conversion_is_invalid)
{
    lh_str_cptr fmt = "%q";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_invalid);
}

TEST(str_ptr_scanf_next, trailing_percent_is_invalid)
{
    lh_str_cptr fmt = "%";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_invalid);
}

TEST(str_ptr_scanf_next, empty_mask_returns_false)
{
    lh_str_cptr fmt = "";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

} // namespace
