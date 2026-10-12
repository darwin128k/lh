#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/container.h>
#include <lh/ui/decl.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/label.h>
#include <lh/ui/image.h>
#include <lh/ui/style.h>
#include <lh/ui/toggle.h>
#include <lh/util/addr.h>

/* Written into the build tree from css2decl.css by scripts/css2decl.py, and compiled
   as C next to this file: the claim under test is that what the generator writes is C
   this library takes, and builds the tree the sheet describes. */
#include "css2decl_generated/ui_generated.h"
#include "css2decl_flow_generated/ui_generated.h"
#include "css2decl_app_generated/ui_generated.h"

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

/* The red channel of a style's fill, or -1 when it has none: the colours in the sheet
   differ in red, so one channel is enough to tell which look this is. */
int
fill_red(const lh_ui_style_t *style)
{
    const lh_ui_color_t *fill;

    if (style == lh_null)
    {
        return -1;
    }
    fill = lh_ui_style_get_fill_color(style);
    return fill == lh_null ? -1 : (int)lh_ui_color_get_r(fill);
}

/* The tree the sheet describes, built once per test from the generated table. */
struct sheet
{
    lh_ui_entity_t *nodes[16];
    lh_ui_entity_t *root;
    lh_ui_decl_fault_t fault;
    alignas(16) lh_byte_t pool[16 * LH_UI_DECL_NODE_MAX];
    lh_bool_t built;

    sheet()
    {
        lh_test_css_init();
        built = lh_ui_decl_build(lh_test_css_decls, lh_test_css_decl_count, nodes, pool, sizeof pool,
                                 lh_addr_of(root), lh_addr_of(fault));
    }

    lh_ui_rect_t
    rect(const char *id)
    {
        return lh_ui_entity_get_rect(lh_test_css_entity(nodes, id));
    }
};

/* Build @p decls, lay out every container that has a flow, parents first, and say
   which rows moved. The generator works layouts out the way the engine does, so the
   table it writes is a tree that is already laid out: nothing moves on the first
   frame, and a row that does is a place the two disagree. */
void
expect_the_first_layout_moves_nothing(const lh_ui_decl_t *decls, lh_u32_t count, const char *const *names)
{
    alignas(16) static lh_byte_t pool[64 * LH_UI_DECL_NODE_MAX];
    lh_ui_entity_t *nodes[64];
    lh_ui_rect_t before[64];
    lh_ui_entity_t *root;
    lh_ui_decl_fault_t fault;
    lh_ui_entity_transform_t transform;
    lh_u32_t i;

    ASSERT_LE(count, 64u);
    ASSERT_EQ(lh_ui_decl_build(decls, count, nodes, pool, sizeof pool, &root, &fault), lh_bool_true);
    for (i = 0; i < count; ++i)
    {
        before[i] = lh_ui_entity_get_rect(nodes[i]);
    }
    for (i = 0; i < count; ++i)
    {
        lh_ui_container_t *box = lh_ui_entity_as_container(nodes[i]);

        if (box != lh_null && lh_ui_container_get_layout(box) != lh_null)
        {
            lh_ui_entity_transform_init(lh_addr_of(transform));
            lh_ui_container_place_children(box, lh_addr_of(transform));
        }
    }
    for (i = 0; i < count; ++i)
    {
        const lh_ui_rect_t after = lh_ui_entity_get_rect(nodes[i]);

        EXPECT_TRUE(rect_is(after, before[i]))
            << "row " << i << " (" << names[i] << ") moved on the first layout: the table has ("
            << (int)before[i].origin.x << "," << (int)before[i].origin.y << " " << (int)before[i].size.width
            << "x" << (int)before[i].size.height << "), the engine ("
            << (int)after.origin.x << "," << (int)after.origin.y << " " << (int)after.size.width << "x"
            << (int)after.size.height << ")";
    }
}

} // namespace

TEST(ui_css2decl, the_generated_table_builds)
{
    sheet s;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_EQ(s.fault.code, lh_ui_decl_error_none);
    EXPECT_EQ(lh_test_css_decl_count, 10u);
    EXPECT_EQ(s.root, s.nodes[0]);
}

