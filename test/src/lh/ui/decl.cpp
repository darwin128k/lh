#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/tiny_font.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/decl.h>
#include <lh/ui/entity.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::draw_log;
using lh_test::draw_log_init;
using lh_test::rect_is;
using lh_test::rect_of;
using lh_test::tiny_font;

/* Rows of the table, and of the tree it builds. The hand-written tree uses the same
   order, so a difference in row N is a difference in the same widget. */
const int k_rows = 9;

int g_clicks = 0;
lh_bool_t g_clicked_a_button = lh_bool_false;

void
count_click(lh_ui_button_t *self, lh_ptr context)
{
    (void)context;
    ++g_clicks;
    g_clicked_a_button = (self != lh_null) ? lh_bool_true : lh_bool_false;
}

/* Everything both trees are built from, so that "the same tree" is a statement
   about the build and not about two different sets of materials. */
struct scene
{
    lh_ui_color_t root_color, card_color, action_color, action_hot_color, text_color, ink;
    lh_ui_paint_t root_paint, card_paint, action_paint, action_hot_paint, text_paint;
    lh_ui_style_t root_style, card_style, action_style, action_hot_style, text_style;
    lh_ui_layout_t column;
    lh_ui_layout_t button_flow;
    lh_ui_place_t fixed20;
    lh_ui_place_t wrap_center;
    lh_byte_t glyph[8];
    lh_ui_mask_t mask;

    scene()
    {
        lh_ui_color_init(&root_color, 20, 22, 28, 255);
        lh_ui_paint_init_color(&root_paint, &root_color);
        lh_ui_style_init(&root_style);
        lh_ui_style_set_fill(&root_style, &root_paint);

        lh_ui_color_init(&card_color, 60, 120, 200, 255);
        lh_ui_paint_init_color(&card_paint, &card_color);
        lh_ui_style_init(&card_style);
        lh_ui_style_set_fill(&card_style, &card_paint);
        lh_ui_style_set_radius(&card_style, lh_ui_scalar(6));

        lh_ui_color_init(&action_color, 70, 76, 88, 255);
        lh_ui_paint_init_color(&action_paint, &action_color);
        lh_ui_color_init(&action_hot_color, 90, 98, 112, 255);
        lh_ui_paint_init_color(&action_hot_paint, &action_hot_color);
        lh_ui_style_init(&action_style);
        lh_ui_style_set_fill(&action_style, &action_paint);
        lh_ui_style_set_radius(&action_style, lh_ui_scalar(8));
        lh_ui_style_init(&action_hot_style);
        lh_ui_style_set_fill(&action_hot_style, &action_hot_paint);

        lh_ui_color_init(&text_color, 220, 226, 236, 255);
        lh_ui_paint_init_color(&text_paint, &text_color);
        lh_ui_style_init(&text_style);
        lh_ui_style_set_font(&text_style, tiny_font());
        lh_ui_style_set_text(&text_style, &text_paint);

        lh_ui_color_init(&ink, 255, 255, 255, 255);
        for (int i = 0; i < 8; ++i)
        {
            glyph[i] = 0xFF;
        }
        lh_ui_mask_init(&mask, glyph, 4, 2, 4, 8);

        lh_ui_layout_init(&column, lh_ui_axis_vertical, lh_ui_scalar(4));
        lh_ui_layout_init(&button_flow, lh_ui_axis_horizontal, lh_ui_scalar(6));
        lh_ui_layout_set_justify(&button_flow, lh_ui_justify_center);
        lh_ui_place_init(&fixed20, lh_ui_place_size_fixed, lh_ui_scalar(20));
        lh_ui_place_set_align(&fixed20, lh_ui_place_align_fill);
        lh_ui_place_init(&wrap_center, lh_ui_place_size_wrap, lh_ui_scalar(0));
        lh_ui_place_set_align(&wrap_center, lh_ui_place_align_center);
    }
};

/* What two trees can be compared by: the rect of every node, the class of every
   node, what the canvas was sent, and who took a click. */
struct built
{
    lh_ui_rect_t rects[k_rows];
    const lh_ui_entity_class_t *classes[k_rows];
    draw_log log;
    lh_ui_entity_t *clicked;
    int clicks;
};

