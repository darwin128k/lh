#include <gtest/gtest.h>
#include <vector>

#include <lh/cpu/simd.h>
#include <lh/expect/death.h>
#include <lh/ui/pixmap.h>
#include <lh/test/out.h>

namespace
{

/* width x 3 pixels of @p format, rows one spare pixel longer. */
struct pixmap_fixture
{
    static const int capacity = 3 * 41;
    lh_u32_t words[capacity]; /* u32 storage keeps any format aligned */
    lh_ui_pixmap_t pixmap;
    int width;
    int bytes;

    pixmap_fixture(lh_u32_t fill, lh_ui_pixmap_format_t format = lh_ui_pixmap_format_argb8888, int w = 4)
        : width(w), bytes(lh_ui_pixmap_format_get_bytes(format))
    {
        for (lh_u32_t &word : words)
        {
            word = fill;
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), w, 3, (w + 1) * bytes, format);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return lh_ui_pixmap_read_word(lh_addr_of(pixmap), x, y);
    }

    /* The spare pixel past the width of row @p y. */
    lh_u32_t
    spare(int y) const
    {
        return at(width, y);
    }
};

lh_ui_color_t
color_of(int r, int g, int b, int a)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), static_cast<lh_u8_t>(r), static_cast<lh_u8_t>(g), static_cast<lh_u8_t>(b),
                     static_cast<lh_u8_t>(a));
    return c;
}

} // namespace

TEST(ui_pixmap, init_keeps_shape_and_rows_follow_the_stride)
{
    pixmap_fixture f(0u);

    EXPECT_EQ(lh_ui_pixmap_get_width(lh_addr_of(f.pixmap)), 4);
    EXPECT_EQ(lh_ui_pixmap_get_height(lh_addr_of(f.pixmap)), 3);
    EXPECT_EQ(lh_ui_pixmap_get_format(lh_addr_of(f.pixmap)), lh_ui_pixmap_format_argb8888);
    EXPECT_EQ(lh_ui_pixmap_get_row(lh_addr_of(f.pixmap), 2), lh_ptr_rcast(lh_byte_t, f.words) + 2 * 20);
    EXPECT_EQ(lh_ui_pixmap_get_address(lh_addr_of(f.pixmap), 3, 1), lh_ptr_rcast(lh_byte_t, f.words) + 20 + 12);
    const lh_ui_rect_t bounds = lh_test::out(lh_ui_pixmap_get_bounds, lh_addr_of(f.pixmap));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 4);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 3);
}

TEST(ui_pixmap, format_sizes)
{
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_argb8888), 4);
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_rgb565), 2);
}

TEST(ui_pixmap, set_and_get_pixel_round_trip_through_argb)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(1, 2, 3, 4);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 3, 1, lh_addr_of(c));
    EXPECT_EQ(f.at(3, 1), 0x04010203u);
    const lh_ui_color_t back = lh_test::out(lh_ui_pixmap_get_pixel, lh_addr_of(f.pixmap), 3, 1);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(back), lh_addr_of(c)));
}

TEST(ui_pixmap, rgb565_stores_16_bit_words_and_reads_opaque)
{
    pixmap_fixture f(0u, lh_ui_pixmap_format_rgb565);
    const lh_ui_color_t c = color_of(255, 0, 255, 77);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 1, 0, lh_addr_of(c));
    EXPECT_EQ(f.at(1, 0), 0xF81Fu);
    /* Only the two bytes of that pixel changed. */
    EXPECT_EQ(f.at(0, 0), 0u);
    EXPECT_EQ(f.at(2, 0), 0u);
    const lh_ui_color_t back = lh_test::out(lh_ui_pixmap_get_pixel, lh_addr_of(f.pixmap), 1, 0);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(back)), 0);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(back)), 255);
}

