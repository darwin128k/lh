#include <gtest/gtest.h>

#include <lh/pair.h>

namespace
{

TEST(pair_init, both_halves_hold_nothing)
{
    lh_pair_t p;
    lh_pair_init(&p);

    EXPECT_TRUE(lh_value_is_none(lh_pair_get_first(&p)));
    EXPECT_TRUE(lh_value_is_none(lh_pair_get_second(&p)));

    const lh_pair_t made = lh_pair_make_none();
    EXPECT_TRUE(lh_pair_equals(&made, &p));
}

TEST(pair_make, keeps_any_kinds)
{
    /* A text key with a number value, and the other way round. */
    const lh_pair_t kv = lh_pair_make(lh_value_make_str(lh_str_view_lit("port")), lh_value_make_uint(8080u));
    const lh_pair_t iv = lh_pair_make(lh_value_make_uint(3u), lh_value_make_str(lh_str_view_lit("third")));

    EXPECT_TRUE(lh_value_has_kind(lh_pair_get_first(&kv), lh_value_kind_str));
    EXPECT_EQ(lh_value_get_uint(lh_pair_get_second(&kv)), 8080u);
    EXPECT_EQ(lh_value_get_uint(lh_pair_get_first(&iv)), 3u);
    EXPECT_TRUE(lh_value_has_kind(lh_pair_get_second(&iv), lh_value_kind_str));
}

TEST(pair_set, replaces_one_half)
{
    lh_pair_t p = lh_pair_make(lh_value_make_uint(1u), lh_value_make_uint(2u));

    lh_pair_set_second(&p, lh_value_make_bool(lh_bool_true));
    EXPECT_EQ(lh_value_get_uint(lh_pair_get_first(&p)), 1u);
    EXPECT_TRUE(lh_value_get_bool(lh_pair_get_second(&p)));

    lh_pair_set_first(&p, lh_value_make_none());
    EXPECT_TRUE(lh_value_is_none(lh_pair_get_first(&p)));
}

TEST(pair_equals_and_assign, compare_both_halves)
{
    const lh_pair_t a = lh_pair_make(lh_value_make_uint(1u), lh_value_make_sint(-1));
    const lh_pair_t b = lh_pair_make(lh_value_make_uint(1u), lh_value_make_sint(-1));
    const lh_pair_t c = lh_pair_make(lh_value_make_uint(1u), lh_value_make_sint(-2));
    lh_pair_t d;

    EXPECT_TRUE(lh_pair_equals(&a, &b));
    EXPECT_FALSE(lh_pair_equals(&a, &c));

    lh_pair_init(&d);
    lh_pair_assign(&d, &c);
    EXPECT_TRUE(lh_pair_equals(&d, &c));
}

} // namespace