TEST(ui_css2decl, a_column_places_its_rows_down_with_its_gap)
{
    sheet s;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_TRUE(rect_is(s.rect("root"), rect_of(0, 0, 400, 300)));
    /* No width: stretched across the column. */
    EXPECT_TRUE(rect_is(s.rect("title"), rect_of(0, 0, 400, 40)));
    EXPECT_TRUE(rect_is(s.rect("body"), rect_of(0, 44, 400, 200)));
    /* `flex: 1`: 300 less 40, 200 and two gaps of 4. */
    EXPECT_TRUE(rect_is(s.rect("list"), rect_of(0, 248, 400, 52)));
}

TEST(ui_css2decl, a_row_flows_inside_its_padding)
{
    sheet s;

    ASSERT_EQ(s.built, lh_bool_true);
    /* `#body` is 400 x 200 with 5 of padding: 390 x 190 of room, starting at (5, 49).
       A 100 button, a 60 toggle, a 16 icon and three gaps of 10 leave 184 to fill. */
    EXPECT_TRUE(rect_is(s.rect("ok"), rect_of(5, 49, 100, 190)));
    EXPECT_TRUE(rect_is(s.rect("sw"), rect_of(115, 49, 60, 190)));
    /* 20 high and centred in 190: (190 - 20 + 1) / 2 = 85 down. */
    EXPECT_TRUE(rect_is(s.rect("icon"), rect_of(185, 134, 16, 20)));
    EXPECT_TRUE(rect_is(s.rect("rest"), rect_of(211, 49, 184, 190)));
}

TEST(ui_css2decl, a_row_placed_by_hand_is_where_left_and_top_say)
{
    sheet s;

    ASSERT_EQ(s.built, lh_bool_true);
    /* `left` and `top` are from the corner of `#list`, which the column put at 248. */
    EXPECT_TRUE(rect_is(s.rect("rows"), rect_of(0, 248, 380, 52)));
    EXPECT_TRUE(rect_is(s.rect("bar"), rect_of(388, 248, 12, 52)));
}

TEST(ui_css2decl, the_first_layout_keeps_every_rect_the_table_has)
{
    lh_test_css_init();
    expect_the_first_layout_moves_nothing(lh_test_css_decls, lh_test_css_decl_count, lh_test_css_names);
}

/* Every way a flow hands out room -- `space-evenly`, `space-around`, weights,
   margins, `margin: 0 auto`, bounds, a hidden row and a padded root -- worked out by
   the generator and then by the engine. */
TEST(ui_css2decl, every_way_a_flow_hands_out_room_is_worked_out_as_the_engine_does)
{
    lh_test_flow_init();
    expect_the_first_layout_moves_nothing(lh_test_flow_decls, lh_test_flow_decl_count, lh_test_flow_names);
}

TEST(ui_css2decl, display_none_is_built_hidden)
{
    alignas(16) static lh_byte_t pool[64 * LH_UI_DECL_NODE_MAX];
    lh_ui_entity_t *nodes[64];
    lh_ui_entity_t *root;
    lh_ui_decl_fault_t fault;

    lh_test_flow_init();
    ASSERT_EQ(lh_ui_decl_build(lh_test_flow_decls, lh_test_flow_decl_count, nodes, pool, sizeof pool, &root,
                               &fault),
              lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_hidden(lh_test_flow_entity(nodes, "g")), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_hidden(lh_test_flow_entity(nodes, "f")), lh_bool_false);
}

TEST(ui_css2decl, a_caption_survives_its_escapes)
{
    sheet s;
    lh_ui_label_t *title;

    ASSERT_EQ(s.built, lh_bool_true);
    title = lh_test_css_label(s.nodes, "title");
    ASSERT_NE(title, lh_null);
    /* `\"` is a quote, braces are text, and `é` is the two bytes of its UTF-8. */
    EXPECT_STREQ(lh_ui_label_get_text(title), "Say \"hi\" { to } \xC3\xA9");
}

TEST(ui_css2decl, an_accessor_answers_null_for_a_row_of_another_kind)
{
    sheet s;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_NE(lh_test_css_button(s.nodes, "ok"), lh_null);
    EXPECT_EQ(lh_test_css_label(s.nodes, "ok"), lh_null);
    EXPECT_EQ(lh_test_css_scrollbar(s.nodes, "nope"), lh_null);
    EXPECT_EQ(lh_test_css_find("nope"), LH_UI_DECL_ROOT);
}

