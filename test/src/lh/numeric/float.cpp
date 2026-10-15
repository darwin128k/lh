#include <gtest/gtest.h>

#include <lh/numeric/float.h>

TEST(numeric_float, min_is_positive)
{
    EXPECT_GT(LH_FLOAT_T_MIN, 0.0f);
}

TEST(numeric_float, max_is_greater_than_min)
{
    EXPECT_GT(LH_FLOAT_T_MAX, LH_FLOAT_T_MIN);
}

TEST(numeric_float, eps_is_between_zero_and_one)
{
    EXPECT_GT(LH_FLOAT_T_EPS, 0.0f);
    EXPECT_LT(LH_FLOAT_T_EPS, 1.0f);
}

TEST(numeric_float, one_plus_eps_differs_from_one)
{
    volatile lh_float_t one = 1.0f;
    volatile lh_float_t sum = one + LH_FLOAT_T_EPS;
    EXPECT_NE(sum, one);
}
