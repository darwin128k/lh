#include <gtest/gtest.h>

#include <lh/ui/color.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_color, init_hex_keeps_a_translucent_alpha)
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

TEST(ui_color, over_translucent_dst_divides_by_out_alpha)
{
    lh_ui_color_t dst;
    lh_ui_color_t src;
    lh_ui_color_t out;
    lh_ui_color_init(lh_addr_of(dst), 200, 0, 0, 128);
    lh_ui_color_init(lh_addr_of(src), 0, 0, 200, 128);
    out = lh_ui_color_over(lh_addr_of(dst), lh_addr_of(src));
    /* out_a = 0.502 + 0.502 * 0.498 = 0.752; r = 200 * 0.25 / 0.752; b = 200 * 0.502 / 0.752. */
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(out)), 66);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(out)), 0);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(out)), 134);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(out)), 192);
}

TEST(ui_color, over_opaque_on_transparent_dst_is_src)
{
    lh_ui_color_t dst;
    lh_ui_color_t src;
    lh_ui_color_t out;
    lh_ui_color_init(lh_addr_of(dst), 10, 20, 30, 0);
    lh_ui_color_init(lh_addr_of(src), 100, 110, 120, 77);
    out = lh_ui_color_over(lh_addr_of(dst), lh_addr_of(src));
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(out), lh_addr_of(src)));
}

TEST(ui_color, over_two_transparent_is_zero)
{
    lh_ui_color_t dst;
    lh_ui_color_t src;
    lh_ui_color_t out;
    lh_ui_color_t zero;
    lh_ui_color_init(lh_addr_of(dst), 10, 20, 30, 0);
    lh_ui_color_init(lh_addr_of(src), 100, 110, 120, 0);
    lh_ui_color_init(lh_addr_of(zero), 0, 0, 0, 0);
    out = lh_ui_color_over(lh_addr_of(dst), lh_addr_of(src));
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(out), lh_addr_of(zero)));
}

TEST(ui_color, argb_is_the_pixel_word_and_round_trips)
{
    lh_ui_color_t c;
    lh_ui_color_t back;

    lh_ui_color_init(lh_addr_of(c), 0x11, 0x22, 0x33, 0x44);
    EXPECT_EQ(lh_ui_color_get_argb(lh_addr_of(c)), 0x44112233u);
    lh_ui_color_init_argb(lh_addr_of(back), 0x44112233u);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(back), lh_addr_of(c)));
}

/* The fast path must not change a single pixel: every alpha, a grid of channels. */
TEST(ui_color, over_an_opaque_dst_matches_the_general_formula)
{
    for (int a = 0; a <= 255; ++a)
    {
        for (int s = 0; s <= 255; s += 15)
        {
            for (int d = 0; d <= 255; d += 15)
            {
                lh_ui_color_t dst;
                lh_ui_color_t src;
                lh_ui_color_init(lh_addr_of(dst), static_cast<lh_u8_t>(d), static_cast<lh_u8_t>(255 - d),
                                 static_cast<lh_u8_t>(d / 2), 255);
                lh_ui_color_init(lh_addr_of(src), static_cast<lh_u8_t>(s), static_cast<lh_u8_t>(s / 3),
                                 static_cast<lh_u8_t>(255 - s), static_cast<lh_u8_t>(a));
                const lh_ui_color_t fast = lh_ui_color_over_opaque(lh_addr_of(dst), lh_addr_of(src));
                const lh_ui_color_t slow = lh_ui_color_over_translucent(lh_addr_of(dst), lh_addr_of(src));
                ASSERT_TRUE(lh_ui_color_equals(lh_addr_of(fast), lh_addr_of(slow))) << a << " " << s << " " << d;
            }
        }
    }
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
