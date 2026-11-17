/**
 * @file size.cpp
 * @brief Tests for `lh/gx/size.h`.
 *
 * The gx size is the math size under the name of this part. What is checked
 * here is that the name reaches the plane gx draws on: `width` and `height`,
 * and an empty size for "nothing".
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gx/scalar.h>
#include <lh/gx/size.h>
#include <lh/test/out.h>

namespace
{

TEST(gx_size, width_and_height_are_the_extent)
{
    lh_gx_size_t size;

    lh_gx_size_init(lh_addr_of(size), lh_gx_scalar(320), lh_gx_scalar(240));

    EXPECT_EQ(lh_gx_size_get_width(lh_addr_of(size)), lh_gx_scalar(320));
    EXPECT_EQ(lh_gx_size_get_height(lh_addr_of(size)), lh_gx_scalar(240));
}

TEST(gx_size, an_empty_size_is_nothing)
{
    lh_gx_size_t size;

    lh_gx_size_init_empty(lh_addr_of(size));

    EXPECT_EQ(lh_gx_size_is_empty(lh_addr_of(size)), lh_bool_true);
    EXPECT_EQ(lh_gx_size_get_width(lh_addr_of(size)), lh_gx_scalar(0));
}

TEST(gx_size, a_size_is_a_value_the_caller_owns)
{
    lh_gx_size_t first;

    lh_gx_size_init(lh_addr_of(first), lh_gx_scalar(10), lh_gx_scalar(20));

    lh_gx_size_t second = first;

    lh_gx_size_set_width(lh_addr_of(first), lh_gx_scalar(99));

    EXPECT_EQ(lh_gx_size_get_width(lh_addr_of(second)), lh_gx_scalar(10));
    EXPECT_EQ(lh_gx_size_equals(lh_addr_of(first), lh_addr_of(second)), lh_bool_false);
}

} // namespace