TEST(ui_pixmap, rgb565_swapped_puts_the_high_byte_first_in_memory)
{
    pixmap_fixture f(0u, lh_ui_pixmap_format_rgb565_swapped);
    const lh_ui_color_t c = color_of(255, 0, 0, 255); /* 0xF800 */
    const lh_byte_t *bytes = lh_ptr_rcast(const lh_byte_t, f.words);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c));
    EXPECT_EQ(bytes[0], 0xF8);
    EXPECT_EQ(bytes[1], 0x00);
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_rgb565_swapped), 2);
    const lh_ui_color_t back = lh_test::out(lh_ui_pixmap_get_pixel, lh_addr_of(f.pixmap), 0, 0);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(back)), 0);
}

TEST(ui_pixmap, fill_span_stores_an_opaque_color_and_leaves_the_rest)
{
    pixmap_fixture f(0x11111111u);
    const lh_ui_color_t c = color_of(10, 20, 30, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 1, 3, 2, lh_addr_of(c));
    EXPECT_EQ(f.at(0, 2), 0x11111111u);
    EXPECT_EQ(f.at(1, 2), 0xff0a141eu);
    EXPECT_EQ(f.at(2, 2), 0xff0a141eu);
    EXPECT_EQ(f.at(3, 2), 0x11111111u);
}

/* Spans of every length (unrolled and tail): every pixel, and not one more. */
TEST(ui_pixmap, long_store_span_fills_exactly_its_pixels_in_both_formats)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_argb8888, lh_ui_pixmap_format_rgb565,
                                             lh_ui_pixmap_format_rgb565_swapped};
    for (lh_ui_pixmap_format_t format : formats)
    {
        for (int x1 = 3; x1 <= 40; ++x1)
        {
            pixmap_fixture f(0u, format, 40);
            lh_ui_pixmap_store_span(lh_addr_of(f.pixmap), 2, x1, 1, 0xABCDu);
            for (int x = 0; x <= 40; ++x)
            {
                const lh_u32_t want = x >= 2 && x < x1 ? 0xABCDu : 0u;
                ASSERT_EQ(f.at(x, 1), want) << format << " " << x1 << " " << x;
            }
            ASSERT_EQ(f.at(0, 0) | f.at(39, 0) | f.at(0, 2) | f.at(39, 2), 0u);
        }
    }
}

TEST(ui_pixmap, fill_span_blends_a_translucent_color)
{
    pixmap_fixture f(0xff000000u);
    const lh_ui_color_t c = color_of(200, 100, 0, 128);
    const lh_ui_color_t black = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 0, 1, 0, lh_addr_of(c));
    const lh_ui_color_t want = lh_test::out(lh_ui_color_over, lh_addr_of(black), lh_addr_of(c));
    const lh_ui_color_t got = lh_test::out(lh_ui_pixmap_get_pixel, lh_addr_of(f.pixmap), 0, 0);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(got), lh_addr_of(want)));
}

TEST(ui_pixmap, rgb565_blend_works_on_the_unpacked_pixel)
{
    pixmap_fixture f(0u, lh_ui_pixmap_format_rgb565);
    const lh_ui_color_t white = color_of(255, 255, 255, 255);
    const lh_ui_color_t half = color_of(0, 0, 0, 128);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(white));
    lh_ui_pixmap_blend_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(half));
    const lh_ui_color_t over = lh_test::out(lh_ui_color_over, lh_addr_of(white), lh_addr_of(half));
    EXPECT_EQ(f.at(0, 0), lh_ui_color_get_rgb565(lh_addr_of(over)));
}

TEST(ui_pixmap, cover_pixel_scales_alpha_skips_zero_and_stores_full)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(9, 8, 7, 255);

    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 0);
    EXPECT_EQ(f.at(0, 0), 0u);
    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 100);
    const lh_ui_color_t got = lh_test::out(lh_ui_pixmap_get_pixel, lh_addr_of(f.pixmap), 0, 0);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(got)), 100);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(got)), 9);
    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 1, 0, lh_addr_of(c), 255);
    EXPECT_EQ(f.at(1, 0), 0xff090807u);
}

