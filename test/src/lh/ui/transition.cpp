#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/ui/canvas/sw.h>
#include <lh/ui/view.h>

namespace
{
using lh_test::rect_of;

lh_ui_color_t
color_of(int r, int g, int b, int a = 255)
{
    lh_ui_color_t c;
    lh_ui_color_init(&c, (lh_ui_color_channel_t)r, (lh_ui_color_channel_t)g, (lh_ui_color_channel_t)b,
                     (lh_ui_color_channel_t)a);
    return c;
}

/* A style filled with one colour, and the paint it is copied from. */
struct look
{
    lh_ui_style_t style;

    explicit look(lh_ui_color_t fill, lh_u16_t ms = 0)
    {
        lh_ui_paint_t paint;

        lh_ui_style_init(&style);
        lh_ui_paint_init_color(&paint, &fill);
        lh_ui_style_assign_fill(&style, &paint);
        lh_ui_style_set_transition(&style, ms);
    }
};

int
red_now(const lh_ui_entity_t *entity)
{
    return (int)lh_ui_color_get_r(lh_ui_style_get_fill_color(lh_ui_entity_get_style(entity)));
}

/* A view over one 40 x 40 box, painted by the software canvas into memory. */
struct scene
{
    static const int side = 40;
    lh_u32_t pixels[side * side];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;
    lh_ui_entity_t root;
    lh_ui_entity_t box;

    scene()
    {
        lh_ui_size_t size;

        for (lh_u32_t &p : pixels)
        {
            p = 0;
        }
        lh_ui_pixmap_init(&pixmap, lh_ptr_rcast(lh_byte_t, pixels), side, side, side * 4,
                          lh_ui_pixmap_format_argb8888);
        lh_ui_canvas_sw_init(&sw);
        lh_ui_canvas_sw_use_pixmap(&sw, &pixmap);
        lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_sw, &sw);
        lh_ui_size_init(&size, lh_ui_scalar_of(side), lh_ui_scalar_of(side));
        lh_ui_canvas_set_size(&canvas, size);
        lh_ui_entity_init(&root, rect_of(0, 0, side, side));
        lh_ui_entity_init(&box, rect_of(0, 0, side, side));
        lh_ui_entity_add_child(&root, &box);
        lh_ui_view_init(&view);
        lh_ui_view_set_canvas(&view, &canvas);
        lh_ui_view_set_root(&view, &root);
    }

    lh_u32_t
    painted()
    {
        lh_ui_view_paint(&view);
        return pixels[20 * side + 20];
    }
};

} // namespace

/* -- Curves ---------------------------------------------------------------- */

TEST(ui_easing, every_curve_starts_at_nothing_and_ends_at_everything)
{
    for (int e = lh_ui_easing_ease; e <= lh_ui_easing_ease_in_out; ++e)
    {
        EXPECT_EQ(lh_ui_easing_apply((lh_ui_easing_t)e, 0), 0u) << "curve " << e;
        EXPECT_EQ(lh_ui_easing_apply((lh_ui_easing_t)e, 255), 255u) << "curve " << e;
        lh_u32_t last = 0;
        for (lh_u32_t t = 0; t <= 255; ++t)
        {
            const lh_u32_t at = lh_ui_easing_apply((lh_ui_easing_t)e, t);
            EXPECT_GE(at, last) << "curve " << e << " goes back at " << t;
            last = at;
        }
    }
}

TEST(ui_easing, ease_in_is_slow_to_start_and_ease_out_slow_to_stop)
{
    EXPECT_EQ(lh_ui_easing_apply(lh_ui_easing_linear, 128), 128u);
    EXPECT_LT(lh_ui_easing_apply(lh_ui_easing_ease_in, 64), 64u);
    EXPECT_GT(lh_ui_easing_apply(lh_ui_easing_ease_out, 64), 64u);
    EXPECT_GT(lh_ui_easing_apply(lh_ui_easing_ease, 128), 190u) << "CSS's ease is most of the way at half time";
}

/* -- A look between two ---------------------------------------------------- */