void
snapshot(built *out, lh_ui_entity_t **nodes, lh_ui_entity_t *root, lh_ui_canvas_t *canvas)
{
    /* Ask the root for its children the way a frame does: that is the step that
       places a flow, so the rects compared below are the ones a drawn frame uses. */
    lh_ui_point_t offset;
    lh_ui_point_t on_caption;

    lh_ui_entity_get_children_transform(root, &offset);
    for (int i = 0; i < k_rows; ++i)
    {
        out->rects[i] = lh_ui_entity_get_rect(nodes[i]);
        out->classes[i] = lh_ui_entity_get_class(nodes[i]);
    }
    draw_log_init(&out->log, canvas, true, false, true);
    lh_ui_entity_draw(root, canvas);

    /* A click on the caption of the button, which is row 6: this is the row that
       says whether the pointer reaches the button or only the text on it. */
    g_clicks = 0;
    g_clicked_a_button = lh_bool_false;
    lh_ui_point_init(&on_caption, lh_ui_scalar(70), lh_ui_scalar(88));
    out->clicked = lh_ui_entity_click(root, on_caption);
    out->clicks = g_clicks;
}

/* The tree as an app writes it: one variable per widget, and an array of the
   entities behind them so the two trees can be walked in the same order. */
struct hand_tree
{
    lh_ui_entity_t background;
    lh_ui_container_t box;
    lh_ui_label_t row_one;
    lh_ui_label_t row_two;
    lh_ui_button_t action;
    lh_ui_image_t icon;
    lh_ui_label_t caption;
    lh_ui_scrollbar_t bar;
    lh_ui_toggle_t sw;
    lh_ui_canvas_t canvas;
};

/* The same tree, written the way an app writes it. */
void
build_by_hand(scene &s, hand_tree &tree, lh_ui_entity_t **nodes, lh_ui_entity_t **root)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 200, 140);
    lh_ui_entity_init(&tree.background, rect);
    lh_ui_entity_set_style(&tree.background, &s.root_style);

    lh_ui_rect_init(&rect, 10, 10, 160, 60);
    lh_ui_container_init(&tree.box, rect);
    lh_ui_entity_set_style(lh_ui_container_as_entity(&tree.box), &s.card_style);
    lh_ui_container_set_layout(&tree.box, &s.column);
    lh_ui_entity_add_child(&tree.background, lh_ui_container_as_entity(&tree.box));

    lh_ui_rect_init(&rect, 0, 0, 150, 20);
    lh_ui_label_init(&tree.row_one, rect, "row one");
    lh_ui_entity_set_place(lh_ui_label_as_entity(&tree.row_one), &s.fixed20);
    lh_ui_entity_add_child(lh_ui_container_as_entity(&tree.box), lh_ui_label_as_entity(&tree.row_one));

    lh_ui_rect_init(&rect, 0, 0, 150, 20);
    lh_ui_label_init(&tree.row_two, rect, "row two");
    lh_ui_entity_set_place(lh_ui_label_as_entity(&tree.row_two), &s.fixed20);
    lh_ui_entity_add_child(lh_ui_container_as_entity(&tree.box), lh_ui_label_as_entity(&tree.row_two));

    lh_ui_rect_init(&rect, 10, 80, 140, 30);
    lh_ui_button_init(&tree.action, rect);
    lh_ui_button_set_style(&tree.action, &s.action_style);
    lh_ui_button_set_hot_style(&tree.action, &s.action_hot_style);
    lh_ui_button_set_on_click(&tree.action, count_click, lh_null);
    lh_ui_container_set_layout(lh_ui_button_as_container(&tree.action), &s.button_flow);
    lh_ui_entity_add_child(&tree.background, lh_ui_button_as_entity(&tree.action));

    lh_ui_rect_init(&rect, 0, 0, 4, 2);
    lh_ui_image_init(&tree.icon, rect, &s.mask);
    lh_ui_image_set_tint(&tree.icon, &s.ink);
    lh_ui_entity_set_place(lh_ui_image_as_entity(&tree.icon), &s.wrap_center);
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_ui_button_as_container(&tree.action)),
                           lh_ui_image_as_entity(&tree.icon));

    lh_ui_rect_init(&rect, 0, 0, 60, 14);
    lh_ui_label_init(&tree.caption, rect, "Hide panel");
    lh_ui_entity_set_style(lh_ui_label_as_entity(&tree.caption), &s.text_style);
    lh_ui_entity_set_place(lh_ui_label_as_entity(&tree.caption), &s.wrap_center);
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_ui_button_as_container(&tree.action)),
                           lh_ui_label_as_entity(&tree.caption));

    lh_ui_rect_init(&rect, 180, 10, 10, 60);
    lh_ui_scrollbar_init(&tree.bar, rect, lh_ui_axis_vertical, &tree.box);
    lh_ui_scrollbar_set_mode(&tree.bar, lh_ui_scrollbar_mode_always);
    lh_ui_entity_add_child(&tree.background, lh_ui_scrollbar_as_entity(&tree.bar));

    lh_ui_rect_init(&rect, 160, 110, 24, 24);
    lh_ui_toggle_init(&tree.sw, rect);
    lh_ui_toggle_set_off_style(&tree.sw, &s.card_style, &s.action_style);
    lh_ui_toggle_set_on_style(&tree.sw, &s.action_style, &s.action_hot_style);
    lh_ui_entity_add_child(&tree.background, lh_ui_toggle_as_entity(&tree.sw));

    nodes[0] = &tree.background;
    nodes[1] = lh_ui_container_as_entity(&tree.box);
    nodes[2] = lh_ui_label_as_entity(&tree.row_one);
    nodes[3] = lh_ui_label_as_entity(&tree.row_two);
    nodes[4] = lh_ui_button_as_entity(&tree.action);
    nodes[5] = lh_ui_image_as_entity(&tree.icon);
    nodes[6] = lh_ui_label_as_entity(&tree.caption);
    nodes[7] = lh_ui_scrollbar_as_entity(&tree.bar);
    nodes[8] = lh_ui_toggle_as_entity(&tree.sw);
    *root = &tree.background;
    lh_ui_canvas_init(&tree.canvas, lh_addr_of(lh_ui_canvas_backend_null), lh_null);
}

