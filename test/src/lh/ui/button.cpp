#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/button.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{
int g_clicks = 0;
const lh_ui_button_t *g_clicked = lh_null;
lh_ptr g_context = lh_null;

void
count_click(lh_ui_button_t *self, lh_ptr context)
{
    ++g_clicks;
    g_clicked = self;
    g_context = context;
}

/* The styles a button needs are declared here so a test that forgets one cannot
   pass by accident: the resting look paints the middle, the hot one the top,
   and the pressed one of the resting style paints the bottom. */
void
build_styles(lh_ui_style_t *rest, lh_ui_style_t *hot, lh_ui_style_t *pressed)
{
    lh_ui_paint_t paint;
    lh_ui_color_t color;

    lh_ui_style_init(rest);
    lh_ui_color_init(&color, 10, 20, 30, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_set_fill(rest, &paint);
    lh_ui_style_set_radius(rest, lh_ui_scalar(8));

    lh_ui_style_init(hot);
    lh_ui_color_init(&color, 40, 50, 60, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_set_fill(hot, &paint);

    lh_ui_style_init(pressed);
    lh_ui_color_init(&color, 70, 80, 90, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_set_pressed(rest, pressed);
}

void
click(lh_ui_button_t *button)
{
    lh_ui_point_t local;

    lh_ui_point_init(&local, lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_entity_send(lh_ui_button_as_entity(button), lh_ui_entity_event_click, lh_addr_of(local));
}
} // namespace

TEST(entity_button, init_is_an_entity_with_nothing_declared)
{
    lh_ui_button_t button;
    lh_ui_rect_t rect;
    lh_ui_rect_t stored;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);

    stored = lh_ui_entity_get_rect(lh_ui_button_as_entity(lh_addr_of(button)));
    EXPECT_TRUE(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)));
    EXPECT_EQ(lh_ui_entity_get_class(lh_ui_button_as_entity(lh_addr_of(button))),
              lh_addr_of(lh_ui_button_class));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button)))));
    EXPECT_FALSE(lh_ui_button_get_hot(lh_addr_of(button)));
    EXPECT_TRUE(lh_null_eq(lh_ui_button_get_on_click(lh_addr_of(button))));
}

TEST(entity_button, as_button_tells_a_button_from_anything_else)
{
    lh_ui_button_t button;
    lh_ui_entity_t plain;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_entity_init(lh_addr_of(plain), rect);

    EXPECT_EQ(lh_ui_entity_as_button(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(button));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_as_button(lh_addr_of(plain))))
        << "a plain entity was taken for a button";
}

/* The whole point of two styles: the pointer gets its own look, and leaving it
   puts the resting one back without the app touching a pointer. */
TEST(entity_button, a_hot_button_shows_the_hot_style_and_leaves_it)
{
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_style_t pressed;
    lh_ui_rect_t rect;

    build_styles(lh_addr_of(rest), lh_addr_of(hot), lh_addr_of(pressed));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));
    lh_ui_button_set_hot_style(lh_addr_of(button), lh_addr_of(hot));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(rest));

    lh_ui_button_set_hot(lh_addr_of(button), lh_bool_true);
    EXPECT_TRUE(lh_ui_button_get_hot(lh_addr_of(button)));
    EXPECT_EQ(lh_ui_button_get_style_now(lh_addr_of(button)), lh_addr_of(hot));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(hot))
        << "the entity still paints the resting look";

    lh_ui_button_set_hot(lh_addr_of(button), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(rest));
    EXPECT_EQ(lh_ui_button_get_style_now(lh_addr_of(button)), lh_addr_of(rest));
}

/* Press is the engine's and hover is the button's, and the two stack: a hot
   button under the pointer that is also pressed paints the pressed style of the
   hot one, because that is what its entity style is. */