TEST(ui_pixmap, fill_box_and_clear_stay_inside_the_width)
{
    pixmap_fixture f(0x12345678u, lh_ui_pixmap_format_argb8888, 20);
    const lh_ui_color_t c = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_box(lh_addr_of(f.pixmap), 1, 1, 19, 3, lh_addr_of(c));
    EXPECT_EQ(f.at(1, 1), 0xff000000u);
    EXPECT_EQ(f.at(18, 2), 0xff000000u);
    EXPECT_EQ(f.at(19, 2), 0x12345678u);
    EXPECT_EQ(f.at(1, 0), 0x12345678u);

    lh_ui_pixmap_clear(lh_addr_of(f.pixmap), lh_addr_of(c));
    EXPECT_EQ(f.at(19, 2), 0xff000000u);
    EXPECT_EQ(f.at(0, 0), 0xff000000u);
    EXPECT_EQ(f.spare(0), 0x12345678u);
    EXPECT_EQ(f.spare(2), 0x12345678u);
}

TEST(ui_pixmap, translucent_box_blends_every_row)
{
    pixmap_fixture f(0xff000000u, lh_ui_pixmap_format_argb8888, 20);
    const lh_ui_color_t c = color_of(255, 255, 255, 128);

    lh_ui_pixmap_fill_box(lh_addr_of(f.pixmap), 0, 0, 20, 3, lh_addr_of(c));
    EXPECT_EQ(f.at(0, 0), f.at(19, 2));
    EXPECT_NE(f.at(0, 0), 0xff000000u);
    EXPECT_NE(f.at(0, 0), 0xffffffffu);
}

/* fill_colors_span walks a gradient pixel by pixel through lh_ui_pixmap_over_word;
   each pixel has to come out exactly as a one-pixel fill_span of its colour would
   leave it, in every format, for opaque, translucent and transparent colours, and
   (ARGB) over a translucent destination too. */
TEST(ui_pixmap, fill_colors_span_is_a_fill_span_per_pixel_in_every_format)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_argb8888, lh_ui_pixmap_format_rgb565,
                                             lh_ui_pixmap_format_rgb565_swapped};
    const int count = 64;

    for (lh_ui_pixmap_format_t format : formats)
    {
        const bool wide = format == lh_ui_pixmap_format_argb8888;
        std::vector<lh_u32_t> got(count);
        std::vector<lh_u32_t> want(count);
        std::vector<lh_ui_color_t> colors(count);
        lh_ui_pixmap_t a;
        lh_ui_pixmap_t b;

        for (int i = 0; i < count; ++i)
        {
            const lh_u32_t under = (wide ? (static_cast<lh_u32_t>(i * 37) & 0xFFu) << 24 : 0u) |
                                   (static_cast<lh_u32_t>(i) * 0x0A0B0Cu & 0x00FFFFFFu);
            got[i] = wide ? under : (under & 0xFFFFu);
            want[i] = got[i];
            lh_ui_color_init(lh_addr_of(colors[i]), static_cast<lh_ui_color_channel_t>(i * 4),
                             static_cast<lh_ui_color_channel_t>(255 - i * 3), static_cast<lh_ui_color_channel_t>(i * 7),
                             static_cast<lh_ui_color_channel_t>(i % 4 == 0 ? 0 : (i % 4 == 1 ? 255 : i * 4)));
        }
        if (!wide)
        {
            /* 16-bit words packed two to a lane of the vectors above. */
            std::vector<lh_u16_t> g16(count);
            std::vector<lh_u16_t> w16(count);
            for (int i = 0; i < count; ++i)
            {
                g16[i] = static_cast<lh_u16_t>(got[i]);
                w16[i] = g16[i];
            }
            lh_ui_pixmap_init(lh_addr_of(a), lh_ptr_rcast(lh_byte_t, g16.data()), count, 1, count * 2, format);
            lh_ui_pixmap_init(lh_addr_of(b), lh_ptr_rcast(lh_byte_t, w16.data()), count, 1, count * 2, format);
            lh_ui_pixmap_fill_colors_span(lh_addr_of(a), 0, count, 0, colors.data());
            for (int i = 0; i < count; ++i)
            {
                lh_ui_pixmap_fill_span(lh_addr_of(b), i, i + 1, 0, lh_addr_of(colors[i]));
            }
            for (int i = 0; i < count; ++i)
            {
                ASSERT_EQ(g16[i], w16[i]) << "format " << static_cast<int>(format) << " pixel " << i;
            }
            continue;
        }
        lh_ui_pixmap_init(lh_addr_of(a), lh_ptr_rcast(lh_byte_t, got.data()), count, 1, count * 4, format);
        lh_ui_pixmap_init(lh_addr_of(b), lh_ptr_rcast(lh_byte_t, want.data()), count, 1, count * 4, format);
        lh_ui_pixmap_fill_colors_span(lh_addr_of(a), 0, count, 0, colors.data());
        for (int i = 0; i < count; ++i)
        {
            lh_ui_pixmap_fill_span(lh_addr_of(b), i, i + 1, 0, lh_addr_of(colors[i]));
        }
        for (int i = 0; i < count; ++i)
        {
            ASSERT_EQ(got[i], want[i]) << "argb pixel " << i;
        }
    }
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

