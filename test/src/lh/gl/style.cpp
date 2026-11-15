#include <gtest/gtest.h>

#include <lh/gl/style.h>

TEST(ui_style, init_has_empty_fill)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_gl_paint_is_empty(lh_gl_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_gl_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, set_fill_copies_paint)
{
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_gl_paint_t paint;
    lh_gl_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_gl_style_t style;
    lh_gl_style_init(lh_addr_of(style));
    lh_gl_style_assign_fill(lh_addr_of(style), lh_addr_of(paint));
    EXPECT_FALSE(lh_gl_paint_is_empty(lh_gl_style_get_fill(lh_addr_of(style))));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_style_get_fill_color(lh_addr_of(style))), 1);
    EXPECT_EQ(lh_gl_color_get_g(lh_gl_style_get_fill_color(lh_addr_of(style))), 2);
    EXPECT_EQ(lh_gl_color_get_b(lh_gl_style_get_fill_color(lh_addr_of(style))), 3);
}

TEST(ui_style, set_fill_null_clears)
{
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 10, 20, 30, 255);
    lh_gl_paint_t paint;
    lh_gl_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_gl_style_t style;
    lh_gl_style_init(lh_addr_of(style));
    lh_gl_style_assign_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_gl_style_assign_fill(lh_addr_of(style), lh_null);
    EXPECT_TRUE(lh_gl_paint_is_empty(lh_gl_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_gl_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, radius_starts_square_and_keeps_the_value)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_gl_style_get_radius(lh_addr_of(style)), lh_gl_scalar(0));
    lh_gl_style_set_radius_v(lh_addr_of(style), lh_gl_scalar(6));
    EXPECT_EQ(lh_gl_style_get_radius(lh_addr_of(style)), lh_gl_scalar(6));
    lh_gl_style_set_radius_v(lh_addr_of(style), LH_GL_RADIUS_CIRCLE);
    EXPECT_EQ(lh_gl_style_get_radius(lh_addr_of(style)), LH_GL_RADIUS_CIRCLE);
}

TEST(ui_style, the_hit_radius_starts_at_the_whole_rect)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    /* The target is not the look: a rounded entity is pressed by its rect until
       an app says otherwise, and `0` is that "otherwise" without a sentinel. */
    EXPECT_EQ(lh_gl_style_get_hit_radius(lh_addr_of(style)), lh_gl_scalar(0));
    EXPECT_EQ(lh_gl_style_get_radius(lh_addr_of(style)), lh_gl_scalar(0));
    lh_gl_style_set_hit_radius_v(lh_addr_of(style), lh_gl_scalar(6));
    EXPECT_EQ(lh_gl_style_get_hit_radius(lh_addr_of(style)), lh_gl_scalar(6));
    EXPECT_EQ(lh_gl_style_get_radius(lh_addr_of(style)), lh_gl_scalar(0)) << "the look followed";
    lh_gl_style_set_hit_radius_v(lh_addr_of(style), LH_GL_RADIUS_CIRCLE);
    EXPECT_EQ(lh_gl_style_get_hit_radius(lh_addr_of(style)), LH_GL_RADIUS_CIRCLE);
}

TEST(ui_style, padding_starts_at_zero_and_keeps_what_is_set)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_gl_insets_is_zero(lh_gl_style_get_padding(lh_addr_of(style))), lh_bool_true);
    lh_gl_style_set_padding_all(lh_addr_of(style), lh_gl_scalar(12));
    EXPECT_EQ(lh_gl_insets_get_left(lh_gl_style_get_padding(lh_addr_of(style))), lh_gl_scalar(12));
    EXPECT_EQ(lh_gl_insets_get_bottom(lh_gl_style_get_padding(lh_addr_of(style))), lh_gl_scalar(12));
    lh_gl_insets_t sides;
    lh_gl_insets_init(lh_addr_of(sides), 1, 2, 3, 4);
    lh_gl_style_set_padding_v(lh_addr_of(style), lh_addr_of(sides));
    EXPECT_EQ(lh_gl_insets_get_top(lh_gl_style_get_padding(lh_addr_of(style))), lh_gl_scalar(2));
    EXPECT_EQ(lh_gl_insets_get_right(lh_gl_style_get_padding(lh_addr_of(style))), lh_gl_scalar(3));
}

