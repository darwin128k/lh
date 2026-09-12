#include <gtest/gtest.h>

#include <lh/foreach.h>

namespace
{

TEST(foreach, visits_every_element_in_order)
{
    int v[3] = {1, 2, 3};
    int seen[3] = {0, 0, 0};
    lh_usize_t i = 0;

    lh_foreach(int, it, v, 3)
    {
        seen[i++] = *it;
    }

    EXPECT_EQ(i, 3U);
    EXPECT_EQ(seen[0], 1);
    EXPECT_EQ(seen[1], 2);
    EXPECT_EQ(seen[2], 3);
}

TEST(foreach, can_mutate_elements_in_place)
{
    int v[3] = {1, 2, 3};
    lh_foreach(int, it, v, 3)
    {
        *it *= 2;
    }
    EXPECT_EQ(v[0], 2);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 6);
}

TEST(foreach, over_zero_elements_does_not_execute_body)
{
    int v[1] = {42};
    lh_usize_t n = 0;
    int calls = 0;

    lh_foreach(int, it, v, n)
    {
        (void)it;
        ++calls;
    }

    EXPECT_EQ(calls, 0);
}

TEST(foreach, can_be_nested_without_name_collision)
{
    int outer[2] = {1, 2};
    int inner[2] = {10, 20};
    int sum = 0;

    lh_foreach(int, it, outer, 2)
    {
        lh_foreach(int, jt, inner, 2)
        {
            sum += *it * *jt;
        }
    }

    // (1*10 + 1*20) + (2*10 + 2*20) = 30 + 60 = 90
    EXPECT_EQ(sum, 90);
}

} // namespace
