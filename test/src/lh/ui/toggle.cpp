#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/button.h>
#include <lh/ui/toggle.h>
#include <lh/ui/paint.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/shadow.h>
#include <lh/ui/style.h>
#include <lh/ui/view.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

int g_clicks = 0;
const lh_ui_toggle_t *g_clicked = lh_null;
lh_bool_t g_checked_at_click = lh_bool_false;
lh_ptr g_context = lh_null;

void
count_click(lh_ui_toggle_t *self, lh_ptr context)
{
    ++g_clicks;
    g_clicked = self;
    g_context = context;
    /* The state at the moment the app is told: a toggle answers with itself, so the
       callback does not have to track which way it went. */
    g_checked_at_click = lh_ui_toggle_get_checked(self);
}

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;

    lh_ui_point_init(&point, lh_ui_scalar(x), lh_ui_scalar(y));
    return point;
}

void
build_style(lh_ui_style_t *style, lh_byte_t grey, lh_ui_scalar_t spread)
{
    lh_ui_paint_t paint;
    lh_ui_color_t color;
    lh_ui_shadow_t shadow;

    lh_ui_style_init(style);
    lh_ui_color_init(&color, grey, grey, grey, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_set_fill(style, &paint);
    if (spread > lh_ui_scalar(0))
    {
        lh_ui_shadow_init(&shadow);
        lh_ui_shadow_set_color(&shadow, lh_ui_color_t{0, 0, 0, 200});
        lh_ui_shadow_set_spread(&shadow, spread);
        lh_ui_style_set_shadow(style, &shadow);
    }
}

/* The four looks of a toggle, each a different grey so a test can tell which one
   the entity is painting. Only the resting look of the "on" state carries a
   shadow: that is the one the damage test needs to reach past the box. */
struct looks
{
    lh_ui_style_t off;
    lh_ui_style_t off_hot;
    lh_ui_style_t on;
    lh_ui_style_t on_hot;
};

void
build_looks(looks &l)
{
    build_style(&l.off, 10, lh_ui_scalar(0));
    build_style(&l.off_hot, 30, lh_ui_scalar(0));
    build_style(&l.on, 50, lh_ui_scalar(6));
    build_style(&l.on_hot, 70, lh_ui_scalar(0));
}

void
build_toggle(lh_ui_toggle_t *toggle, looks &l)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 20, 20, 40, 20);
    lh_ui_toggle_init(toggle, rect);
    lh_ui_toggle_set_off_style(toggle, &l.off, &l.off_hot);
    lh_ui_toggle_set_on_style(toggle, &l.on, &l.on_hot);
}

void
click(lh_ui_toggle_t *toggle)
{
    lh_ui_point_t local;

    lh_ui_point_init(&local, lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_entity_send(lh_ui_toggle_as_entity(toggle), lh_ui_entity_event_click, &local);
}
} // namespace

/* A toggle is a button and a tree has to be able to tell that from the class
   alone: the button class is the toggle's base, so both searches find it, and
   the toggle's own search does not find a plain button. */
TEST(entity_toggle, init_is_a_button_with_nothing_declared)
{
    lh_ui_toggle_t toggle;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 40, 20);
    lh_ui_toggle_init(&toggle, rect);

    EXPECT_EQ(lh_ui_entity_get_class(lh_ui_toggle_as_entity(&toggle)),
              lh_addr_of(lh_ui_toggle_class));
    EXPECT_EQ(lh_ui_entity_as_toggle(lh_ui_toggle_as_entity(&toggle)), &toggle);
    EXPECT_EQ(lh_ui_entity_as_button(lh_ui_toggle_as_entity(&toggle)),
              lh_ui_toggle_as_button(&toggle));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle))));
    EXPECT_FALSE(lh_ui_toggle_get_checked(&toggle));
    EXPECT_FALSE(lh_ui_button_get_hot(lh_ui_toggle_as_button(&toggle)));
    EXPECT_TRUE(lh_null_eq(lh_ui_toggle_get_on_click(&toggle)));
}

TEST(entity_toggle, as_toggle_tells_a_toggle_from_a_button_or_a_plain_entity)
{
    lh_ui_toggle_t toggle;
    lh_ui_button_t button;
    lh_ui_entity_t plain;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 40, 20);
    lh_ui_toggle_init(&toggle, rect);
    lh_ui_button_init(&button, rect);
    lh_ui_entity_init(&plain, rect);

    EXPECT_TRUE(lh_null_eq(lh_ui_entity_as_toggle(lh_ui_button_as_entity(&button))))
        << "a plain button was taken for a toggle";
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_as_toggle(&plain)))
        << "a plain entity was taken for a toggle";
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_as_toggle(lh_null)));
}

/* What a toggle adds to a button: the state picks which pair the entity paints,
   and both directions are the same rule. */
TEST(entity_toggle, the_state_picks_the_pair_the_entity_paints)
{
    lh_ui_toggle_t toggle;
    looks l;

    build_looks(l);
    build_toggle(&toggle, l);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.off);

    lh_ui_toggle_set_checked(&toggle, lh_bool_true);
    EXPECT_TRUE(lh_ui_toggle_get_checked(&toggle));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on)
        << "the entity still paints the looks of the other state";

    lh_ui_toggle_set_checked(&toggle, lh_bool_false);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.off);
}

