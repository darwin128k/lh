#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/math/slice.h>
#include <lh/null.h>
#include <lh/util/addr.h>

#include <cmath>

namespace
{

const lh_float_t data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

lh_math_slice_t
slice_of(const lh_float_t *at, lh_usize_t count)
{
    lh_math_slice_t s;

    lh_math_slice_init(lh_addr_of(s), at, count);
    return s;
}

TEST(math_slice, init_bounds_are_the_first_and_the_last_element)
{
    const lh_math_slice_t s = slice_of(data, 5U);

    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(s)), data);
    EXPECT_EQ(lh_math_slice_get_end(lh_addr_of(s)), data + 4);
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(s)), 5U);
    EXPECT_FALSE(lh_math_slice_is_empty(lh_addr_of(s)));
    EXPECT_TRUE(lh_math_slice_is_valid(lh_addr_of(s)));
}

TEST(math_slice, one_element_is_a_closed_slice_of_size_one)
{
    const lh_math_slice_t s = slice_of(data + 2, 1U);

    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(s)), lh_math_slice_get_end(lh_addr_of(s)));
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(s)), 1U);
    EXPECT_EQ(lh_math_slice_get_at(lh_addr_of(s), 0U), 3.0f);
}

TEST(math_slice, empty_by_init_empty_by_zero_count_and_by_null)
{
    lh_math_slice_t e;

    lh_math_slice_init_empty(lh_addr_of(e));
    for (const lh_math_slice_t &s : {e, slice_of(data, 0U), slice_of(lh_null, 3U)})
    {
        EXPECT_TRUE(lh_math_slice_is_empty(lh_addr_of(s)));
        EXPECT_TRUE(lh_math_slice_is_valid(lh_addr_of(s)));
        EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(s)), 0U);
        EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(s)), nullptr);
        EXPECT_EQ(lh_math_slice_get_end(lh_addr_of(s)), nullptr);
        EXPECT_FALSE(lh_math_slice_is_valid_index(lh_addr_of(s), 0U));
    }
}

TEST(math_slice, set_and_validity)
{
    lh_math_slice_t s;

    lh_math_slice_set(lh_addr_of(s), data + 1, data + 3);
    EXPECT_TRUE(lh_math_slice_is_valid(lh_addr_of(s)));
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(s)), 3U);
    lh_math_slice_set(lh_addr_of(s), data + 3, data + 1);
    EXPECT_FALSE(lh_math_slice_is_valid(lh_addr_of(s)));
    lh_math_slice_set(lh_addr_of(s), data, lh_null);
    EXPECT_FALSE(lh_math_slice_is_valid(lh_addr_of(s)));
    EXPECT_FALSE(lh_math_slice_is_empty(lh_addr_of(s)));
}

TEST(math_slice, init_by_other_names_the_same_elements)
{
    const lh_math_slice_t a = slice_of(data, 5U);
    lh_math_slice_t b;

    lh_math_slice_init_by_other(lh_addr_of(b), lh_addr_of(a));
    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(b)), data);
    EXPECT_EQ(lh_math_slice_get_end(lh_addr_of(b)), data + 4);
}

TEST(math_slice, get_at_and_index_bounds)
{
    const lh_math_slice_t s = slice_of(data, 5U);

    EXPECT_EQ(lh_math_slice_get_at(lh_addr_of(s), 0U), 1.0f);
    EXPECT_EQ(lh_math_slice_get_at(lh_addr_of(s), 4U), 5.0f);
    EXPECT_TRUE(lh_math_slice_is_valid_index(lh_addr_of(s), 4U));
    EXPECT_FALSE(lh_math_slice_is_valid_index(lh_addr_of(s), 5U));
}

TEST(math_slice, sub_take_and_drop_share_the_data)
{
    const lh_math_slice_t s = slice_of(data, 5U);
    const lh_math_slice_t sub = lh_math_slice_get_sub(lh_addr_of(s), 1U, 3U);
    const lh_math_slice_t first = lh_math_slice_take_first(lh_addr_of(s), 2U);
    const lh_math_slice_t last = lh_math_slice_take_last(lh_addr_of(s), 2U);
    const lh_math_slice_t dropped_first = lh_math_slice_drop_first(lh_addr_of(s), 2U);
    const lh_math_slice_t dropped_last = lh_math_slice_drop_last(lh_addr_of(s), 2U);

    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(sub)), data + 1);
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(sub)), 3U);
    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(first)), data);
    EXPECT_EQ(lh_math_slice_get_end(lh_addr_of(first)), data + 1);
    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(last)), data + 3);
    EXPECT_EQ(lh_math_slice_get_end(lh_addr_of(last)), data + 4);
    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(dropped_first)), data + 2);
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(dropped_first)), 3U);
    EXPECT_EQ(lh_math_slice_get_begin(lh_addr_of(dropped_last)), data);
    EXPECT_EQ(lh_math_slice_get_size(lh_addr_of(dropped_last)), 3U);
}