/* Alignment is a style's to say, and what a fresh style says has to be what a label
   did before alignment existed — otherwise every entity in the tree moves. */
TEST(ui_style, init_aligns_text_top_left)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_gl_style_get_align_h(lh_addr_of(style)), lh_gl_text_align_h_left);
    EXPECT_EQ(lh_gl_style_get_align_v(lh_addr_of(style)), lh_gl_text_align_v_top);
}

TEST(ui_style, align_round_trips)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    lh_gl_style_set_align_h(lh_addr_of(style), lh_gl_text_align_h_right);
    lh_gl_style_set_align_v(lh_addr_of(style), lh_gl_text_align_v_bottom);
    EXPECT_EQ(lh_gl_style_get_align_h(lh_addr_of(style)), lh_gl_text_align_h_right);
    EXPECT_EQ(lh_gl_style_get_align_v(lh_addr_of(style)), lh_gl_text_align_v_bottom);
}

/* A fresh style casts no shadow, so an entity that never asked for one pays one
   question per frame and nothing else. */
TEST(ui_style, init_casts_no_shadow)
{
    lh_gl_style_t style;

    lh_gl_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_gl_shadow_is_empty(lh_gl_style_get_shadow(lh_addr_of(style))));
    EXPECT_EQ(lh_gl_shadow_get_spread(lh_gl_style_get_shadow(lh_addr_of(style))), lh_gl_scalar(0));
}

TEST(ui_style, set_shadow_copies_it_and_null_clears)
{
    lh_gl_shadow_t shadow;

    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_gl_color_t{0, 0, 0, 150});
    lh_gl_shadow_set_spread_v(lh_addr_of(shadow), lh_gl_scalar(14));
    lh_gl_shadow_set_offset(lh_addr_of(shadow), lh_gl_scalar(0), lh_gl_scalar(4));
    lh_gl_style_t style;
    lh_gl_style_init(lh_addr_of(style));
    lh_gl_style_set_shadow_v(lh_addr_of(style), lh_addr_of(shadow));

    /* Copied, like the paints: the style is not a window onto somebody's value. */
    EXPECT_EQ(lh_gl_shadow_get_spread(lh_gl_style_get_shadow(lh_addr_of(style))), lh_gl_scalar(14));
    lh_gl_shadow_set_spread_v(lh_addr_of(shadow), lh_gl_scalar(99));
    EXPECT_EQ(lh_gl_shadow_get_spread(lh_gl_style_get_shadow(lh_addr_of(style))), lh_gl_scalar(14));

    lh_gl_style_set_shadow_v(lh_addr_of(style), lh_null);
    EXPECT_TRUE(lh_gl_shadow_is_empty(lh_gl_style_get_shadow(lh_addr_of(style))));
}

TEST(ui_style, a_fresh_style_has_no_pressed_look)
{
    lh_gl_style_t pressed;

    lh_gl_style_init(lh_addr_of(pressed));
    lh_gl_style_t style;
    lh_gl_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_null_eq(lh_gl_style_get_pressed(lh_addr_of(style))));

    /* Not copied, and not owned: the pressed style is a style like any other and
       has to outlive every entity that points at it. */
    lh_gl_style_set_pressed(lh_addr_of(style), lh_addr_of(pressed));
    EXPECT_EQ(lh_gl_style_get_pressed(lh_addr_of(style)), lh_addr_of(pressed));
    lh_gl_style_set_pressed(lh_addr_of(style), lh_null);
    EXPECT_TRUE(lh_null_eq(lh_gl_style_get_pressed(lh_addr_of(style))));
}