/* The declaration. It says what the tree **is**: who is a child of whom, where it
   sits, which flow places it and what it carries. Every material — a style, a text,
   a mask, a flow, a callback — is the app's own object, named here by address. */
const lh_ui_decl_t g_rows[k_rows] = {
    {lh_ui_decl_kind_entity, 0, 0, 200, 140, LH_UI_DECL_ROOT, nullptr, nullptr, nullptr, {}},
    {lh_ui_decl_kind_container, 10, 10, 160, 60, 0, nullptr, nullptr, nullptr, {}},
    {lh_ui_decl_kind_label, 0, 0, 150, 20, 1, nullptr, nullptr, nullptr, {.label = {"row one"}}},
    {lh_ui_decl_kind_label, 0, 0, 150, 20, 1, nullptr, nullptr, nullptr, {.label = {"row two"}}},
    {lh_ui_decl_kind_button, 10, 80, 140, 30, 0, nullptr, nullptr, nullptr,
     {.button = {nullptr, count_click, lh_null}}},
    {lh_ui_decl_kind_image, 0, 0, 4, 2, 4, nullptr, nullptr, nullptr, {.image = {nullptr, nullptr}}},
    {lh_ui_decl_kind_label, 0, 0, 60, 14, 4, nullptr, nullptr, nullptr, {.label = {"Hide panel"}}},
    {lh_ui_decl_kind_scrollbar, 180, 10, 10, 60, 0, nullptr, nullptr, nullptr,
     {.scrollbar = {1, lh_ui_axis_vertical, lh_ui_scrollbar_mode_always}}},
    {lh_ui_decl_kind_toggle, 160, 110, 24, 24, 0, nullptr, nullptr, nullptr, {.toggle = {}}},
};

/* The materials of the tree, filled into a copy of the table: styles, flows, places,
   a mask and a callback — every one of them the app's own object. */
