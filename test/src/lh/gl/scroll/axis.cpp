#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/scroll/axis.h>

TEST(gl_scroll_axis, init_stores_one_offset_and_add_keeps_it)
{
    lh_gl_scroll_axis_t axis;

    lh_gl_scroll_axis_init(lh_addr_of(axis), 3);
    EXPECT_EQ(lh_gl_scroll_axis_get_offset(lh_addr_of(axis)), 3);
    lh_gl_scroll_axis_add(lh_addr_of(axis), 4);
    EXPECT_EQ(lh_gl_scroll_axis_get_offset(lh_addr_of(axis)), 7);
    lh_gl_scroll_axis_add(lh_addr_of(axis), -2);
    EXPECT_EQ(lh_gl_scroll_axis_get_offset(lh_addr_of(axis)), 5);
    lh_gl_scroll_axis_set_offset(lh_addr_of(axis), 0);
    EXPECT_EQ(lh_gl_scroll_axis_get_offset(lh_addr_of(axis)), 0);
}
