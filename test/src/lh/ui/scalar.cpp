#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/ui/scalar.h>
#include <lh/test/out.h>

TEST(ui_scalar, centre_matches_layout_half_up)
{
    EXPECT_EQ(lh_ui_scalar_centre(lh_ui_scalar(16), lh_ui_scalar(15)), lh_ui_scalar(1));
    EXPECT_EQ(lh_ui_scalar_centre(lh_ui_scalar(16), lh_ui_scalar(9)), lh_ui_scalar(4));
    EXPECT_EQ(lh_ui_scalar_centre(lh_ui_scalar(4), lh_ui_scalar(10)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_scalar_centre(lh_ui_scalar(5), lh_ui_scalar(0)), lh_ui_scalar(3));
}

TEST(ui_scalar, slack_is_non_negative)
{
    EXPECT_EQ(lh_ui_scalar_slack(lh_ui_scalar(10), lh_ui_scalar(4)), lh_ui_scalar(6));
    EXPECT_EQ(lh_ui_scalar_slack(lh_ui_scalar(4), lh_ui_scalar(10)), lh_ui_scalar(0));
}

TEST(ui_scalar, trunc_of_a_half_drops_it)
{
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar_of(3) + lh_ui_scalar_of(1) / lh_ui_scalar(2)), lh_ui_scalar(3));
#if LH_LIBRARY_OPTION_MATH_FPU
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar_of(-3) - lh_ui_scalar_of(1) / lh_ui_scalar(2)), lh_ui_scalar(-3));
#endif
}

#if LH_LIBRARY_OPTION_MATH_FPU

TEST(ui_scalar, trunc_drops_the_fraction_toward_zero)
{
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(45.5)), lh_ui_scalar(45));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(97.5)), lh_ui_scalar(97));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(-2.5)), lh_ui_scalar(-2));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(3)), lh_ui_scalar(3));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(0)), lh_ui_scalar(0));
}

TEST(ui_scalar, trunc_may_write_over_its_input)
{
    lh_ui_scalar_t x = lh_ui_scalar(45.5);

    lh_ui_scalar_trunc(x, lh_addr_of(x));
    EXPECT_EQ(x, lh_ui_scalar(45));
}

#else

TEST(ui_scalar, trunc_copies_a_whole_scalar)
{
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(45)), lh_ui_scalar(45));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(-2)), lh_ui_scalar(-2));
    EXPECT_EQ(lh_test::out(lh_ui_scalar_trunc, lh_ui_scalar(0)), lh_ui_scalar(0));
}

TEST(ui_scalar, trunc_may_write_over_its_input)
{
    lh_ui_scalar_t x = lh_ui_scalar(-7);

    lh_ui_scalar_trunc(x, lh_addr_of(x));
    EXPECT_EQ(x, lh_ui_scalar(-7));
}

#endif

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(ui_scalar_death, trunc_null_out)
{
    LH_EXPECT_DEATH(lh_ui_scalar_trunc(lh_ui_scalar(1), lh_null));
}

#endif
