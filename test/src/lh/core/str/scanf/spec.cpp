#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/core/str/scanf/spec.h>

namespace
{

lh_str_scanf_spec_t
make_dirty_spec(lh_str_ptr literal)
{
    lh_str_scanf_spec_t spec;
    lh_str_scanf_spec_set_kind(&spec, lh_str_scanf_spec_kind_hex);
    lh_str_scanf_spec_set_zero_pad(&spec, lh_bool_true);
    lh_str_scanf_spec_set_left_justify(&spec, lh_bool_true);
    lh_str_scanf_spec_set_uppercase(&spec, lh_bool_true);
    lh_str_scanf_spec_set_length(&spec, lh_str_scanf_spec_length_long_long);
    lh_str_scanf_spec_set_width(&spec, 7U);
    lh_str_scanf_spec_set_have_precision(&spec, lh_bool_true);
    lh_str_scanf_spec_set_precision(&spec, 3U);
    lh_str_scanf_spec_set_literal(&spec, literal);
    lh_str_scanf_spec_set_literal_size(&spec, 4U);
    return spec;
}

TEST(str_scanf_spec, clear_resets_every_field)
{
    lh_char_t old_text[] = "old!";
    lh_char_t fmt[] = "%u";
    lh_str_scanf_spec_t spec = make_dirty_spec(old_text);

    lh_str_scanf_spec_clear(&spec, fmt);

    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_invalid);
    EXPECT_FALSE(lh_str_scanf_spec_get_zero_pad(&spec));
    EXPECT_FALSE(lh_str_scanf_spec_get_left_justify(&spec));
    EXPECT_FALSE(lh_str_scanf_spec_get_uppercase(&spec));
    EXPECT_EQ(lh_str_scanf_spec_get_length(&spec), lh_str_scanf_spec_length_none);
    EXPECT_EQ(lh_str_scanf_spec_get_width(&spec), 0U);
    EXPECT_FALSE(lh_str_scanf_spec_get_have_precision(&spec));
    EXPECT_EQ(lh_str_scanf_spec_get_precision(&spec), 0U);
    EXPECT_EQ(lh_str_scanf_spec_get_literal(&spec), fmt);
    EXPECT_EQ(lh_str_scanf_spec_get_literal_size(&spec), 0U);
}

TEST(str_scanf_spec, limit_is_the_width_when_smaller)
{
    lh_char_t fmt[] = "";
    lh_str_scanf_spec_t spec;
    lh_str_scanf_spec_clear(&spec, fmt);

    EXPECT_EQ(lh_str_scanf_spec_get_limit(&spec, 10U), 10U);
    lh_str_scanf_spec_set_width(&spec, 4U);
    EXPECT_EQ(lh_str_scanf_spec_get_limit(&spec, 10U), 4U);
    EXPECT_EQ(lh_str_scanf_spec_get_limit(&spec, 3U), 3U);
}

TEST(str_scanf_spec, pad_reaches_the_width)
{
    lh_char_t fmt[] = "";
    lh_str_scanf_spec_t spec;
    lh_str_scanf_spec_clear(&spec, fmt);

    EXPECT_EQ(lh_str_scanf_spec_get_pad(&spec, 2U), 0U);
    lh_str_scanf_spec_set_width(&spec, 5U);
    EXPECT_EQ(lh_str_scanf_spec_get_pad(&spec, 2U), 3U);
    EXPECT_EQ(lh_str_scanf_spec_get_pad(&spec, 9U), 0U);
}

TEST(str_scanf_spec, conversion_sets_kind_and_case)
{
    lh_char_t fmt[] = "";
    lh_str_scanf_spec_t spec;

    lh_str_scanf_spec_clear(&spec, fmt);
    lh_str_scanf_spec_set_conversion(&spec, 'X');
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_hex);
    EXPECT_TRUE(lh_str_scanf_spec_get_uppercase(&spec));

    lh_str_scanf_spec_clear(&spec, fmt);
    lh_str_scanf_spec_set_conversion(&spec, 'i');
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_sint);

    lh_str_scanf_spec_clear(&spec, fmt);
    lh_str_scanf_spec_set_conversion(&spec, 'q');
    EXPECT_EQ(lh_str_scanf_spec_get_kind(&spec), lh_str_scanf_spec_kind_invalid);
}

#if LH_TEST_EXPECT_DEATH_ENABLED
TEST(str_scanf_spec, clear_checks_self_before_writing)
{
    lh_char_t fmt[] = "";
    LH_EXPECT_DEATH(lh_str_scanf_spec_clear(lh_null, fmt));
}
#endif

} // namespace
