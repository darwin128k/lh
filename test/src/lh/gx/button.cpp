/**
 * @file button.cpp
 * @brief Tests for `lh/gx/button.h`: the first control made of entities.
 *
 * A button on a small screen, drawn by the software backend with a tiny baked
 * font (an "A" two pixels square): the caption is centred in the box, a press
 * on the caption is a press on the button, a release on it is a click and a
 * release elsewhere is not, the pressed look is put on and taken off and only
 * the button is redrawn for it.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/button.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/pointer.h>
#include <lh/gx/render.h>
#include <lh/test/gx/tiny_font.h>

namespace
{

constexpr lh_gx_color_t black = 0xFF000000U;
constexpr lh_gx_color_t white = 0xFFFFFFFFU;
constexpr lh_gx_color_t grey = 0xFF808080U;
constexpr lh_gx_color_t red = 0xFFFF0000U;

lh_void
count_click(lh_gx_button_t *, lh_ptr context)
{
    ++*static_cast<int *>(context);
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

/* A 40 by 20 black screen, a white button "A" at (4,4) 20 by 10, ink red. */
struct Screen
{
    lh_test::gx_tiny_font tiny;
    lh_u32_t words[40 * 20] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t button_style;
    lh_gx_style_t pressed_style;
    lh_gx_entity_t back;
    lh_gx_button_t button;
    lh_gx_pointer_t pointer;
    int clicks = 0;

    Screen()
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 40U, 20U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 40, 20);
        lh_gx_style_init(&back_style, black, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&button_style, white, 0U, 0, 0, red, &tiny.font);
        lh_gx_style_init(&pressed_style, grey, 0U, 0, 0, red, &tiny.font);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 20);
        lh_gx_button_init(&button, &button_style, 4, 4, 20, 10, "A", count_click, &clicks);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, lh_gx_button_get_box_mut(&button));
        lh_gx_pointer_init(&pointer);
        draw();
    }

    lh_void
    draw()
    {
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 40 + x];
    }

    lh_void
    press(lh_gx_scalar_t x, lh_gx_scalar_t y)
    {
        const lh_gx_point_t where = point(x, y);

        lh_gx_pointer_press(&pointer, lh_gx_render_get_screen_mut(&render), &where);
    }

    lh_void
    release(lh_gx_scalar_t x, lh_gx_scalar_t y)
    {
        const lh_gx_point_t where = point(x, y);

        lh_gx_pointer_release(&pointer, &where);
    }
};

TEST(gx_button, the_caption_is_centred_and_drawn_in_the_ink)
{
    Screen screen;
    const lh_gx_rect_t *caption = lh_gx_entity_get_place(lh_gx_button_get_caption(&screen.button));

    /* "A" is 3 wide (its advance) and a line is 3 tall: (20-3+1)/2, (10-3+1)/2. */
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_rect_get_origin(caption)), 9);
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_rect_get_origin(caption)), 4);
    EXPECT_EQ(lh_gx_size_get_width(lh_gx_rect_get_size(caption)), 3);
    EXPECT_EQ(screen.at(5, 5), white) << "the box";
    /* The caption at (13,8) on the screen; ascent 2, so the "A" is rows 8 and 9. */
    EXPECT_EQ(screen.at(13, 8), red);
    EXPECT_EQ(screen.at(14, 9), red);
    EXPECT_EQ(screen.at(15, 8), white) << "past the ink";
    EXPECT_EQ(screen.at(2, 2), black) << "outside the button";
}

TEST(gx_button, a_press_on_the_caption_and_a_release_on_it_is_a_click)
{
    Screen screen;

    screen.press(13, 8);
    EXPECT_EQ(lh_gx_pointer_get_captured(&screen.pointer), lh_gx_button_get_box(&screen.button))
        << "the caption takes no input: the press goes to the box";
    EXPECT_TRUE(lh_gx_button_is_pressed(&screen.button));
    screen.release(20, 12);
    EXPECT_FALSE(lh_gx_button_is_pressed(&screen.button));
    EXPECT_EQ(screen.clicks, 1);
}

TEST(gx_button, a_release_outside_or_a_cancel_is_no_click)
{
    Screen screen;

    screen.press(6, 6);
    screen.release(30, 6);
    EXPECT_FALSE(lh_gx_button_is_pressed(&screen.button));
    screen.press(6, 6);
    lh_gx_pointer_cancel(&screen.pointer);
    EXPECT_FALSE(lh_gx_button_is_pressed(&screen.button));
    EXPECT_EQ(screen.clicks, 0);
}

TEST(gx_button, the_pressed_look_redraws_only_the_button)
{
    Screen screen;

    lh_gx_button_set_pressed_style(&screen.button, &screen.pressed_style);
    screen.press(6, 6);
    lh_gx_damage_t drawn;
    lh_gx_render_draw(&screen.render, &drawn);
    ASSERT_EQ(lh_gx_damage_get_count(&drawn), 1U);
    EXPECT_TRUE(lh_gx_rect_eq(lh_gx_damage_get_rect(&drawn, 0U),
                              lh_gx_entity_get_place(lh_gx_button_get_box(&screen.button))))
        << "the button's own rectangle (its owner is at the origin)";
    EXPECT_EQ(screen.at(5, 5), grey);
    EXPECT_EQ(screen.at(13, 8), red) << "the caption is drawn over the new fill";
    screen.release(6, 6);
    screen.draw();
    EXPECT_EQ(screen.at(5, 5), white);
    EXPECT_EQ(screen.clicks, 1);
}

TEST(gx_button, a_new_text_is_centred_again)
{
    Screen screen;

    lh_gx_button_set_text(&screen.button, "AA");
    const lh_gx_rect_t *caption = lh_gx_entity_get_place(lh_gx_button_get_caption(&screen.button));

    EXPECT_EQ(lh_gx_size_get_width(lh_gx_rect_get_size(caption)), 6);
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_rect_get_origin(caption)), 7);
    EXPECT_STREQ(lh_gx_button_get_text(&screen.button), "AA");
}

} // namespace
