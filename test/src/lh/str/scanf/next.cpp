#include <gtest/gtest.h>

#include <string>

#include <lh/str/scanf/next.h>

namespace
{

TEST(str_ptr_scanf_next, walks_literals_and_conversions)
{
    lh_str_cptr fmt = "%u.%u";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_uint);

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_literal);
    EXPECT_EQ(std::string(spec.literal, spec.literal_size), ".");

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_uint);

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

TEST(str_ptr_scanf_next, parses_flags_width_precision)
{
    lh_str_cptr fmt = "%-05.3s";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_str);
    EXPECT_TRUE(spec.left_justify);
    EXPECT_TRUE(spec.zero_pad);
    EXPECT_EQ(spec.width, 5U);
    EXPECT_TRUE(spec.have_precision);
    EXPECT_EQ(spec.precision, 3U);
}

TEST(str_ptr_scanf_next, distinguishes_hex_case)
{
    lh_str_cptr fmt = "%x%X";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_hex);
    EXPECT_FALSE(spec.uppercase);

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_hex);
    EXPECT_TRUE(spec.uppercase);
}

TEST(str_ptr_scanf_next, percent_literal)
{
    lh_str_cptr fmt = "100%%";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_literal);

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_percent);

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

TEST(str_ptr_scanf_next, unknown_conversion_is_invalid)
{
    lh_str_cptr fmt = "%q";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_invalid);
}

TEST(str_ptr_scanf_next, trailing_percent_is_invalid)
{
    lh_str_cptr fmt = "%";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    ASSERT_TRUE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
    EXPECT_EQ(spec.kind, lh_str_scanf_spec_kind_invalid);
}

TEST(str_ptr_scanf_next, empty_mask_returns_false)
{
    lh_str_cptr fmt = "";
    lh_usize_t pos = 0;
    lh_str_scanf_spec_t spec;

    EXPECT_FALSE(lh_str_ptr_scanf_next(fmt, &pos, &spec));
}

} // namespace