namespace
{

/* What the scalar kernel owes the caller, written out per pixel from the
   public channel formula rather than by calling the kernel under test: an
   alpha of 0 skips the pixel, 255 stores the source, anything else is
   lh_ui_pixmap_mix_rgb over an opaque destination. */
lh_u32_t
blend_reference(lh_u32_t dst, lh_u32_t src, lh_byte_t alpha)
{
    if (alpha == 0u)
    {
        return dst;
    }
    if (alpha == 255u)
    {
        return 0xff000000u | src;
    }
    return 0xff000000u | lh_ui_pixmap_mix_rgb(dst, src, alpha);
}

} // namespace

/* The SIMD kernel trades three per-pixel branches for one unconditional
   formula, so the thing worth pinning is that it lands on exactly the same
   word as the scalar kernel for every alpha there is — including 0 and 255,
   which the formula reproduces rather than special-cases. Every run length up
   to 40 covers the 4-pixel loop, its tail, and both together. */
TEST(ui_pixmap, blend_alpha_32_sse2_is_the_scalar_formula_for_every_alpha_and_run_length)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t src = 0x0033ccffu;
    for (int count = 1; count <= 40; ++count)
    {
        lh_u32_t at[40];
        lh_u32_t want[40];
        lh_byte_t alpha[40];
        lh_ui_pixmap_t pixmap;
        /* Opaque destinations, so the kernel stays on its vector path. */
        for (int i = 0; i < count; ++i)
        {
            at[i] = 0xff000000u | static_cast<lh_u32_t>((i * 7u * 2654435761u) & 0x00ffffffu);
            alpha[i] = static_cast<lh_byte_t>(i);
            want[i] = blend_reference(at[i], src, alpha[i]);
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, at), count, 1,
                          count * static_cast<int>(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_argb8888)),
                          lh_ui_pixmap_format_argb8888);

        lh_ui_pixmap_blend_alpha_32_sse2(lh_addr_of(pixmap), at, alpha, static_cast<lh_usize_t>(count), src);
        for (int i = 0; i < count; ++i)
        {
            ASSERT_EQ(at[i], want[i]) << "count " << count << " pixel " << i << " alpha " << static_cast<int>(alpha[i]);
        }
    }
}

/* A translucent destination takes the general formula, with a divisor per pixel,
   so the kernel has to send a block holding one through
   lh_ui_pixmap_blend_alpha_32_over_sse2. The destination alpha walks the whole range including fully
   transparent, where a source alpha of zero must leave the pixel untouched —
   the one case the unconditional vector formula gets wrong, and the one a
   single mid-alpha hole would not catch. */
