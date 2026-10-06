#include <gtest/gtest.h>

#include <lh/ui/color.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_color, make_holds_channels)
{
    lh_ui_color_t c;

    lh_ui_color_init(lh_addr_of(c), 1, 2, 3, 4);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(c)), 1);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(c)), 2);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(c)), 3);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(c)), 4);
}

TEST(ui_color, make_hex_is_rrggbbaa)
{
    lh_ui_color_t c;

    lh_ui_color_init_hex(lh_addr_of(c), 0x11558880u);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(c)), 0x11);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(c)), 0x55);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(c)), 0x88);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(c)), 0x80);
}

TEST(ui_color, init_holds_channels)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), 1, 2, 3, 4);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(c)), 1);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(c)), 2);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(c)), 3);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(c)), 4);
}

TEST(ui_color, init_hex_is_rrggbbaa)
{
    lh_ui_color_t c;
    lh_ui_color_init_hex(lh_addr_of(c), 0xAABBCCFFU);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(c)), 0xAA);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(c)), 0xBB);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(c)), 0xCC);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(c)), 0xFF);
}

TEST(ui_color, setters)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), 0, 0, 0, 0);
    lh_ui_color_set_r(lh_addr_of(c), 10);
    lh_ui_color_set_g(lh_addr_of(c), 20);
    lh_ui_color_set_b(lh_addr_of(c), 30);
    lh_ui_color_set_a(lh_addr_of(c), 40);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(c)), 10);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(c)), 20);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(c)), 30);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(c)), 40);
}

TEST(ui_color, over_opaque_replaces_dst)
{
    lh_ui_color_t dst;
    lh_ui_color_t src;
    lh_ui_color_t out;
    lh_ui_color_init(lh_addr_of(dst), 10, 20, 30, 255);
    lh_ui_color_init(lh_addr_of(src), 100, 110, 120, 255);
    out = lh_ui_color_over(lh_addr_of(dst), lh_addr_of(src));
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(out)), 100);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(out)), 110);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(out)), 120);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(out)), 255);
}

TEST(ui_color, over_transparent_keeps_dst)
{
    lh_ui_color_t dst;
    lh_ui_color_t src;
    lh_ui_color_t out;
    lh_ui_color_init(lh_addr_of(dst), 10, 20, 30, 255);
    lh_ui_color_init(lh_addr_of(src), 100, 110, 120, 0);
    out = lh_ui_color_over(lh_addr_of(dst), lh_addr_of(src));
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(out)), 10);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(out)), 20);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(out)), 30);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(out)), 255);
}

TEST(ui_color, equals_compares_channels)
{
    lh_ui_color_t a;
    lh_ui_color_t b;

    lh_ui_color_init(lh_addr_of(a), 1, 2, 3, 4);
    lh_ui_color_init(lh_addr_of(b), 1, 2, 3, 4);
    EXPECT_EQ(lh_ui_color_equals(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    lh_ui_color_set_a(lh_addr_of(b), 5);
    EXPECT_EQ(lh_ui_color_equals(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
}

} // namespace