TEST(ui_style_blend, everything_with_an_amount_is_mixed)
{
    look from(color_of(200, 0, 0));
    look to(color_of(0, 0, 100));
    lh_ui_style_t mid;
    lh_ui_radii_t corners;
    lh_ui_paint_t ink;
    const lh_ui_color_t black = color_of(0, 0, 0);
    const lh_ui_color_t white = color_of(250, 250, 250);

    lh_ui_style_set_radius(&from.style, lh_ui_scalar(0));
    lh_ui_radii_init(&corners, lh_ui_scalar(10), lh_ui_scalar(20), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_style_set_radii_v(&to.style, &corners);
    lh_ui_paint_init_color(&ink, &black);
    lh_ui_style_assign_text(&from.style, &ink);
    lh_ui_paint_init_color(&ink, &white);
    lh_ui_style_assign_text(&to.style, &ink);

    lh_ui_style_init_blend(&mid, &from.style, &to.style, 51);
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_style_get_fill_color(&mid)), 160);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_style_get_fill_color(&mid)), 20);
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_style_get_text_color(&mid)), 50);
    EXPECT_EQ(lh_ui_style_get_radii(&mid)->top_left, lh_ui_scalar(2));
    EXPECT_EQ(lh_ui_style_get_radii(&mid)->top_right, lh_ui_scalar(4));

    lh_ui_style_init_blend(&mid, &from.style, &to.style, 255);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_style_get_fill_color(&mid)), 100) << "all the way is the target";
}

TEST(ui_style_blend, a_shadow_cast_at_one_end_only_fades_in_from_nothing)
{
    look from(color_of(0, 0, 0));
    look to(color_of(0, 0, 0));
    lh_ui_shadow_t cast;
    lh_ui_style_t mid;

    lh_ui_shadow_init(&cast);
    lh_ui_shadow_set_color(&cast, color_of(0, 0, 0, 200));
    lh_ui_shadow_set_spread(&cast, lh_ui_scalar(10));
    lh_ui_style_set_shadow_v(&to.style, &cast);

    lh_ui_style_init_blend(&mid, &from.style, &to.style, 0);
    EXPECT_EQ(lh_ui_color_get_a(&mid.shadow.color), 0) << "nothing at the start";
    lh_ui_style_init_blend(&mid, &from.style, &to.style, 128);
    EXPECT_NEAR((int)lh_ui_color_get_a(&mid.shadow.color), 100, 2);
}

/* -- Flight ---------------------------------------------------------------- */

TEST(ui_transition, a_style_with_no_time_is_taken_at_once)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255));

    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    EXPECT_EQ(lh_ui_entity_get_style(&s.box), &blue.style);
    EXPECT_EQ(lh_ui_view_get_transition_target(&s.view, &s.box), lh_null);
}

TEST(ui_transition, a_style_with_time_is_reached_over_it_and_then_landed_on)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    lh_ui_style_set_easing(&blue.style, lh_ui_easing_linear);
    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    EXPECT_EQ(lh_ui_view_get_transition_target(&s.view, &s.box), &blue.style);
    EXPECT_EQ(red_now(&s.box), 255) << "it sets out from where it was";

    EXPECT_TRUE(lh_ui_view_advance(&s.view, 80) == lh_bool_true);
    EXPECT_NEAR(red_now(&s.box), 128, 2) << "linear, half the time, half the way";

    EXPECT_TRUE(lh_ui_view_advance(&s.view, 80) == lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style(&s.box), &blue.style) << "landed on the style itself";
    EXPECT_EQ(lh_ui_view_get_transition_target(&s.view, &s.box), lh_null);
    EXPECT_TRUE(lh_ui_view_advance(&s.view, 16) == lh_bool_false) << "and nothing is left to move";
}

TEST(ui_transition, the_same_style_asked_for_again_does_not_start_over)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    lh_ui_style_set_easing(&blue.style, lh_ui_easing_linear);
    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_view_advance(&s.view, 80);
    lh_ui_entity_set_style(&s.box, &blue.style);
    EXPECT_NEAR(red_now(&s.box), 128, 2) << "a hover asked for on every pointer move";
}