TEST(ui_pixmap, blend_alpha_32_sse2_falls_back_for_a_translucent_destination)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t src = 0x0077aa33u;
    const int count = 20;
    const lh_u32_t dest_alpha[] = {0x00000000u, 0x01000000u, 0x7f000000u, 0x80000000u, 0xfe000000u};
    for (lh_u32_t dst_alpha : dest_alpha)
    {
        for (int hole = 0; hole < count; ++hole)
        {
            lh_u32_t at[20];
            lh_u32_t want[20];
            lh_byte_t alpha[20];
            lh_ui_pixmap_t pixmap;
            for (int i = 0; i < count; ++i)
            {
                /* One pixel takes the destination alpha under test, the rest
                   stay opaque; the hole walks the run so every position inside
                   a group is hit. */
                const bool special_pixel = (i == hole);
                at[i] = (special_pixel ? dst_alpha : 0xff000000u) | static_cast<lh_u32_t>(i * 0x030507u);
                /* Every third pixel blends nothing at all. */
                alpha[i] = static_cast<lh_byte_t>((i % 3 == 0) ? 0u : i * 13u);
            }
            lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, at), count, 1, count * 4,
                              lh_ui_pixmap_format_argb8888);
            /* Reference through the public per-pixel blend, which is the contract. */
            for (int i = 0; i < count; ++i)
            {
                lh_ui_color_t dst;
                lh_ui_color_t color;
                lh_ui_color_t over;
                lh_ui_color_init_argb(lh_addr_of(dst), at[i]);
                lh_ui_color_init_argb(lh_addr_of(color), (static_cast<lh_u32_t>(alpha[i]) << 24) | src);
                over = lh_test::out(lh_ui_color_over, lh_addr_of(dst), lh_addr_of(color));
                want[i] = alpha[i] == 0u ? at[i] : lh_ui_pixmap_pack(lh_addr_of(pixmap), lh_addr_of(over));
            }
            lh_ui_pixmap_blend_alpha_32_sse2(lh_addr_of(pixmap), at, alpha, static_cast<lh_usize_t>(count), src);
            for (int i = 0; i < count; ++i)
            {
                ASSERT_EQ(at[i], want[i]) << "dst alpha " << static_cast<lh_u32_t>(dst_alpha >> 24) << " hole at "
                                          << hole << " pixel " << i;
            }
        }
    }
}

/* The general "over" divides in single precision, and is exact only because every
   numerator stays below 2^24 and every quotient that is not whole stays more than
   half an ulp from the next integer. The tightest cases are a large total (both
   alphas near 255) with channels near 255, and a tiny total with channels at the
   ends: every destination alpha and every source alpha, against the channel values
   that sit at those edges, each compared bit for bit with the scalar word form. */
TEST(ui_pixmap, blend_alpha_32_over_sse2_is_exact_for_every_alpha_pair)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t channels[] = {0u, 1u, 2u, 63u, 64u, 127u, 128u, 129u, 191u, 253u, 254u, 255u};
    const lh_usize_t n = sizeof(channels) / sizeof(channels[0]);
    std::vector<lh_u32_t> at(256u);
    std::vector<lh_u32_t> want(256u);
    std::vector<lh_byte_t> alpha(256u);

    for (lh_u32_t dst_a = 0u; dst_a < 256u; ++dst_a)
    {
        for (lh_usize_t si = 0u; si < n; ++si)
        {
            for (lh_usize_t di = 0u; di < n; ++di)
            {
                const lh_u32_t s = channels[si];
                const lh_u32_t d = channels[di];
                const lh_u32_t src = (s << 16) | (d << 8) | (255u - s);
                const lh_u32_t dst = (dst_a << 24) | (d << 16) | (s << 8) | (255u - d);

                for (lh_u32_t a = 0u; a < 256u; ++a)
                {
                    at[a] = dst;
                    alpha[a] = static_cast<lh_byte_t>(a);
                    want[a] = a == 0u ? dst : lh_ui_color_argb_over(dst, (a << 24) | src);
                }
                lh_ui_pixmap_blend_alpha_32_over_sse2(at.data(), alpha.data(), 256u, src);
                for (lh_u32_t a = 0u; a < 256u; ++a)
                {
                    ASSERT_EQ(at[a], want[a]) << "dst alpha " << dst_a << " src alpha " << a << " s " << s
                                              << " d " << d;
                }
            }
        }
    }
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* The 16-bit kernel is the MCU path, and the format that most easily goes
   quietly wrong is the swapped one: a missing byte swap still produces plausible
   pixels, just the wrong ones. Both formats, every alpha, every run length up to
   40 so the eight-pixel loop, its tail and both together are all covered. */
