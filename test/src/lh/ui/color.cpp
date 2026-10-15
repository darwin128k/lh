#include <gtest/gtest.h>

#include <lh/ui/color.h>
#include <lh/util/addr.h>
#include <lh/test/out.h>

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
    out = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(src));
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
    out = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(src));
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
    out = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(src));
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
    out = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(src));
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
    out = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(src));
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
                const lh_ui_color_t fast = lh_test::out(lh_ui_color_over_opaque, lh_addr_of(dst), lh_addr_of(src));
                const lh_ui_color_t slow = lh_test::out(lh_ui_color_over_translucent, lh_addr_of(dst), lh_addr_of(src));
                ASSERT_TRUE(lh_ui_color_equals(lh_addr_of(fast), lh_addr_of(slow))) << a << " " << s << " " << d;
            }
        }
    }
}

TEST(ui_color, rgb565_packs_the_top_bits_and_widens_back)
{
    lh_ui_color_t c;
    lh_ui_color_t back;

    lh_ui_color_init(lh_addr_of(c), 0xFF, 0x80, 0x07, 0x10);
    EXPECT_EQ(lh_ui_color_get_rgb565(lh_addr_of(c)), (0x1Fu << 11) | (0x20u << 5) | 0x00u);
    lh_ui_color_init_rgb565(lh_addr_of(back), 0xFFFFu);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(back)), 255);
    lh_ui_color_init_rgb565(lh_addr_of(back), 0u);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(back)), 0);
    /* Every 565 word survives unpack + pack. */
    for (lh_u32_t w = 0; w <= 0xFFFFu; ++w)
    {
        lh_ui_color_init_rgb565(lh_addr_of(back), w);
        ASSERT_EQ(lh_ui_color_get_rgb565(lh_addr_of(back)), w);
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

/* The scalar formula every word path has to reproduce bit for bit. */
lh_u32_t
over_channel_reference(lh_u32_t src, lh_u32_t dst, lh_u32_t a)
{
    return (src * a + dst * (255u - a) + 127u) / 255u;
}

TEST(ui_color, div255_round_is_the_division_for_every_numerator)
{
    for (lh_u32_t x = 0u; x <= 255u * 255u; ++x)
    {
        ASSERT_EQ(lh_ui_color_div255_round(x), (x + 127u) / 255u) << x;
    }
}

TEST(ui_color, div255_round_pair_divides_each_lane_on_its_own)
{
    const lh_u32_t others[] = {0u, 1u, 127u, 128u, 255u * 255u};

    for (lh_u32_t x = 0u; x <= 255u * 255u; ++x)
    {
        for (lh_u32_t y : others)
        {
            const lh_u32_t got = lh_ui_color_div255_round_pair(x | (y << 16));

            ASSERT_EQ(got & 0xFFFFu, (x + 127u) / 255u) << x << " beside " << y;
            ASSERT_EQ(got >> 16, (y + 127u) / 255u) << y << " beside " << x;
        }
    }
}

TEST(ui_color, dual_mac_is_two_products_and_the_accumulator)
{
    EXPECT_EQ(lh_ui_color_dual_mac(0u, 0u, 7u), 7u);
    EXPECT_EQ(lh_ui_color_dual_mac(3u | (5u << 16), 7u | (11u << 16), 2u), 2u + 3u * 7u + 5u * 11u);
    EXPECT_EQ(lh_ui_color_dual_mac(255u | (255u << 16), 255u | (255u << 16), 127u), 2u * 255u * 255u + 127u);
}

TEST(ui_color, mix_pair_is_the_channel_formula_in_both_lanes)
{
    for (lh_u32_t a = 0u; a <= 255u; ++a)
    {
        for (lh_u32_t s = 0u; s <= 255u; ++s)
        {
            for (lh_u32_t d = 0u; d <= 255u; ++d)
            {
                const lh_u32_t other = (s * 7u + d * 13u + a) & 0xFFu;
                const lh_u32_t got = lh_ui_color_mix_pair(d | (other << 16), s | ((255u - other) << 16), a);

                ASSERT_EQ(got & 0xFFFFu, over_channel_reference(s, d, a)) << s << " " << d << " " << a;
                ASSERT_EQ(got >> 16, over_channel_reference(255u - other, other, a)) << other << " " << a;
            }
        }
    }
}

TEST(ui_color, over_opaque_channel_is_the_formula_for_every_input)
{
    for (lh_u32_t a = 0u; a <= 255u; ++a)
    {
        for (lh_u32_t s = 0u; s <= 255u; ++s)
        {
            for (lh_u32_t d = 0u; d <= 255u; ++d)
            {
                ASSERT_EQ(lh_ui_color_over_opaque_channel(s, d, a), over_channel_reference(s, d, a));
            }
        }
    }
}

TEST(ui_color, argb_over_opaque_is_the_channel_formula_word_for_word)
{
    lh_u32_t seed = 12345u;

    for (lh_u32_t a = 0u; a <= 255u; ++a)
    {
        for (int i = 0; i < 4096; ++i)
        {
            seed = seed * 1664525u + 1013904223u;
            const lh_u32_t dst = 0xFF000000u | (seed >> 8);
            seed = seed * 1664525u + 1013904223u;
            const lh_u32_t src = (a << 24) | (seed & 0x00FFFFFFu);
            const lh_u32_t want = 0xFF000000u |
                                  (over_channel_reference((src >> 16) & 0xFFu, (dst >> 16) & 0xFFu, a) << 16) |
                                  (over_channel_reference((src >> 8) & 0xFFu, (dst >> 8) & 0xFFu, a) << 8) |
                                  over_channel_reference(src & 0xFFu, dst & 0xFFu, a);

            ASSERT_EQ(lh_ui_color_argb_over_opaque(dst, src), want) << std::hex << dst << " " << src;
        }
    }
    /* The ends: alpha 255 is the source, alpha 0 the destination. */
    EXPECT_EQ(lh_ui_color_argb_over_opaque(0xFF123456u, 0xFFABCDEFu), 0xFFABCDEFu);
    EXPECT_EQ(lh_ui_color_argb_over_opaque(0xFF123456u, 0x00ABCDEFu), 0xFF123456u);
}

} // namespace