/* The pointer asked the button for a look, not the state: a flip while hovered
   keeps the hot look, and it is the hot look of the state it moved to. */
TEST(entity_toggle, a_flip_while_hovered_keeps_the_hot_look_of_the_new_state)
{
    lh_ui_toggle_t toggle;
    looks l;

    build_looks(l);
    build_toggle(&toggle, l);
    lh_ui_button_set_hot(lh_ui_toggle_as_button(&toggle), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.off_hot);

    lh_ui_toggle_set_checked(&toggle, lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on_hot)
        << "the flip dropped the pointer's look";
    EXPECT_EQ(lh_ui_button_get_style_now(lh_ui_toggle_as_button(&toggle)), &l.on_hot);

    lh_ui_button_set_hot(lh_ui_toggle_as_button(&toggle), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on)
        << "leaving the pointer did not put the resting look of the state back";
}

/* A pair that arrives while its state is the one showing has to show at once,
   the same as a hot style that arrives under the pointer. */
TEST(entity_toggle, looks_declared_while_on_show_at_once)
{
    lh_ui_toggle_t toggle;
    looks l;
    lh_ui_rect_t rect;

    build_looks(l);
    lh_ui_rect_init(&rect, 0, 0, 40, 20);
    lh_ui_toggle_init(&toggle, rect);
    lh_ui_toggle_set_checked(&toggle, lh_bool_true);
    lh_ui_toggle_set_on_style(&toggle, &l.on, &l.on_hot);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on);

    lh_ui_toggle_set_off_style(&toggle, &l.off, &l.off_hot);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on)
        << "the pair of the other state took over the entity";
}

/* Half a pair is allowed: a state with no hot look is a hover that changes
   nothing, and must not put an empty style on the entity. */
TEST(entity_toggle, a_state_with_no_hot_look_paints_the_resting_one)
{
    lh_ui_toggle_t toggle;
    looks l;

    build_looks(l);
    build_toggle(&toggle, l);
    lh_ui_toggle_set_on_style(&toggle, &l.on, lh_null);
    lh_ui_toggle_set_checked(&toggle, lh_bool_true);
    lh_ui_button_set_hot(lh_ui_toggle_as_button(&toggle), lh_bool_true);

    EXPECT_TRUE(lh_ui_button_get_hot(lh_ui_toggle_as_button(&toggle)));
    EXPECT_EQ(lh_ui_button_get_style_now(lh_ui_toggle_as_button(&toggle)), &l.on);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_toggle_as_entity(&toggle)), &l.on);
}

TEST(entity_toggle, a_click_flips_before_it_calls_the_callback)
{
    lh_ui_toggle_t toggle;
    looks l;
    int context = 7;

    build_looks(l);
    build_toggle(&toggle, l);
    g_clicks = 0;
    g_clicked = lh_null;
    g_checked_at_click = lh_bool_false;
    lh_ui_toggle_set_on_click(&toggle, count_click, &context);

    click(&toggle);

    EXPECT_EQ(g_clicks, 1);
    EXPECT_EQ(g_clicked, &toggle);
    EXPECT_EQ(g_context, &context);
    EXPECT_TRUE(g_checked_at_click)
        << "the callback ran before the flip, so the app had to track which way it went";
    EXPECT_TRUE(lh_ui_toggle_get_checked(&toggle));
}

/* No callback is not a broken toggle: the state is the component's business. */
TEST(entity_toggle, a_click_without_a_callback_still_flips)
{
    lh_ui_toggle_t toggle;
    looks l;

    build_looks(l);
    build_toggle(&toggle, l);
    g_clicks = 0;

    click(&toggle);

    EXPECT_EQ(g_clicks, 0);
    EXPECT_TRUE(lh_ui_toggle_get_checked(&toggle));
    click(&toggle);
    EXPECT_FALSE(lh_ui_toggle_get_checked(&toggle));
}

/* The click flips the looks, and the flip happens after the release damaged the
   looks the toggle had then. The look it arrives on has a shadow that reaches
   six pixels past the box, so a damage that stops at the box leaves that fringe
   on the surface until something else paints over it. */
TEST(entity_toggle, a_flip_damages_the_look_it_arrives_on)
{
    lh_ui_entity_t root;
    lh_ui_toggle_t toggle;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;
    looks l;
    const lh_ui_rect_t *damage;

    build_looks(l);
    build_toggle(&toggle, l);
    lh_ui_entity_init(&root, rect_of(0, 0, 200, 200));
    lh_ui_entity_add_child(&root, lh_ui_toggle_as_entity(&toggle));
    lh_ui_canvas_init(&canvas, lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_view_init(&view);
    lh_ui_view_set_canvas(&view, &canvas);
    lh_ui_view_set_root(&view, &root);
    lh_ui_canvas_reset_damage(&canvas);

    lh_ui_view_press(&view, point_of(30, 30));
    EXPECT_EQ(lh_ui_view_release(&view, point_of(30, 30)), lh_ui_toggle_as_entity(&toggle))
        << "the click did not reach the toggle";
    EXPECT_TRUE(lh_ui_toggle_get_checked(&toggle));

    damage = lh_ui_canvas_get_damage(&canvas);
    ASSERT_NE(damage, nullptr);
    EXPECT_TRUE(rect_is(*damage, rect_of(14, 14, 52, 32)))
        << "the shadow of the look the toggle arrived on is not in the damage";
}