TEST(ui_pixmap, blend_alpha_16_sse2_is_the_scalar_kernel_for_every_alpha_in_both_orders)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t src = 0x0033ccffu;
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_rgb565, lh_ui_pixmap_format_rgb565_swapped};
    for (lh_ui_pixmap_format_t format : formats)
    {
        for (int count = 1; count <= 40; ++count)
        {
            lh_u16_t at[40];
            lh_u16_t want[40];
            lh_byte_t alpha[40];
            lh_ui_pixmap_t pixmap;
            lh_u32_t solid;

            lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, at), count, 1, count * 2, format);
            solid = lh_ui_pixmap_order_16(lh_addr_of(pixmap), lh_ui_pixmap_pack_rgb(lh_addr_of(pixmap), src));
            for (int i = 0; i < count; ++i)
            {
                at[i] = static_cast<lh_u16_t>((i * 2654435761u) & 0xffffu);
                alpha[i] = static_cast<lh_byte_t>(i);
                want[i] = alpha[i] == 0u
                              ? at[i]
                              : (alpha[i] == 255u ? static_cast<lh_u16_t>(solid)
                                                   : static_cast<lh_u16_t>(lh_ui_pixmap_mix_16(lh_addr_of(pixmap), at[i],
                                                                                             src, alpha[i])));
            }
            lh_ui_pixmap_blend_alpha_16_sse2(lh_addr_of(pixmap), at, alpha, static_cast<lh_usize_t>(count), src);
            for (int i = 0; i < count; ++i)
            {
                ASSERT_EQ(at[i], want[i]) << "format " << static_cast<int>(format) << " count " << count
                                          << " pixel " << i << " alpha " << static_cast<int>(alpha[i]);
            }
        }
    }
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

/* Eight coverages from a register are the same blend as the same coverages from
   a buffer, for every count, in every format, over opaque and translucent ARGB. */
TEST(ui_pixmap, blend_cover_8_is_blend_alpha_span_for_every_count_and_format)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_argb8888, lh_ui_pixmap_format_rgb565,
                                             lh_ui_pixmap_format_rgb565_swapped};
    const lh_u32_t src = 0x0033ccffu;

    for (lh_ui_pixmap_format_t format : formats)
    {
        for (int translucent = 0; translucent < 2; ++translucent)
        {
            for (int count = 1; count <= 8; ++count)
            {
                for (int seed = 0; seed < 64; ++seed)
                {
                    lh_u32_t got[12];
                    lh_u32_t want[12];
                    lh_byte_t alpha[8];
                    lh_u64_t cover = 0u;
                    lh_ui_pixmap_t a;
                    lh_ui_pixmap_t b;
                    const int bytes = lh_ui_pixmap_format_get_bytes(format);

                    for (int i = 0; i < 12; ++i)
                    {
                        const lh_u32_t word = (static_cast<lh_u32_t>(i + seed * 12) * 2654435761u) ^ 0x5a5a5a5au;

                        got[i] = want[i] = translucent != 0 ? word : (word | 0xff000000u);
                    }
                    for (int i = 0; i < 8; ++i)
                    {
                        const int pick = (seed * 7 + i * 29) % 4;

                        alpha[i] = static_cast<lh_byte_t>(pick == 0 ? 0 : (pick == 1 ? 255 : (seed * 31 + i * 97) & 0xff));
                        cover |= static_cast<lh_u64_t>(alpha[i]) << (8 * i);
                    }
                    lh_ui_pixmap_init(lh_addr_of(a), lh_ptr_rcast(lh_byte_t, got), 12 * 4 / bytes, 1, 48, format);
                    lh_ui_pixmap_init(lh_addr_of(b), lh_ptr_rcast(lh_byte_t, want), 12 * 4 / bytes, 1, 48, format);
                    lh_ui_pixmap_blend_cover_8(lh_addr_of(a), 2, 0, count, cover, src);
                    lh_ui_pixmap_blend_alpha_span(lh_addr_of(b), 2, 2 + count, 0, src, alpha);
                    for (int i = 0; i < 12; ++i)
                    {
                        ASSERT_EQ(got[i], want[i]) << "format " << static_cast<int>(format) << " translucent "
                                                   << translucent << " count " << count << " seed " << seed
                                                   << " word " << i;
                    }
                }
            }
        }
    }
}