void
fill_rows(scene &s, lh_ui_decl_t *rows)
{
    for (int i = 0; i < k_rows; ++i)
    {
        rows[i] = g_rows[i];
    }
    rows[0].style = &s.root_style;
    rows[1].style = &s.card_style;
    rows[1].layout = &s.column;
    rows[2].place = &s.fixed20;
    rows[3].place = &s.fixed20;
    rows[4].style = &s.action_style;
    rows[4].layout = &s.button_flow;
    rows[4].as.button.hot_style = &s.action_hot_style;
    rows[5].place = &s.wrap_center;
    rows[5].as.image.mask = &s.mask;
    rows[5].as.image.tint = &s.ink;
    rows[6].style = &s.text_style;
    rows[6].place = &s.wrap_center;
    rows[8].as.toggle.off_style = &s.card_style;
    rows[8].as.toggle.off_hot_style = &s.action_style;
    rows[8].as.toggle.on_style = &s.action_style;
    rows[8].as.toggle.on_hot_style = &s.action_hot_style;
}

lh_byte_t g_storage[k_rows * (int)LH_UI_DECL_NODE_MAX];

bool
build_by_decl(scene &s, lh_ui_entity_t **nodes, lh_ui_entity_t **root, lh_ui_canvas_t *canvas,
              lh_ui_decl_fault_t *fault)
{
    lh_ui_decl_t rows[k_rows];
    lh_bool_t ok;

    fill_rows(s, rows);
    ok = lh_ui_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), root, fault);
    lh_ui_canvas_init(canvas, lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    return ok == lh_bool_true;
}

} // namespace

/* The contract this whole file exists for: a tree built from a table is the tree
   the same app would have written by hand. Same rects, same classes, same pixels,
   same click. */
TEST(ui_decl, a_tree_from_a_table_is_the_tree_written_by_hand)
{
    scene hand_materials;
    scene decl_materials;
    hand_tree tree;
    lh_ui_entity_t *hand_nodes[k_rows] = {};
    lh_ui_entity_t *decl_nodes[k_rows] = {};
    lh_ui_entity_t *hand_root = nullptr;
    lh_ui_entity_t *decl_root = nullptr;
    lh_ui_canvas_t decl_canvas;
    lh_ui_decl_fault_t fault;
    built hand;
    built from_decl;

    build_by_hand(hand_materials, tree, hand_nodes, &hand_root);
    ASSERT_TRUE(build_by_decl(decl_materials, decl_nodes, &decl_root, &decl_canvas, &fault) !=
                lh_bool_false)
        << "the declaration was refused: code " << (int)fault.code << " row " << (int)fault.index;
    snapshot(&hand, hand_nodes, hand_root, &tree.canvas);
    snapshot(&from_decl, decl_nodes, decl_root, &decl_canvas);

    for (int i = 0; i < k_rows; ++i)
    {
        EXPECT_EQ(hand.classes[i], from_decl.classes[i]) << "row " << i << " is a different widget";
        if (!rect_is(hand.rects[i], from_decl.rects[i]))
        {
            const lh_ui_rect_t *a = &hand.rects[i];
            const lh_ui_rect_t *b = &from_decl.rects[i];
            fprintf(stderr,
                    "        row %d: hand (%d,%d %dx%d), declared (%d,%d %dx%d)\n", i,
                    (int)lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(*a))),
                    (int)lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(*a))),
                    (int)lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(*a))),
                    (int)lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(*a))),
                    (int)lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(*b))),
                    (int)lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(*b))),
                    (int)lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(*b))),
                    (int)lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(*b))));
            ADD_FAILURE() << "row " << i << " is not where the hand-written tree put it";
        }
    }

    ASSERT_EQ(hand.log.fill_count, from_decl.log.fill_count)
        << "the two trees sent a different number of fills to the canvas";
    for (int i = 0; i < hand.log.fill_count; ++i)
    {
        EXPECT_TRUE(rect_is(hand.log.fills[i], from_decl.log.fills[i])) << "fill " << i;
    }
    ASSERT_EQ(hand.log.round_count, from_decl.log.round_count)
        << "the two trees sent a different number of rounded fills";
    ASSERT_EQ(hand.log.mask_count, from_decl.log.mask_count)
        << "the picture landed a different number of times";
    for (int i = 0; i < hand.log.mask_count; ++i)
    {
        EXPECT_TRUE(rect_is(hand.log.masks[i], from_decl.log.masks[i])) << "picture " << i;
    }

    EXPECT_EQ(from_decl.clicked, decl_nodes[4]) << "the click did not reach the button";
    EXPECT_EQ(from_decl.clicks, 1) << "the button callback did not fire exactly once";
    EXPECT_EQ(g_clicked_a_button, lh_bool_true);
    EXPECT_EQ(hand.clicks, from_decl.clicks) << "the hand-written tree answered the click differently";
    EXPECT_EQ(hand.clicked, hand_nodes[4]) << "the hand-written tree sent the click somewhere else";
}

