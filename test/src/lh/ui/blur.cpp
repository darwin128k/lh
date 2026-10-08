#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/blur.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

/* 16 x 8 of ARGB8888. Blur reads neighbours, so the test wants a picture with
   edges in it rather than one flat colour. */
struct blur_fixture
{
    static const int width = 16;
    static const int height = 8;
    lh_u32_t words[width * height];
    lh_u8_t scratch[width * height * 4];
    /* Memory right after the scratch, filled with whatever the test is testing: a
       blur that reads past its buffer reads *this*, and a test that only checks
       values cannot tell a right answer from a lucky one. */
    lh_u8_t poison[8192];
    lh_ui_pixmap_t pixmap;

    blur_fixture()
    {
        lh_ui_color_t color;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                /* A vertical step: black on the left half, white on the right. */
                lh_ui_color_init(&color, x < width / 2 ? 0 : 255, x < width / 2 ? 0 : 255,
                                 x < width / 2 ? 0 : 255, 255);
                words[y * width + x] = lh_ui_color_get_argb(&color);
            }
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), width, height, width * 4,
                          lh_ui_pixmap_format_argb8888);
    }

    void
    fill_poison(lh_u8_t value)
    {
        for (lh_u8_t &b : poison)
        {
            b = value;
        }
    }

    lh_u8_t
    red_at(int x, int y) const
    {
        const lh_ui_color_t pixel = lh_ui_pixmap_get_pixel(lh_addr_of(pixmap), x, y);

        return lh_ui_color_get_r(&pixel);
    }
};

TEST(ui_blur, a_flat_picture_stays_flat)
{
    blur_fixture f;
    lh_ui_rect_t rect;

    for (int y = 0; y < blur_fixture::height; ++y)
    {
        for (int x = 0; x < blur_fixture::width; ++x)
        {
            f.words[y * blur_fixture::width + x] = 0xFF404040;
        }
    }
    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(2), lh_ui_scalar(1), lh_ui_scalar(10), lh_ui_scalar(5));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(2), f.scratch);

    /* Every channel keeps its value when there is nothing to average towards. */
    EXPECT_EQ(f.red_at(5, 3), 0x40);
}

TEST(ui_blur, an_edge_becomes_a_ramp_and_stays_a_ramp)
{
    blur_fixture f;
    lh_ui_rect_t rect;
    lh_u8_t middle;
    lh_u8_t near_edge;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(blur_fixture::width),
                    lh_ui_scalar(blur_fixture::height));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(3), f.scratch);

    middle = f.red_at(blur_fixture::width / 2 - 1, 3);
    near_edge = f.red_at(blur_fixture::width / 2, 3);

    /* The step was 0 -> 255. A normalised kernel moves the shades around without
       inventing any, so the two pixels straddling the step still add up to what
       was there: the darker one gained exactly what the lighter one lost. */
    EXPECT_GT(middle, 0);
    EXPECT_LT(middle, 255);
    EXPECT_EQ(middle + near_edge, 255);
    /* Radius 3 reaches three pixels back from the step, so four is still solid
       black and three is already touched. */
    EXPECT_EQ(f.red_at(blur_fixture::width / 2 - 4, 3), 0);
    EXPECT_GT(f.red_at(blur_fixture::width / 2 - 3, 3), 0);
    /* Two past the step the window still reaches back over it; four does not. */
    EXPECT_LT(f.red_at(blur_fixture::width / 2 + 2, 3), 255);
    EXPECT_EQ(f.red_at(blur_fixture::width / 2 + 4, 3), 255);
}

TEST(ui_blur, a_radius_of_zero_changes_nothing)
{
    blur_fixture f;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(blur_fixture::width),
                    lh_ui_scalar(blur_fixture::height));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(0), f.scratch);

    EXPECT_EQ(f.red_at(0, 3), 0);
    EXPECT_EQ(f.red_at(blur_fixture::width - 1, 3), 255);
}

TEST(ui_blur, only_the_rect_moves)
{
    blur_fixture f;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(6), lh_ui_scalar(2), lh_ui_scalar(10), lh_ui_scalar(5));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(3), f.scratch);

    /* Left of the rect the step is untouched; inside it has been spread. */
    EXPECT_EQ(f.red_at(0, 3), 0);
    EXPECT_EQ(f.red_at(4, 3), 0);
    EXPECT_GT(f.red_at(7, 3), 0);
}

TEST(ui_blur, a_rect_hanging_over_the_edge_blurs_what_is_there)
{
    blur_fixture f;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(-8), lh_ui_scalar(-8), lh_ui_scalar(64), lh_ui_scalar(64));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(2), f.scratch);

    /* Cut to the pixmap first: every pixel is in range afterwards, and the
       picture is blurred rather than half written. */
    for (int y = 0; y < blur_fixture::height; ++y)
    {
        for (int x = 0; x < blur_fixture::width; ++x)
        {
            EXPECT_LE(f.red_at(x, y), 255);
        }
    }
    EXPECT_GT(f.red_at(blur_fixture::width / 2 - 2, 3), 0);
}

/* A blur that has to allocate is a blur that faults on an app with no heap under
 * it, so the buffer is the caller's and nothing happens without one. */
