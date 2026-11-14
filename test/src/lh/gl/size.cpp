#include <gtest/gtest.h>

#include <type_traits>

#include <lh/core/util/addr.h>
#include <lh/gl/size.h>
#include <lh/gl/size/initializer.h>
#include <lh/test/out.h>

namespace
{

TEST(gl_size, is_the_same_type_as_a_point)
{
    EXPECT_TRUE((std::is_same<lh_gl_size_t, lh_gl_point_t>::value));
}

TEST(gl_size, init_and_accessors_use_width_then_height)
{
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 640, 480);
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), 640);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), 480);
    lh_gl_size_set_width(lh_addr_of(size), 800);
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), 800);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), 480);
    lh_gl_size_set_height(lh_addr_of(size), 600);
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), 800);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), 600);
}

TEST(gl_size, init_empty_is_zero_and_empty)
{
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 5, 5);
    lh_gl_size_init_empty(lh_addr_of(size));
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), 0);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), 0);
    EXPECT_TRUE(lh_gl_size_is_empty(lh_addr_of(size)));
}

TEST(gl_size, is_empty_for_zero_or_negative)
{
    lh_gl_size_t zero_width;

    lh_gl_size_init(lh_addr_of(zero_width), 0, 4);
    lh_gl_size_t negative_height;

    lh_gl_size_init(lh_addr_of(negative_height), 4, -1);
    lh_gl_size_t ok;

    lh_gl_size_init(lh_addr_of(ok), 2, 3);
    EXPECT_TRUE(lh_gl_size_is_empty(lh_addr_of(zero_width)));
    EXPECT_TRUE(lh_gl_size_is_empty(lh_addr_of(negative_height)));
    EXPECT_FALSE(lh_gl_size_is_empty(lh_addr_of(ok)));
}

TEST(gl_size, from_extent_is_delta)
{
    lh_gl_point_t min;

    lh_gl_point_init(lh_addr_of(min), 1, 2);
    lh_gl_point_t max;

    lh_gl_point_init(lh_addr_of(max), 6, 5);
    const lh_gl_size_t size =
        lh_test::out(lh_gl_size_from_extent, lh_addr_of(min), lh_addr_of(max));
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), 5);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), 3);
}

TEST(gl_size, compare_suite_is_lexicographic)
{
    lh_gl_size_t a;

    lh_gl_size_init(lh_addr_of(a), 1, 8);
    lh_gl_size_t b;

    lh_gl_size_init(lh_addr_of(b), 1, 9);
    lh_gl_size_t c;

    lh_gl_size_init(lh_addr_of(c), 2, 0);
    EXPECT_EQ(lh_gl_size_eq(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_size_eq(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
    EXPECT_EQ(lh_gl_size_is_less(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    EXPECT_EQ(lh_gl_size_is_less(lh_addr_of(a), lh_addr_of(c)), lh_bool_true);
    EXPECT_EQ(lh_gl_size_is_less(lh_addr_of(c), lh_addr_of(a)), lh_bool_false);
    EXPECT_EQ(lh_gl_size_is_greater(lh_addr_of(c), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_size_is_at_least(lh_addr_of(b), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_gl_size_is_at_least(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
}

TEST(gl_size, min_max_are_component_wise)
{
    lh_gl_size_t a;

    lh_gl_size_init(lh_addr_of(a), 1, 8);
    lh_gl_size_t b;

    lh_gl_size_init(lh_addr_of(b), 4, 2);
    const lh_gl_size_t lo = lh_test::out(lh_gl_size_min, lh_addr_of(a), lh_addr_of(b));
    const lh_gl_size_t hi = lh_test::out(lh_gl_size_max, lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(lo)), 1);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(lo)), 2);
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(hi)), 4);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(hi)), 8);
}

TEST(gl_size, min_side_is_the_shorter_extent)
{
    lh_gl_size_t wide;

    lh_gl_size_init(lh_addr_of(wide), 10, 3);
    lh_gl_size_t tall;

    lh_gl_size_init(lh_addr_of(tall), 3, 10);
    EXPECT_EQ(lh_gl_size_min_side(lh_addr_of(wide)), 3);
    EXPECT_EQ(lh_gl_size_min_side(lh_addr_of(tall)), 3);
}

TEST(gl_size, inset_shrinks_both_sides)
{
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 10, 8);
    const lh_gl_size_t shrunk = lh_test::out(lh_gl_size_inset, lh_addr_of(size), 2, 1);
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(shrunk)), 6);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(shrunk)), 6);
}

TEST(gl_size_initializer, fills_width_then_height)
{
    const lh_gl_size_t value = lh_gl_size_initializer(3, 4);

    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(value)), 3);
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(value)), 4);
}

} // namespace