/* A table is data, so a typo in one row has to say which row. Every refusal below
   is a mistake an app actually makes. */
TEST(ui_decl, a_child_cannot_be_declared_before_the_thing_that_holds_it)
{
    scene materials;
    lh_ui_entity_t *nodes[k_rows] = {};
    lh_ui_entity_t *root = nullptr;
    lh_ui_decl_fault_t fault;
    lh_ui_decl_t rows[k_rows];

    fill_rows(materials, rows);
    rows[2].parent = 4; /* row 2 claims a parent that is declared after it */

    EXPECT_FALSE(lh_ui_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(fault.code, lh_ui_decl_error_parent);
    EXPECT_EQ(fault.index, 2u) << "the refusal did not name the row that is wrong";
}

TEST(ui_decl, a_table_with_no_root_is_not_a_tree)
{
    lh_ui_entity_t *nodes[2] = {};
    lh_ui_entity_t *root = nullptr;
    lh_ui_decl_fault_t fault;
    lh_ui_decl_t rows[2] = {};

    rows[0].kind = lh_ui_decl_kind_entity;
    rows[0].parent = LH_UI_DECL_ROOT;
    rows[1].kind = lh_ui_decl_kind_label;
    rows[1].parent = LH_UI_DECL_ROOT; /* two roots: not a mistake a build can guess at */

    EXPECT_FALSE(lh_ui_decl_build(rows, 2, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(fault.code, lh_ui_decl_error_roots);
}

TEST(ui_decl, a_pool_too_small_for_the_rows_is_refused_not_overrun)
{
    scene materials;
    lh_ui_entity_t *nodes[k_rows] = {};
    lh_ui_entity_t *root = nullptr;
    lh_ui_decl_fault_t fault;
    lh_ui_decl_t rows[k_rows];
    lh_byte_t small[k_rows * 16];

    fill_rows(materials, rows);
    /* A pool for a few rows: the build measures what the rows need before it writes
       any of them, so this is a refusal and not a row outside the buffer. */
    EXPECT_FALSE(lh_ui_decl_build(rows, k_rows, nodes, small, (lh_u32_t)sizeof(small), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(fault.code, lh_ui_decl_error_storage) << "the pool was written past instead of refused";
}

TEST(ui_decl, a_scrollbar_naming_a_row_that_is_not_a_container_is_refused)
{
    scene materials;
    lh_ui_entity_t *nodes[k_rows] = {};
    lh_ui_entity_t *root = nullptr;
    lh_ui_decl_fault_t fault;
    lh_ui_decl_t rows[k_rows];

    fill_rows(materials, rows);
    rows[7].as.scrollbar.box = 5; /* row 5 is a picture, not a container */

    EXPECT_FALSE(lh_ui_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(fault.code, lh_ui_decl_error_box);
    EXPECT_EQ(fault.index, 7u);
}

TEST(ui_decl, an_unknown_kind_is_refused_with_its_row)
{
    lh_ui_entity_t *nodes[1] = {};
    lh_ui_entity_t *root = nullptr;
    lh_ui_decl_fault_t fault;
    lh_ui_decl_t rows[1] = {};

    rows[0].kind = (lh_ui_decl_kind_t)42;
    rows[0].parent = LH_UI_DECL_ROOT;

    EXPECT_FALSE(lh_ui_decl_build(rows, 1, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(fault.code, lh_ui_decl_error_kind);
    EXPECT_EQ(fault.index, 0u);
}