TEST(ui_blur, without_a_scratch_nothing_is_blurred)
{
    blur_fixture f;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(blur_fixture::width),
                    lh_ui_scalar(blur_fixture::height));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(3), static_cast<lh_u8_t *>(nullptr));

    EXPECT_EQ(f.red_at(0, 3), 0);
    EXPECT_EQ(f.red_at(blur_fixture::width / 2 - 1, 3), 0);
    EXPECT_EQ(f.red_at(blur_fixture::width / 2, 3), 255);
    EXPECT_EQ(f.red_at(blur_fixture::width - 1, 3), 255);
}

TEST(ui_blur, the_scratch_is_one_rgba_pixel_per_pixel)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(2), lh_ui_scalar(3), lh_ui_scalar(20), lh_ui_scalar(10));
    EXPECT_EQ(lh_ui_blur_scratch_size(lh_addr_of(rect)), 20u * 10u * 4u);

    lh_ui_rect_init_empty(lh_addr_of(rect));
    EXPECT_EQ(lh_ui_blur_scratch_size(lh_addr_of(rect)), 0u);
}

TEST(ui_blur, a_window_sums_and_gives_back_what_it_took)
{
    lh_ui_blur_window_t window = {0, 0, 0, 0};
    lh_ui_color_t a;
    lh_ui_color_t b;

    lh_ui_color_init(&a, 10, 20, 30, 40);
    lh_ui_color_init(&b, 50, 60, 70, 80);
    lh_ui_blur_window_take(&window, &a);
    lh_ui_blur_window_take(&window, &b);
    EXPECT_EQ(lh_ui_blur_window_average(&window, 0, 2), 30);
    EXPECT_EQ(lh_ui_blur_window_average(&window, 3, 2), 60);

    lh_ui_blur_window_drop(&window, &a);
    EXPECT_EQ(lh_ui_blur_window_average(&window, 0, 1), 50);
}

} // namespace
/* The bottom rows are where a sliding window runs off the end of its own buffer:
 * the pass that slides has to stop taking when there is nothing left to take, or
 * it averages whatever memory follows the scratch and the bottom of the picture
 * goes dark for no reason at all. */
TEST(ui_blur, the_bottom_rows_are_blurred_and_not_read_past)
{
    blur_fixture f;
    lh_ui_rect_t rect;
    lh_ui_color_t color;

    /* Top half black, bottom half white: after the blur every row still has to be
     * somewhere between its own value and its neighbour's, and the last row of a
     * white half is white. */
    for (int y = 0; y < blur_fixture::height; ++y)
    {
        for (int x = 0; x < blur_fixture::width; ++x)
        {
            lh_ui_color_init(&color, y < blur_fixture::height / 2 ? 0 : 255, y < blur_fixture::height / 2 ? 0 : 255,
                             y < blur_fixture::height / 2 ? 0 : 255, 255);
            f.words[y * blur_fixture::width + x] = lh_ui_color_get_argb(&color);
        }
    }
    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(blur_fixture::width),
                    lh_ui_scalar(blur_fixture::height));
    lh_ui_blur_rect(lh_addr_of(f.pixmap), lh_addr_of(rect), lh_ui_scalar(2), f.scratch);

    EXPECT_EQ(f.red_at(0, 0), 0);
    EXPECT_EQ(f.red_at(0, blur_fixture::height - 1), 255);
    for (int y = 1; y < blur_fixture::height; ++y)
    {
        EXPECT_GE(f.red_at(0, y), f.red_at(0, y - 1)) << y;
    }
}
/* The sharpest detector there is for a blur that reads past its own buffer: the
 * same picture, the same radius, and whatever memory follows the scratch filled
 * two different ways. A blur that stays inside its buffer cannot tell them
 * apart; one that does not, cannot. */
TEST(ui_blur, the_picture_does_not_depend_on_what_follows_the_scratch)
{
    blur_fixture low;
    blur_fixture high;

    for (int y = 0; y < blur_fixture::height; ++y)
    {
        for (int x = 0; x < blur_fixture::width; ++x)
        {
            lh_ui_color_t color;

            lh_ui_color_init(&color, x * 16, y * 32, 128, 255);
            low.words[y * blur_fixture::width + x] = lh_ui_color_get_argb(&color);
            high.words[y * blur_fixture::width + x] = lh_ui_color_get_argb(&color);
        }
    }
    low.fill_poison(0);
    high.fill_poison(255);

    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(blur_fixture::width),
                    lh_ui_scalar(blur_fixture::height));
    lh_ui_blur_rect(lh_addr_of(low.pixmap), lh_addr_of(rect), lh_ui_scalar(3), low.scratch);
    lh_ui_blur_rect(lh_addr_of(high.pixmap), lh_addr_of(rect), lh_ui_scalar(3), high.scratch);

    int different = 0;
    for (int y = 0; y < blur_fixture::height; ++y)
    {
        for (int x = 0; x < blur_fixture::width; ++x)
        {
            const lh_ui_color_t a = lh_ui_pixmap_get_pixel(lh_addr_of(low.pixmap), x, y);
            const lh_ui_color_t b = lh_ui_pixmap_get_pixel(lh_addr_of(high.pixmap), x, y);

            if (a.r != b.r || a.g != b.g || a.b != b.b)
            {
                ++different;
            }
        }
    }
    EXPECT_EQ(different, 0);
}