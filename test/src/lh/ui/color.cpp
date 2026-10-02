#include <gtest/gtest.h>

#include <lh/ui/color.h>

namespace
{

TEST(ui_color, argb_round_trip)
{
    const lh_ui_color_t c = lh_ui_color_from_argb(0x80112233u);
    EXPECT_EQ(c.a, 0x80);
    EXPECT_EQ(c.r, 0x11);
    EXPECT_EQ(c.g, 0x22);
    EXPECT_EQ(c.b, 0x33);
    EXPECT_EQ(lh_ui_color_to_argb(c), 0x80112233u);
    EXPECT_EQ(lh_ui_color_to_argb(lh_ui_color_make(1, 2, 3, 4)), 0x04010203u);
}

} // namespace