TEST(ui_css2decl, root_variables_are_handed_out_by_name)
{
    sheet s;
    const lh_ui_color_t *fill;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(lh_test_css_colour_accent_dim)), 0x2E);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(lh_test_css_colour_accent_dim)), 0x4A);
    EXPECT_EQ(lh_ui_color_get_b(lh_addr_of(lh_test_css_colour_accent_dim)), 0x6B);

    fill = lh_ui_style_get_fill_color(lh_ui_entity_get_style(s.root));
    ASSERT_NE(fill, lh_null);
    EXPECT_EQ(lh_ui_color_get_r(fill), 0x1B);
    EXPECT_EQ(lh_ui_color_get_b(fill), 0x27);
}

TEST(ui_css2decl, a_hover_look_starts_from_the_row_own)
{
    sheet s;
    lh_ui_button_t *ok;
    const lh_ui_style_t *hot;

    ASSERT_EQ(s.built, lh_bool_true);
    ok = lh_test_css_button(s.nodes, "ok");
    ASSERT_NE(ok, lh_null);
    hot = lh_ui_button_get_hot_style(ok);

    EXPECT_EQ(fill_red(lh_ui_button_get_style(ok)), 0x2E);
    EXPECT_EQ(fill_red(hot), 0xD2);
    /* `:hover` named only the fill; the radius is the button's own, not 0. */
    ASSERT_NE(hot, lh_null);
    EXPECT_EQ(lh_ui_style_get_radius(hot), 6);
}

TEST(ui_css2decl, active_is_the_pressed_look_of_every_look)
{
    sheet s;
    lh_ui_button_t *ok;
    const lh_ui_style_t *pressed;

    ASSERT_EQ(s.built, lh_bool_true);
    ok = lh_test_css_button(s.nodes, "ok");
    ASSERT_NE(ok, lh_null);

    pressed = lh_ui_style_get_pressed(lh_ui_button_get_style(ok));
    EXPECT_EQ(fill_red(pressed), 0x10);
    ASSERT_NE(pressed, lh_null);
    EXPECT_EQ(lh_ui_style_get_radius(pressed), 6);
    /* Pressed while hovered is pressed too. */
    EXPECT_EQ(fill_red(lh_ui_style_get_pressed(lh_ui_button_get_hot_style(ok))), 0x10);
}

TEST(ui_css2decl, a_toggle_gets_all_four_looks_and_its_state)
{
    sheet s;
    lh_ui_toggle_t *sw;
    const lh_ui_style_t *pressed_on;

    ASSERT_EQ(s.built, lh_bool_true);
    sw = lh_test_css_toggle(s.nodes, "sw");
    ASSERT_NE(sw, lh_null);
    EXPECT_EQ(lh_test_css_button(s.nodes, "sw"), lh_null);

    EXPECT_EQ(lh_ui_toggle_get_checked(sw), lh_bool_true);
    EXPECT_EQ(fill_red(lh_ui_toggle_get_off_style(sw)), 0x1B);
    EXPECT_EQ(fill_red(lh_ui_toggle_get_off_hot_style(sw)), 0x33);
    EXPECT_EQ(fill_red(lh_ui_toggle_get_on_style(sw)), 0x2E);
    EXPECT_EQ(fill_red(lh_ui_toggle_get_on_hot_style(sw)), 0xD2);

    /* `:active` said only the radius, so pressed-while-on keeps the on fill. */
    pressed_on = lh_ui_style_get_pressed(lh_ui_toggle_get_on_style(sw));
    ASSERT_NE(pressed_on, lh_null);
    EXPECT_EQ(lh_ui_style_get_radius(pressed_on), 2);
    EXPECT_EQ(fill_red(pressed_on), 0x2E);
}

TEST(ui_css2decl, an_image_color_is_its_tint)
{
    sheet s;
    lh_ui_image_t *icon;
    const lh_ui_color_t *tint;

    ASSERT_EQ(s.built, lh_bool_true);
    icon = lh_test_css_image(s.nodes, "icon");
    ASSERT_NE(icon, lh_null);
    tint = lh_ui_image_get_tint(icon);
    ASSERT_NE(tint, lh_null);
    EXPECT_EQ(lh_ui_color_get_r(tint), 0xD2);
    EXPECT_EQ(lh_ui_color_get_b(tint), 0xE2);
}

/* -- A screen written the way the web writes one ----------------------------- */

