#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/button.h>
#include <lh/ui/container.h>
#include <lh/ui/image.h>
#include <lh/ui/label.h>
#include <lh/ui/layout.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_of;

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

/* A button of the three-object kind, with the picture and the caption placed by
   hand: this fixture is about which node takes a click, not about the flow. */
struct captioned_fixture
{
    static const lh_byte_t g_block[8]; /* 4 x 2 at 8 bpp: a solid block */

    lh_ui_entity_t root;
    lh_ui_button_t button;
    lh_ui_image_t picture;
    lh_ui_label_t caption;

    captioned_fixture()
    {
        lh_ui_mask_t mask;
        lh_ui_rect_t rect;

        lh_ui_mask_init(&mask, g_block, 4, 2, 4, 8);
        lh_ui_entity_init(&root, rect_of(0, 0, 200, 60));
        lh_ui_rect_init(&rect, 10, 10, 120, 30);
        lh_ui_button_init(&button, rect);
        lh_ui_button_set_on_click(&button, count_click, lh_null);
        lh_ui_image_init(&picture, rect_of(20, 20, 4, 2), &mask);
        lh_ui_label_init(&caption, rect_of(60, 20, 40, 10), "Hi");
        lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&button));
        lh_ui_entity_add_child(lh_ui_container_as_entity(lh_ui_button_as_container(&button)),
                               lh_ui_image_as_entity(&picture));
        lh_ui_entity_add_child(lh_ui_container_as_entity(lh_ui_button_as_container(&button)),
                               lh_ui_label_as_entity(&caption));
        g_clicks = 0;
        g_clicked = lh_null;
    }
};
const lh_byte_t captioned_fixture::g_block[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;

    lh_ui_point_init(&point, lh_ui_scalar(x), lh_ui_scalar(y));
    return point;
}

/* The three-object button, and the reason the pointer over a caption or a picture
   has to be asked about rather than taken: they belong to the button. */
TEST(entity_button, a_caption_and_a_picture_hand_the_pointer_to_their_button)
{
    captioned_fixture f;

    EXPECT_EQ(lh_ui_entity_click_target(lh_ui_label_as_entity(&f.caption)),
              lh_ui_button_as_entity(&f.button));
    EXPECT_EQ(lh_ui_entity_click_target(lh_ui_image_as_entity(&f.picture)),
              lh_ui_button_as_entity(&f.button));
}

TEST(entity_button, a_click_on_the_caption_reaches_the_button)
{
    captioned_fixture f;
    lh_ui_point_t on_text = point_of(70, 24);

    EXPECT_EQ(lh_ui_entity_click(&f.root, on_text), lh_ui_button_as_entity(&f.button))
        << "the caption took the click away from the button";
    EXPECT_EQ(g_clicks, 1);
    EXPECT_EQ(g_clicked, lh_addr_of(f.button));
}

TEST(entity_button, a_click_on_the_picture_reaches_the_button)
{
    captioned_fixture f;
    lh_ui_point_t on_picture = point_of(21, 20);

    EXPECT_EQ(lh_ui_entity_click(&f.root, on_picture), lh_ui_button_as_entity(&f.button));
    EXPECT_EQ(g_clicks, 1);
}

/* Nobody claims it, so a click still lands where it was pointed: a plain entity is
   not clickable, and an app that wants to hear about a click on one still does. */
TEST(entity_button, a_click_nobody_claims_still_lands_where_it_pointed)
{
    lh_ui_entity_t root;
    lh_ui_label_t caption;

    lh_ui_entity_init(&root, rect_of(0, 0, 200, 60));
    lh_ui_label_init(&caption, rect_of(20, 20, 40, 10), "Hi");
    lh_ui_entity_add_child(&root, lh_ui_label_as_entity(&caption));

    EXPECT_EQ(lh_ui_entity_click_target(lh_ui_label_as_entity(&caption)),
              lh_ui_label_as_entity(&caption));
}

/* A button that cannot hold a caption is not the button this project means. The
   flow that places a picture and a caption has to come from somewhere, and the only
   somewhere it comes from is the container the button is. */
TEST(entity_button, a_button_is_the_container_its_flow_comes_from)
{
    lh_ui_button_t button;
    lh_ui_layout_t layout;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 40, 20);
    lh_ui_button_init(lh_addr_of(button), rect);
    lh_ui_layout_init(lh_addr_of(layout), lh_ui_axis_horizontal, lh_ui_scalar(6));

    EXPECT_EQ(lh_ui_container_as_entity(lh_ui_button_as_container(lh_addr_of(button))),
              lh_ui_button_as_entity(lh_addr_of(button)));
    EXPECT_TRUE(lh_null_eq(lh_ui_container_get_layout(lh_ui_button_as_container(lh_addr_of(button)))))
        << "a fresh button was handed a flow of its own";

    lh_ui_container_set_layout(lh_ui_button_as_container(lh_addr_of(button)), lh_addr_of(layout));
    EXPECT_EQ(lh_ui_container_get_layout(lh_ui_button_as_container(lh_addr_of(button))),
              lh_addr_of(layout));
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