TEST(math_slice, taking_or_dropping_everything_or_nothing)
{
    const lh_math_slice_t s = slice_of(data, 5U);
    const lh_math_slice_t none = lh_math_slice_take_first(lh_addr_of(s), 0U);
    const lh_math_slice_t all = lh_math_slice_take_last(lh_addr_of(s), 5U);
    const lh_math_slice_t gone = lh_math_slice_drop_first(lh_addr_of(s), 5U);
    const lh_math_slice_t kept = lh_math_slice_drop_last(lh_addr_of(s), 0U);
    lh_math_slice_t empty;

    EXPECT_TRUE(lh_math_slice_is_empty(lh_addr_of(none)));
    EXPECT_TRUE(lh_math_slice_equals(lh_addr_of(all), lh_addr_of(s)));
    EXPECT_TRUE(lh_math_slice_is_empty(lh_addr_of(gone)));
    EXPECT_TRUE(lh_math_slice_equals(lh_addr_of(kept), lh_addr_of(s)));
    lh_math_slice_init_empty(lh_addr_of(empty));
    EXPECT_TRUE(lh_math_slice_is_empty(lh_addr_of(empty)));
    const lh_math_slice_t still = lh_math_slice_take_first(lh_addr_of(empty), 0U);
    EXPECT_TRUE(lh_math_slice_is_empty(lh_addr_of(still)));
}

TEST(math_slice, equals_compares_sizes_and_elements)
{
    const lh_float_t other[] = {1.0f, 2.0f, 3.0f};
    const lh_float_t differs[] = {1.0f, 2.5f, 3.0f};
    const lh_float_t zeros[] = {0.0f};
    const lh_float_t negative_zeros[] = {-0.0f};
    const lh_float_t nan[] = {NAN};
    const lh_math_slice_t s = slice_of(data, 3U);
    lh_math_slice_t a;
    lh_math_slice_t b;

    a = slice_of(other, 3U);
    EXPECT_TRUE(lh_math_slice_equals(lh_addr_of(s), lh_addr_of(a)));
    a = slice_of(differs, 3U);
    EXPECT_FALSE(lh_math_slice_equals(lh_addr_of(s), lh_addr_of(a)));
    a = slice_of(other, 2U);
    EXPECT_FALSE(lh_math_slice_equals(lh_addr_of(s), lh_addr_of(a)));
    a = slice_of(zeros, 1U);
    b = slice_of(negative_zeros, 1U);
    EXPECT_TRUE(lh_math_slice_equals(lh_addr_of(a), lh_addr_of(b)));
    a = slice_of(nan, 1U);
    EXPECT_FALSE(lh_math_slice_equals(lh_addr_of(a), lh_addr_of(a)));
    lh_math_slice_init_empty(lh_addr_of(a));
    lh_math_slice_init_empty(lh_addr_of(b));
    EXPECT_TRUE(lh_math_slice_equals(lh_addr_of(a), lh_addr_of(b)));
}

TEST(math_slice, sum_and_dot)
{
    const lh_math_slice_t s = slice_of(data, 5U);
    const lh_math_slice_t head = lh_math_slice_take_first(lh_addr_of(s), 3U);
    const lh_math_slice_t tail = lh_math_slice_take_last(lh_addr_of(s), 3U);
    const lh_float_t negative_zeros[] = {-0.0f, -0.0f};
    const lh_math_slice_t nz = slice_of(negative_zeros, 2U);
    lh_math_slice_t empty;

    EXPECT_EQ(lh_math_slice_sum(lh_addr_of(s)), 15.0f);
    EXPECT_EQ(lh_math_slice_dot(lh_addr_of(head), lh_addr_of(tail)), 1.0f * 3.0f + 2.0f * 4.0f + 3.0f * 5.0f);
    EXPECT_TRUE(std::signbit(lh_math_slice_sum(lh_addr_of(nz)))) << "-0 + -0 is -0, as written by hand";
    lh_math_slice_init_empty(lh_addr_of(empty));
    EXPECT_EQ(lh_math_slice_sum(lh_addr_of(empty)), 0.0f);
    EXPECT_EQ(lh_math_slice_dot(lh_addr_of(empty), lh_addr_of(empty)), 0.0f);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(math_slice_death, get_at_past_the_end)
{
    const lh_math_slice_t s = slice_of(data, 5U);
    lh_math_slice_t empty;

    lh_math_slice_init_empty(lh_addr_of(empty));
    LH_EXPECT_DEATH(lh_math_slice_get_at(lh_addr_of(s), 5U));
    LH_EXPECT_DEATH(lh_math_slice_get_at(lh_addr_of(empty), 0U));
}

TEST(math_slice_death, sub_past_the_end)
{
    const lh_math_slice_t s = slice_of(data, 5U);

    LH_EXPECT_DEATH(lh_math_slice_get_sub(lh_addr_of(s), 4U, 2U));
    LH_EXPECT_DEATH(lh_math_slice_take_first(lh_addr_of(s), 6U));
    LH_EXPECT_DEATH(lh_math_slice_take_last(lh_addr_of(s), 6U));
    LH_EXPECT_DEATH(lh_math_slice_drop_first(lh_addr_of(s), 6U));
    LH_EXPECT_DEATH(lh_math_slice_drop_last(lh_addr_of(s), 6U));
}

TEST(math_slice_death, size_of_a_reversed_slice_and_dot_of_different_sizes)
{
    lh_math_slice_t reversed;
    const lh_math_slice_t a = slice_of(data, 2U);
    const lh_math_slice_t b = slice_of(data, 3U);

    lh_math_slice_set(lh_addr_of(reversed), data + 3, data + 1);
    LH_EXPECT_DEATH(lh_math_slice_get_size(lh_addr_of(reversed)));
    LH_EXPECT_DEATH(lh_math_slice_dot(lh_addr_of(a), lh_addr_of(b)));
}

#endif

} // namespace