namespace
{
struct app_screen
{
    lh_ui_entity_t *nodes[64];
    lh_ui_entity_t *root;
    lh_ui_decl_fault_t fault;
    alignas(16) lh_byte_t pool[64 * LH_UI_DECL_NODE_MAX];
    lh_bool_t built;

    app_screen()
    {
        lh_test_app_init();
        built = lh_ui_decl_build(lh_test_app_decls, lh_test_app_decl_count, nodes, pool, sizeof pool,
                                 lh_addr_of(root), lh_addr_of(fault));
    }
};
} // namespace

TEST(ui_css2decl_app, a_screen_in_html_names_builds)
{
    app_screen s;

    ASSERT_EQ(s.built, lh_bool_true) << "row " << s.fault.index << " refused, code " << (int)s.fault.code;
    /* `div` is a box, `h1` a label, `checkbox` a toggle, `input` a field. */
    EXPECT_NE(lh_test_app_container(s.nodes, "app"), lh_null);
    EXPECT_NE(lh_test_app_label(s.nodes, "title"), lh_null);
    EXPECT_NE(lh_test_app_toggle(s.nodes, "c1"), lh_null);
    EXPECT_NE(lh_test_app_input(s.nodes, "name"), lh_null);
}

TEST(ui_css2decl_app, a_button_with_text_holds_its_caption)
{
    app_screen s;
    lh_ui_label_t *caption;

    ASSERT_EQ(s.built, lh_bool_true);
    caption = lh_test_app_label(s.nodes, "add__text");
    ASSERT_NE(caption, lh_null);
    EXPECT_STREQ(lh_ui_label_get_text(caption), "Add device");
    EXPECT_EQ(lh_ui_entity_get_parent(lh_ui_label_as_entity(caption)),
              lh_ui_button_as_entity(lh_test_app_button(s.nodes, "add")));
}

TEST(ui_css2decl_app, a_checkbox_with_text_is_its_box_and_its_caption_side_by_side)
{
    app_screen s;
    lh_ui_toggle_t *box;

    ASSERT_EQ(s.built, lh_bool_true);
    box = lh_test_app_toggle(s.nodes, "c1");
    ASSERT_NE(box, lh_null);
    EXPECT_EQ(lh_ui_toggle_get_checked(box), lh_bool_true);
    EXPECT_STREQ(lh_ui_label_get_text(lh_test_app_label(s.nodes, "c1__text")), "Sensor A");
    EXPECT_EQ(lh_ui_entity_get_parent(lh_ui_toggle_as_entity(box)), lh_test_app_entity(s.nodes, "c1__row"));
}

TEST(ui_css2decl_app, a_list_that_overflows_has_a_scroll_bar_driving_it)
{
    app_screen s;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_NE(lh_test_app_scrollbar(s.nodes, "list__vbar"), lh_null);
    /* The list is the frame less the bar. */
    EXPECT_TRUE(rect_is(lh_ui_entity_get_rect(lh_test_app_entity(s.nodes, "list__frame")),
                        rect_of(12, 54, 456, 216)));
    EXPECT_TRUE(rect_is(lh_ui_entity_get_rect(lh_test_app_entity(s.nodes, "list")), rect_of(12, 54, 444, 216)));
}

TEST(ui_css2decl_app, a_field_starts_with_its_value)
{
    app_screen s;

    ASSERT_EQ(s.built, lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_test_app_input(s.nodes, "name")), "New device");
}

TEST(ui_css2decl_app, a_row_placed_by_hand_over_a_flow_stays_where_it_is_after_a_layout)
{
    app_screen s;
    lh_ui_entity_transform_t transform;
    lh_u32_t i;

    ASSERT_EQ(s.built, lh_bool_true);
    for (i = 0; i < lh_test_app_decl_count; ++i)
    {
        lh_ui_container_t *box = lh_ui_entity_as_container(s.nodes[i]);

        if (box != lh_null && lh_ui_container_get_layout(box) != lh_null)
        {
            lh_ui_entity_transform_init(lh_addr_of(transform));
            lh_ui_container_place_children(box, lh_addr_of(transform));
        }
    }
    /* `right: 12px; bottom: 50px` of the 480 x 320 screen. */
    EXPECT_TRUE(rect_is(lh_ui_entity_get_rect(lh_test_app_entity(s.nodes, "toast")), rect_of(348, 240, 120, 30)));
}