namespace
{

/* The RGB565 blend as it was before the word path: widen to 888, blend each
   channel with the scalar formula, truncate back. Kept here as the reference. */
lh_u32_t
mix_16_reference(lh_u32_t word, lh_u32_t src, lh_u32_t a)
{
    const lh_u32_t r5 = (word >> 11) & 0x1Fu;
    const lh_u32_t g6 = (word >> 5) & 0x3Fu;
    const lh_u32_t b5 = word & 0x1Fu;
    const lh_u32_t r = ((src >> 16) & 0xFFu) * a + ((r5 << 3) | (r5 >> 2)) * (255u - a) + 127u;
    const lh_u32_t g = ((src >> 8) & 0xFFu) * a + ((g6 << 2) | (g6 >> 4)) * (255u - a) + 127u;
    const lh_u32_t b = (src & 0xFFu) * a + ((b5 << 3) | (b5 >> 2)) * (255u - a) + 127u;

    return ((r / 255u) >> 3) << 11 | ((g / 255u) >> 2) << 5 | ((b / 255u) >> 3);
}

lh_u32_t
swap16(lh_u32_t w)
{
    return ((w & 0xFFu) << 8) | ((w >> 8) & 0xFFu);
}

TEST(ui_pixmap, the_565_lanes_widen_and_narrow_like_the_888_path)
{
    for (lh_u32_t w = 0u; w <= 0xFFFFu; ++w)
    {
        const lh_u32_t r5 = (w >> 11) & 0x1Fu;
        const lh_u32_t g6 = (w >> 5) & 0x3Fu;
        const lh_u32_t b5 = w & 0x1Fu;
        const lh_u32_t rb = lh_ui_pixmap_rb_of_565(w);

        ASSERT_EQ(rb, (((r5 << 3) | (r5 >> 2)) << 16) | ((b5 << 3) | (b5 >> 2))) << w;
        ASSERT_EQ(lh_ui_pixmap_g_of_565(w), (g6 << 2) | (g6 >> 4)) << w;
        ASSERT_EQ(lh_ui_pixmap_565_of(rb, lh_ui_pixmap_g_of_565(w)), w) << w;
    }
}

TEST(ui_pixmap, mix_16_is_the_888_path_bit_for_bit_in_both_byte_orders)
{
    const lh_u32_t sources[] = {0x000000u, 0xFFFFFFu, 0x0033CCFFu, 0x00F08040u, 0x00010203u, 0x007F807Fu};
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_rgb565, lh_ui_pixmap_format_rgb565_swapped};
    lh_u16_t word = 0;

    for (lh_ui_pixmap_format_t format : formats)
    {
        lh_ui_pixmap_t pixmap;

        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, &word), 1, 1, 2, format);
        for (lh_u32_t src : sources)
        {
            for (lh_u32_t a = 0u; a <= 255u; ++a)
            {
                for (lh_u32_t w = 0u; w <= 0xFFFFu; w += (a & 1u) != 0u ? 7u : 1u)
                {
                    const lh_u32_t stored = format == lh_ui_pixmap_format_rgb565 ? w : swap16(w);
                    const lh_u32_t want = mix_16_reference(w, src, a);

                    ASSERT_EQ(lh_ui_pixmap_mix_16(lh_addr_of(pixmap), stored, src, a),
                              format == lh_ui_pixmap_format_rgb565 ? want : swap16(want))
                        << "format " << static_cast<int>(format) << " word " << w << " src " << src << " a " << a;
                }
            }
        }
    }
}