TEST(ui_transition, turned_round_half_way_it_sets_out_from_the_colour_on_screen)
{
    scene s;
    look red(color_of(255, 0, 0), 160);
    look blue(color_of(0, 0, 255), 160);

    lh_ui_style_set_easing(&red.style, lh_ui_easing_linear);
    lh_ui_style_set_easing(&blue.style, lh_ui_easing_linear);
    lh_ui_entity_set_style_at_once(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_view_advance(&s.view, 80);
    lh_ui_entity_set_style(&s.box, &red.style);
    EXPECT_NEAR(red_now(&s.box), 128, 2) << "no jump back to blue or to red";
    lh_ui_view_advance(&s.view, 80);
    EXPECT_NEAR(red_now(&s.box), 191, 2) << "half way from the middle back to red";
}

TEST(ui_transition, when_every_slot_is_taken_the_switch_is_instant)
{
    scene s;
    lh_ui_entity_t more[LH_UI_VIEW_TRANSITION_MAX];
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    for (lh_ui_entity_t &e : more)
    {
        lh_ui_entity_init(&e, rect_of(0, 0, 4, 4));
        lh_ui_entity_add_child(&s.root, &e);
        lh_ui_entity_set_style(&e, &red.style);
        lh_ui_entity_set_style(&e, &blue.style);
    }
    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    EXPECT_EQ(lh_ui_entity_get_style(&s.box), &blue.style);
    for (lh_ui_entity_t &e : more)
    {
        lh_ui_entity_remove_child(&s.root, &e);
    }
}

TEST(ui_transition, an_entity_taken_out_of_the_tree_lands_and_leaves_its_slot)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_entity_remove_child(&s.root, &s.box);
    EXPECT_EQ(lh_ui_entity_get_style(&s.box), &blue.style) << "landed, not left pointing into the view";
    EXPECT_EQ(lh_ui_view_get_transition_target(&s.view, &s.box), lh_null);
    EXPECT_TRUE(lh_ui_view_advance(&s.view, 16) == lh_bool_false);
}

TEST(ui_transition, a_new_root_lands_the_old_trees_looks)
{
    scene s;
    lh_ui_entity_t other;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_entity_init(&other, rect_of(0, 0, 4, 4));
    lh_ui_view_set_root(&s.view, &other);
    EXPECT_EQ(lh_ui_entity_get_style(&s.box), &blue.style);
}

TEST(ui_transition, a_press_shows_the_pressed_look_of_where_it_is_going)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);
    look pressed(color_of(0, 255, 0));

    lh_ui_style_set_pressed(&blue.style, &pressed.style);
    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_view_advance(&s.view, 32);
    lh_ui_entity_set_pressed(&s.box, lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style_now(&s.box), &pressed.style);
}

/* -- On screen ------------------------------------------------------------- */

/* What the fill draws is the look in flight, not the style it is heading for: the
   pixel in the middle of the box half way through is between the two colours. */
TEST(ui_transition, the_pixels_follow_the_look_in_flight)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    lh_ui_style_set_easing(&blue.style, lh_ui_easing_linear);
    lh_ui_entity_set_style(&s.box, &red.style);
    EXPECT_EQ(s.painted(), 0xFFFF0000u);
    lh_ui_entity_set_style(&s.box, &blue.style);
    lh_ui_view_advance(&s.view, 80);
    const lh_u32_t mid = s.painted();
    EXPECT_NEAR((int)((mid >> 16) & 0xFF), 128, 3);
    EXPECT_NEAR((int)(mid & 0xFF), 127, 3);
    lh_ui_view_advance(&s.view, 80);
    EXPECT_EQ(s.painted(), 0xFF0000FFu);
}

TEST(ui_transition, a_tick_moves_the_looks_and_says_the_frame_changed)
{
    scene s;
    look red(color_of(255, 0, 0));
    look blue(color_of(0, 0, 255), 160);

    EXPECT_TRUE(lh_ui_view_tick(&s.view) == lh_bool_false) << "nothing in flight, nothing to paint";
    lh_ui_entity_set_style(&s.box, &red.style);
    lh_ui_entity_set_style(&s.box, &blue.style);
    EXPECT_TRUE(lh_ui_view_tick(&s.view) == lh_bool_true);
}