TEST(entity_button, hover_and_press_stack_without_the_button_knowing)
{
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_style_t pressed;
    lh_ui_style_t hot_pressed;
    lh_ui_rect_t rect;

    build_styles(lh_addr_of(rest), lh_addr_of(hot), lh_addr_of(pressed));
    lh_ui_style_init(lh_addr_of(hot_pressed));
    lh_ui_style_set_pressed(lh_addr_of(hot), lh_addr_of(hot_pressed));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));
    lh_ui_button_set_hot_style(lh_addr_of(button), lh_addr_of(hot));

    lh_ui_entity_set_pressed(lh_ui_button_as_entity(lh_addr_of(button)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style_now(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(pressed));

    lh_ui_button_set_hot(lh_addr_of(button), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_style_now(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(hot_pressed))
        << "the pressed look of the hot style is what shows";

    lh_ui_entity_set_pressed(lh_ui_button_as_entity(lh_addr_of(button)), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_get_style_now(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(hot));
}

/* A style set while the pointer is already there shows at once: otherwise the
   button would sit in a look that was asked for and never arrived. */
TEST(entity_button, styles_set_while_hot_show_at_once)
{
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_style_t pressed;
    lh_ui_rect_t rect;

    build_styles(lh_addr_of(rest), lh_addr_of(hot), lh_addr_of(pressed));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));
    lh_ui_button_set_hot(lh_addr_of(button), lh_bool_true);

    lh_ui_button_set_hot_style(lh_addr_of(button), lh_addr_of(hot));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(hot))
        << "the hot style arrived while hovered and did not show";
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(pressed));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(hot))
        << "the resting style took over while the pointer is there";
}

/* Hover with no hot style is a button that only has a pressed look: it changes
   nothing, and must not put an empty style on the entity. */
TEST(entity_button, a_button_with_no_hot_style_ignores_hover)
{
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_style_t pressed;
    lh_ui_rect_t rect;

    build_styles(lh_addr_of(rest), lh_addr_of(hot), lh_addr_of(pressed));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));

    lh_ui_button_set_hot(lh_addr_of(button), lh_bool_true);
    EXPECT_TRUE(lh_ui_button_get_hot(lh_addr_of(button)));
    EXPECT_EQ(lh_ui_button_get_style_now(lh_addr_of(button)), lh_addr_of(rest));
    EXPECT_EQ(lh_ui_entity_get_style(lh_ui_button_as_entity(lh_addr_of(button))), lh_addr_of(rest));
}

TEST(entity_button, a_click_calls_the_callback_with_its_context)
{
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_style_t pressed;
    lh_ui_rect_t rect;
    int context = 7;

    build_styles(lh_addr_of(rest), lh_addr_of(hot), lh_addr_of(pressed));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));
    g_clicks = 0;
    g_clicked = lh_null;
    lh_ui_button_set_on_click(lh_addr_of(button), count_click, lh_addr_of(context));
    EXPECT_TRUE(lh_null_eq(lh_ui_button_get_on_click(lh_addr_of(button))) == lh_bool_false)
        << "the callback was not stored";

    click(lh_addr_of(button));

    EXPECT_EQ(g_clicks, 1);
    EXPECT_EQ(g_clicked, lh_addr_of(button));
    EXPECT_EQ(g_context, lh_addr_of(context));
}

/* No callback is a button that does nothing when clicked: not an assert, not a
   null call. */
TEST(entity_button, a_click_without_a_callback_does_nothing)
{
    lh_ui_button_t button;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    g_clicks = 0;

    click(lh_addr_of(button));

    EXPECT_EQ(g_clicks, 0);
    EXPECT_TRUE(lh_null_eq(lh_ui_button_get_on_click(lh_addr_of(button))));
}

/* A click is a click of the entity: the button does not swallow it, so a parent
   that cares still hears it through the tree. */
TEST(entity_button, a_click_still_reaches_the_tree)
{
    lh_ui_button_t button;
    lh_ui_rect_t rect;
    int clicks = 0;
    lh_ui_point_t local;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_point_init(lh_addr_of(local), lh_ui_scalar(0), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_entity_click(lh_ui_button_as_entity(lh_addr_of(button)), local),
              lh_ui_button_as_entity(lh_addr_of(button)));
    EXPECT_EQ(clicks, 0);
}