TEST(ui_pixmap, blend_16_one_keeps_the_ends_exact)
{
    lh_u16_t word = 0;
    lh_ui_pixmap_t pixmap;

    lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, &word), 1, 1, 2, lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_ui_pixmap_blend_16_one(lh_addr_of(pixmap), 0x1234u, 0x0033CCFFu, 0xABCDu, 0u), 0x1234u);
    EXPECT_EQ(lh_ui_pixmap_blend_16_one(lh_addr_of(pixmap), 0x1234u, 0x0033CCFFu, 0xABCDu, 255u), 0xABCDu);
    for (lh_u32_t a : {1u, 127u, 128u, 254u})
    {
        EXPECT_EQ(lh_ui_pixmap_blend_16_one(lh_addr_of(pixmap), 0x1234u, 0x0033CCFFu, 0xABCDu, a),
                  mix_16_reference(0x1234u, 0x0033CCFFu, a)) << a;
    }
}

/* The portable row kernel against the reference, pixel for pixel: every length
   from 0 to 9, from a word-aligned and from an unaligned start, in both byte
   orders, with the ends (0, 255) and the middle alphas (1, 127, 128, 254) mixed
   in; the pixels around the row are never written. */
TEST(ui_pixmap, blend_alpha_16_words_is_the_reference_for_every_length_and_start)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_rgb565, lh_ui_pixmap_format_rgb565_swapped};
    const lh_byte_t alphas[] = {0u, 1u, 127u, 128u, 254u, 255u, 60u, 200u, 0u, 9u};
    const lh_u32_t src = 0x0033CCFFu;

    for (lh_ui_pixmap_format_t format : formats)
    {
        for (int start = 0; start < 2; ++start)
        {
            for (int count = 0; count <= 9; ++count)
            {
                alignas(4) lh_u16_t row[16];
                lh_u16_t want[16];
                lh_ui_pixmap_t pixmap;

                for (int i = 0; i < 16; ++i)
                {
                    row[i] = want[i] = static_cast<lh_u16_t>((i * 2654435761u) >> 7);
                }
                lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, row), 16, 1, 32, format);
                for (int i = 0; i < count; ++i)
                {
                    const lh_u32_t a = alphas[i];
                    const lh_u32_t w = format == lh_ui_pixmap_format_rgb565 ? want[start + 1 + i] : swap16(want[start + 1 + i]);
                    const lh_u32_t solid = lh_ui_pixmap_pack_rgb(lh_addr_of(pixmap), src);
                    const lh_u32_t mixed = a == 0u ? w : (a == 255u ? solid : mix_16_reference(w, src, a));

                    want[start + 1 + i] =
                        static_cast<lh_u16_t>(format == lh_ui_pixmap_format_rgb565 ? mixed : swap16(mixed));
                }
                /* row + 1 is unaligned, row + 2 is aligned. */
                lh_ui_pixmap_blend_alpha_16_words(lh_addr_of(pixmap), row + start + 1, alphas,
                                                  static_cast<lh_usize_t>(count), src);
                for (int i = 0; i < 16; ++i)
                {
                    ASSERT_EQ(row[i], want[i]) << "format " << static_cast<int>(format) << " start " << start
                                               << " count " << count << " pixel " << i;
                }
            }
        }
    }
}

} // namespace

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(ui_pixmap_death, stride_shorter_than_a_row)
{
    lh_u32_t words[4];
    lh_ui_pixmap_t pixmap;

    LH_EXPECT_DEATH(lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), 4, 1, 12,
                                      lh_ui_pixmap_format_argb8888));
}

TEST(ui_pixmap_death, stride_not_a_whole_number_of_pixels)
{
    lh_u32_t words[4];
    lh_ui_pixmap_t pixmap;

    LH_EXPECT_DEATH(lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), 2, 1, 5,
                                      lh_ui_pixmap_format_rgb565));
}

#endif
