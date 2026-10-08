#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/paint.h>
#include <lh/ui/shadow.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(ui_style, init_has_empty_fill)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, set_fill_copies_paint)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    EXPECT_FALSE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_style_get_fill_color(lh_addr_of(style))), 1);
    EXPECT_EQ(lh_ui_color_get_g(lh_ui_style_get_fill_color(lh_addr_of(style))), 2);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_style_get_fill_color(lh_addr_of(style))), 3);
}

TEST(ui_style, set_fill_null_clears)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 10, 20, 30, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_style_set_fill(lh_addr_of(style), lh_ptr_rcast(const lh_ui_paint_t, lh_null));
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, radius_starts_square_and_keeps_the_value)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), lh_ui_scalar(0));
    lh_ui_style_set_radius(lh_addr_of(style), lh_ui_scalar(6));
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), lh_ui_scalar(6));
    lh_ui_style_set_radius(lh_addr_of(style), LH_UI_RADIUS_CIRCLE);
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), LH_UI_RADIUS_CIRCLE);
}

TEST(ui_style, padding_starts_at_zero_and_keeps_what_is_set)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_ui_insets_is_zero(lh_ui_style_get_padding(lh_addr_of(style))), lh_bool_true);
    lh_ui_style_set_padding(lh_addr_of(style), lh_ui_scalar(12));
    EXPECT_EQ(lh_ui_insets_get_left(lh_ui_style_get_padding(lh_addr_of(style))), lh_ui_scalar(12));
    EXPECT_EQ(lh_ui_insets_get_bottom(lh_ui_style_get_padding(lh_addr_of(style))), lh_ui_scalar(12));
    lh_ui_insets_t sides;
    lh_ui_insets_init(lh_addr_of(sides), 1, 2, 3, 4);
    lh_ui_style_set_padding_insets(lh_addr_of(style), lh_addr_of(sides));
    EXPECT_EQ(lh_ui_insets_get_top(lh_ui_style_get_padding(lh_addr_of(style))), lh_ui_scalar(2));
    EXPECT_EQ(lh_ui_insets_get_right(lh_ui_style_get_padding(lh_addr_of(style))), lh_ui_scalar(3));
}

/* Alignment is a style's to say, and what a fresh style says has to be what a label
   did before alignment existed — otherwise every entity in the tree moves. */
TEST(ui_style, init_aligns_text_top_left)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_ui_style_get_align_h(lh_addr_of(style)), lh_ui_text_align_h_left);
    EXPECT_EQ(lh_ui_style_get_align_v(lh_addr_of(style)), lh_ui_text_align_v_top);
}

TEST(ui_style, align_round_trips)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_align_h(lh_addr_of(style), lh_ui_text_align_h_right);
    lh_ui_style_set_align_v(lh_addr_of(style), lh_ui_text_align_v_bottom);
    EXPECT_EQ(lh_ui_style_get_align_h(lh_addr_of(style)), lh_ui_text_align_h_right);
    EXPECT_EQ(lh_ui_style_get_align_v(lh_addr_of(style)), lh_ui_text_align_v_bottom);
}

/* A fresh style casts no shadow, so an entity that never asked for one pays one
   question per frame and nothing else. */
TEST(ui_style, init_casts_no_shadow)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_ui_shadow_is_empty(lh_ui_style_get_shadow(lh_addr_of(style))));
    EXPECT_EQ(lh_ui_shadow_get_spread(lh_ui_style_get_shadow(lh_addr_of(style))), lh_ui_scalar(0));
}

TEST(ui_style, set_shadow_copies_it_and_null_clears)
{
    lh_ui_shadow_t shadow;
    lh_ui_style_t style;

    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), (lh_ui_color_t){0, 0, 0, 150});
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(14));
    lh_ui_shadow_set_offset(lh_addr_of(shadow), lh_ui_scalar(0), lh_ui_scalar(4));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_shadow(lh_addr_of(style), lh_addr_of(shadow));

    /* Copied, like the paints: the style is not a window onto somebody's value. */
    EXPECT_EQ(lh_ui_shadow_get_spread(lh_ui_style_get_shadow(lh_addr_of(style))), lh_ui_scalar(14));
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(99));
    EXPECT_EQ(lh_ui_shadow_get_spread(lh_ui_style_get_shadow(lh_addr_of(style))), lh_ui_scalar(14));

    lh_ui_style_set_shadow(lh_addr_of(style), lh_ptr_rcast(const lh_ui_shadow_t, lh_null));
    EXPECT_TRUE(lh_ui_shadow_is_empty(lh_ui_style_get_shadow(lh_addr_of(style))));
}

TEST(ui_style, a_fresh_style_has_no_pressed_look)
{
    lh_ui_style_t pressed;
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(pressed));
    lh_ui_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_pressed(lh_addr_of(style))));

    /* Not copied, and not owned: the pressed style is a style like any other and
       has to outlive every entity that points at it. */
    lh_ui_style_set_pressed(lh_addr_of(style), lh_addr_of(pressed));
    EXPECT_EQ(lh_ui_style_get_pressed(lh_addr_of(style)), lh_addr_of(pressed));
    lh_ui_style_set_pressed(lh_addr_of(style), lh_ptr_rcast(const lh_ui_style_t, lh_null));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_pressed(lh_addr_of(style))));
}
