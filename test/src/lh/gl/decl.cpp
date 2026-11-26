#include <gtest/gtest.h>

#include <lh/test/gl/draw_log.h>
#include <lh/test/gl/tiny_font.h>

#include <lh/gl/decl.h>
#include <lh/test/out.h>

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
count_click(lh_gl_button_t *self, lh_ptr context)
{
    (void)context;
    ++g_clicks;
    g_clicked_a_button = (self != lh_null) ? lh_bool_true : lh_bool_false;
}

/* Everything both trees are built from, so that "the same tree" is a statement
   about the build and not about two different sets of materials. */
struct scene
{
    lh_gl_color_t root_color, card_color, action_color, action_hot_color, text_color, ink;
    lh_gl_paint_t root_paint, card_paint, action_paint, action_hot_paint, text_paint;
    lh_gl_style_t root_style, card_style, action_style, action_hot_style, text_style;
    lh_gl_layout_t column;
    lh_gl_layout_t button_flow;
    lh_gl_place_t fixed20;
    lh_gl_place_t wrap_center;
    lh_byte_t glyph[8];
    lh_gl_mask_t mask;

    scene()
    {
        lh_gl_color_init(&root_color, 20, 22, 28, 255);
        lh_gl_paint_init_color(&root_paint, &root_color);
        lh_gl_style_init(&root_style);
        lh_gl_style_assign_fill(&root_style, &root_paint);

        lh_gl_color_init(&card_color, 60, 120, 200, 255);
        lh_gl_paint_init_color(&card_paint, &card_color);
        lh_gl_style_init(&card_style);
        lh_gl_style_assign_fill(&card_style, &card_paint);
        lh_gl_style_set_radius_v(&card_style, lh_gl_scalar(6));

        lh_gl_color_init(&action_color, 70, 76, 88, 255);
        lh_gl_paint_init_color(&action_paint, &action_color);
        lh_gl_color_init(&action_hot_color, 90, 98, 112, 255);
        lh_gl_paint_init_color(&action_hot_paint, &action_hot_color);
        lh_gl_style_init(&action_style);
        lh_gl_style_assign_fill(&action_style, &action_paint);
        lh_gl_style_set_radius_v(&action_style, lh_gl_scalar(8));
        lh_gl_style_init(&action_hot_style);
        lh_gl_style_assign_fill(&action_hot_style, &action_hot_paint);

        lh_gl_color_init(&text_color, 220, 226, 236, 255);
        lh_gl_paint_init_color(&text_paint, &text_color);
        lh_gl_style_init(&text_style);
        lh_gl_style_set_font(&text_style, tiny_font());
        lh_gl_style_assign_text(&text_style, &text_paint);

        lh_gl_color_init(&ink, 255, 255, 255, 255);
        for (int i = 0; i < 8; ++i)
        {
            glyph[i] = 0xFF;
        }
        lh_gl_mask_init(&mask, glyph, 4, 2, 4, 8);

        lh_gl_layout_init(&column, lh_gl_axis_vertical, lh_gl_scalar(4));
        lh_gl_layout_init(&button_flow, lh_gl_axis_horizontal, lh_gl_scalar(6));
        lh_gl_layout_set_justify(&button_flow, lh_gl_justify_center);
        lh_gl_place_init(&fixed20, lh_gl_place_size_fixed, lh_gl_scalar(20));
        lh_gl_place_set_align(&fixed20, lh_gl_place_align_fill);
        lh_gl_place_init(&wrap_center, lh_gl_place_size_wrap, lh_gl_scalar(0));
        lh_gl_place_set_align(&wrap_center, lh_gl_place_align_center);
    }
};

/* What two trees can be compared by: the rect of every node, the class of every
   node, what the canvas was sent, and who took a click. */
struct built
{
    lh_gl_rect_t rects[k_rows];
    const lh_gl_entity_class_t *classes[k_rows];
    draw_log log;
    lh_gl_entity_t *clicked;
    int clicks;
};

void
snapshot(built *out, lh_gl_entity_t **nodes, lh_gl_entity_t *root, lh_gl_canvas_t *canvas)
{
    /* Ask the root for its children the way a frame does: that is the step that
       places a flow, so the rects compared below are the ones a drawn frame uses. */
    lh_gl_point_t offset;

    lh_gl_entity_get_children_transform(root, &offset);
    for (int i = 0; i < k_rows; ++i)
    {
        out->rects[i] = lh_test::out(lh_gl_entity_copy_rect, nodes[i]);
        out->classes[i] = lh_gl_entity_get_class(nodes[i]);
    }
    draw_log_init(&out->log, canvas, true, false, true);
    lh_gl_entity_draw(root, canvas);

    /* A click on the caption of the button, which is row 6: this is the row that
       says whether the pointer reaches the button or only the text on it. */
    g_clicks = 0;
    g_clicked_a_button = lh_bool_false;
    lh_gl_point_t on_caption;
    lh_gl_point_init(&on_caption, lh_gl_scalar(70), lh_gl_scalar(88));
    out->clicked = lh_gl_entity_click(root, on_caption);
    out->clicks = g_clicks;
}

/* The tree as an app writes it: one variable per widget, and an array of the
   entities behind them so the two trees can be walked in the same order. */
struct hand_tree
{
    lh_gl_entity_t background;
    lh_gl_container_t box;
    lh_gl_label_t row_one;
    lh_gl_label_t row_two;
    lh_gl_button_t action;
    lh_gl_image_t icon;
    lh_gl_label_t caption;
    lh_gl_scrollbar_t bar;
    lh_gl_toggle_t sw;
    lh_gl_canvas_t canvas;
};

/* The same tree, written the way an app writes it. */
void
build_by_hand(scene &s, hand_tree &tree, lh_gl_entity_t **nodes, lh_gl_entity_t **root)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(&rect, 0, 0, 200, 140);
    lh_gl_entity_init(&tree.background, rect);
    lh_gl_entity_set_style_v(&tree.background, &s.root_style);

    lh_gl_rect_init(&rect, 10, 10, 160, 60);
    lh_gl_container_init(&tree.box, rect);
    lh_gl_entity_set_style_v(lh_gl_container_as_entity(&tree.box), &s.card_style);
    lh_gl_container_set_layout(&tree.box, &s.column);
    lh_gl_entity_add_child(&tree.background, lh_gl_container_as_entity(&tree.box));

    lh_gl_rect_init(&rect, 0, 0, 150, 20);
    lh_gl_label_init(&tree.row_one, rect, "row one");
    lh_gl_entity_set_place_v(lh_gl_label_as_entity(&tree.row_one), &s.fixed20);
    lh_gl_entity_add_child(lh_gl_container_as_entity(&tree.box), lh_gl_label_as_entity(&tree.row_one));

    lh_gl_rect_init(&rect, 0, 0, 150, 20);
    lh_gl_label_init(&tree.row_two, rect, "row two");
    lh_gl_entity_set_place_v(lh_gl_label_as_entity(&tree.row_two), &s.fixed20);
    lh_gl_entity_add_child(lh_gl_container_as_entity(&tree.box), lh_gl_label_as_entity(&tree.row_two));

    lh_gl_rect_init(&rect, 10, 80, 140, 30);
    lh_gl_button_init(&tree.action, rect);
    lh_gl_button_set_style(&tree.action, &s.action_style);
    lh_gl_button_set_hot_style_v(&tree.action, &s.action_hot_style);
    lh_gl_button_set_on_click(&tree.action, count_click, lh_null);
    lh_gl_container_set_layout(lh_gl_button_as_container(&tree.action), &s.button_flow);
    lh_gl_entity_add_child(&tree.background, lh_gl_button_as_entity(&tree.action));

    lh_gl_rect_init(&rect, 0, 0, 4, 2);
    lh_gl_image_init(&tree.icon, rect, &s.mask);
    lh_gl_image_set_tint_v(&tree.icon, &s.ink);
    lh_gl_entity_set_place_v(lh_gl_image_as_entity(&tree.icon), &s.wrap_center);
    lh_gl_entity_add_child(lh_gl_container_as_entity(lh_gl_button_as_container(&tree.action)),
                           lh_gl_image_as_entity(&tree.icon));

    lh_gl_rect_init(&rect, 0, 0, 60, 14);
    lh_gl_label_init(&tree.caption, rect, "Hide panel");
    lh_gl_entity_set_style_v(lh_gl_label_as_entity(&tree.caption), &s.text_style);
    lh_gl_entity_set_place_v(lh_gl_label_as_entity(&tree.caption), &s.wrap_center);
    lh_gl_entity_add_child(lh_gl_container_as_entity(lh_gl_button_as_container(&tree.action)),
                           lh_gl_label_as_entity(&tree.caption));

    lh_gl_rect_init(&rect, 180, 10, 10, 60);
    lh_gl_scrollbar_init(&tree.bar, rect, lh_gl_axis_vertical, &tree.box);
    lh_gl_scrollbar_set_mode(&tree.bar, lh_gl_scrollbar_mode_always);
    lh_gl_entity_add_child(&tree.background, lh_gl_scrollbar_as_entity(&tree.bar));

    lh_gl_rect_init(&rect, 160, 110, 24, 24);
    lh_gl_toggle_init(&tree.sw, rect);
    lh_gl_toggle_set_off_styles(&tree.sw, &s.card_style, &s.action_style);
    lh_gl_toggle_set_on_styles(&tree.sw, &s.action_style, &s.action_hot_style);
    lh_gl_entity_add_child(&tree.background, lh_gl_toggle_as_entity(&tree.sw));

    nodes[0] = &tree.background;
    nodes[1] = lh_gl_container_as_entity(&tree.box);
    nodes[2] = lh_gl_label_as_entity(&tree.row_one);
    nodes[3] = lh_gl_label_as_entity(&tree.row_two);
    nodes[4] = lh_gl_button_as_entity(&tree.action);
    nodes[5] = lh_gl_image_as_entity(&tree.icon);
    nodes[6] = lh_gl_label_as_entity(&tree.caption);
    nodes[7] = lh_gl_scrollbar_as_entity(&tree.bar);
    nodes[8] = lh_gl_toggle_as_entity(&tree.sw);
    lh_ptr_deref(root) = &tree.background;
    lh_gl_canvas_init(&tree.canvas, lh_addr_of(lh_gl_canvas_backend_null), lh_null);
}

/* The declaration. It says what the tree **is**: who is a child of whom, where it
   sits, which flow places it and what it carries. Every material — a style, a text,
   a mask, a flow, a callback — is the app's own object, named here by address. */
const lh_gl_decl_t g_rows[k_rows] = {
    {lh_gl_decl_kind_entity, 0, 0, 200, 140, LH_GL_DECL_ROOT, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_container, 10, 10, 160, 60, 0, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_label, 0, 0, 150, 20, 1, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_label, 0, 0, 150, 20, 1, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_button, 10, 80, 140, 30, 0, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_image, 0, 0, 4, 2, 4, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_label, 0, 0, 60, 14, 4, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_scrollbar, 180, 10, 10, 60, 0, nullptr, nullptr, nullptr, {}, lh_bool_false},
    {lh_gl_decl_kind_toggle, 160, 110, 24, 24, 0, nullptr, nullptr, nullptr, {}, lh_bool_false},
};

/* The materials of the tree, filled into a copy of the table: styles, flows, places,
   a mask and a callback — every one of them the app's own object. What each kind
   carries is set here too and not in the table: a designated initializer for a
   union member is C++20, and MSVC does not take it in the standard tests build with. */
void
fill_rows(scene &s, lh_gl_decl_t *rows)
{
    for (int i = 0; i < k_rows; ++i)
    {
        rows[i] = g_rows[i];
    }
    lh_gl_decl_set_label_text(&rows[2], "row one");
    lh_gl_decl_set_label_text(&rows[3], "row two");
    lh_gl_decl_set_button_on_click(&rows[4], count_click);
    lh_gl_decl_set_label_text(&rows[6], "Hide panel");
    lh_gl_decl_set_scrollbar_box(&rows[7], 1);
    lh_gl_decl_set_scrollbar_axis(&rows[7], lh_gl_axis_vertical);
    lh_gl_decl_set_scrollbar_mode(&rows[7], lh_gl_scrollbar_mode_always);
    lh_gl_decl_set_style(&rows[0], &s.root_style);
    lh_gl_decl_set_style(&rows[1], &s.card_style);
    lh_gl_decl_set_layout(&rows[1], &s.column);
    lh_gl_decl_set_place(&rows[2], &s.fixed20);
    lh_gl_decl_set_place(&rows[3], &s.fixed20);
    lh_gl_decl_set_style(&rows[4], &s.action_style);
    lh_gl_decl_set_layout(&rows[4], &s.button_flow);
    lh_gl_decl_set_button_hot_style(&rows[4], &s.action_hot_style);
    lh_gl_decl_set_place(&rows[5], &s.wrap_center);
    lh_gl_decl_set_image_mask(&rows[5], &s.mask);
    lh_gl_decl_set_image_tint(&rows[5], &s.ink);
    lh_gl_decl_set_style(&rows[6], &s.text_style);
    lh_gl_decl_set_place(&rows[6], &s.wrap_center);
    lh_gl_decl_set_toggle_off_style(&rows[8], &s.card_style);
    lh_gl_decl_set_toggle_off_hot_style(&rows[8], &s.action_style);
    lh_gl_decl_set_toggle_on_style(&rows[8], &s.action_style);
    lh_gl_decl_set_toggle_on_hot_style(&rows[8], &s.action_hot_style);
}

lh_byte_t g_storage[k_rows * (int)LH_GL_DECL_NODE_MAX];

bool
build_by_decl(scene &s, lh_gl_entity_t **nodes, lh_gl_entity_t **root, lh_gl_canvas_t *canvas,
              lh_gl_decl_fault_t *fault)
{
    lh_gl_decl_t rows[k_rows];

    fill_rows(s, rows);
    lh_bool_t ok =
        lh_gl_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), root, fault);
    lh_gl_canvas_init(canvas, lh_addr_of(lh_gl_canvas_backend_null), lh_null);
    return ok == lh_bool_true;
}

} // namespace

/* The contract this whole file exists for: a tree built from a table is the tree
   the same app would have written by hand. Same rects, same classes, same pixels,
   same click. */
TEST(ui_decl, a_tree_from_a_table_is_the_tree_written_by_hand)
{
    lh_gl_entity_t *hand_nodes[k_rows] = {};
    lh_gl_entity_t *decl_nodes[k_rows] = {};
    lh_gl_entity_t *hand_root = nullptr;
    lh_gl_entity_t *decl_root = nullptr;

    scene hand_materials;
    hand_tree tree;
    build_by_hand(hand_materials, tree, hand_nodes, &hand_root);
    lh_gl_canvas_t decl_canvas;
    lh_gl_decl_fault_t fault;
    scene decl_materials;
    ASSERT_TRUE(build_by_decl(decl_materials, decl_nodes, &decl_root, &decl_canvas, &fault) !=
                lh_bool_false)
        << "the declaration was refused: code " << (int)lh_gl_decl_fault_get_code(&fault) << " row "
        << (int)lh_gl_decl_fault_get_index(&fault);
    built hand;
    snapshot(&hand, hand_nodes, hand_root, &tree.canvas);
    built from_decl;
    snapshot(&from_decl, decl_nodes, decl_root, &decl_canvas);

    for (int i = 0; i < k_rows; ++i)
    {
        EXPECT_EQ(hand.classes[i], from_decl.classes[i]) << "row " << i << " is a different widget";
        if (!rect_is(hand.rects[i], from_decl.rects[i]))
        {
            const lh_gl_rect_t *a = &hand.rects[i];
            const lh_gl_rect_t *b = &from_decl.rects[i];
            fprintf(stderr, "        row %d: hand (%d,%d %dx%d), declared (%d,%d %dx%d)\n", i,
                    (int)lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(lh_ptr_deref(a)))),
                    (int)lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(lh_ptr_deref(a)))),
                    (int)lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(lh_ptr_deref(a)))),
                    (int)lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(lh_ptr_deref(a)))),
                    (int)lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(lh_ptr_deref(b)))),
                    (int)lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(lh_ptr_deref(b)))),
                    (int)lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(lh_ptr_deref(b)))),
                    (int)lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(lh_ptr_deref(b)))));
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
    lh_gl_entity_t *nodes[k_rows] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[k_rows];

    scene materials;
    fill_rows(materials, rows);
    lh_gl_decl_set_parent(&rows[2], 4); /* row 2 claims a parent that is declared after it */

    lh_gl_decl_fault_t fault;
    EXPECT_FALSE(lh_gl_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_parent);
    EXPECT_EQ(lh_gl_decl_fault_get_index(&fault), 2u)
        << "the refusal did not name the row that is wrong";
}

TEST(ui_decl, a_table_with_no_root_is_not_a_tree)
{
    lh_gl_entity_t *nodes[2] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[2] = {};

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_entity);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_label);
    lh_gl_decl_set_parent(&rows[1],
                          LH_GL_DECL_ROOT); /* two roots: not a mistake a build can guess at */

    lh_gl_decl_fault_t fault;
    EXPECT_FALSE(lh_gl_decl_build(rows, 2, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_roots);
}

TEST(ui_decl, a_pool_too_small_for_the_rows_is_refused_not_overrun)
{
    lh_gl_entity_t *nodes[k_rows] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[k_rows];

    scene materials;
    fill_rows(materials, rows);
    /* A pool for a few rows: the build measures what the rows need before it writes
       any of them, so this is a refusal and not a row outside the buffer. */
    lh_gl_decl_fault_t fault;
    lh_byte_t small[k_rows * 16];
    EXPECT_FALSE(lh_gl_decl_build(rows, k_rows, nodes, small, (lh_u32_t)sizeof(small), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_storage)
        << "the pool was written past instead of refused";
}

TEST(ui_decl, a_scrollbar_naming_a_row_that_is_not_a_container_is_refused)
{
    lh_gl_entity_t *nodes[k_rows] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[k_rows];

    scene materials;
    fill_rows(materials, rows);
    lh_gl_decl_set_scrollbar_box(&rows[7], 5); /* row 5 is a picture, not a container */

    lh_gl_decl_fault_t fault;
    EXPECT_FALSE(lh_gl_decl_build(rows, k_rows, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_box);
    EXPECT_EQ(lh_gl_decl_fault_get_index(&fault), 7u);
}

TEST(ui_decl, an_unknown_kind_is_refused_with_its_row)
{
    lh_gl_entity_t *nodes[1] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[1] = {};

    lh_gl_decl_set_kind(&rows[0], (lh_gl_decl_kind_t)42);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);

    lh_gl_decl_fault_t fault;
    EXPECT_FALSE(lh_gl_decl_build(rows, 1, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root,
                                  &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_kind);
    EXPECT_EQ(lh_gl_decl_fault_get_index(&fault), 0u);
}

TEST(ui_decl, a_hidden_row_is_built_hidden_and_the_rest_are_not)
{
    lh_gl_entity_t *nodes[2] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[2] = {};

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_container);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_w(&rows[0], 100);
    lh_gl_decl_set_h(&rows[0], 100);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_entity);
    lh_gl_decl_set_parent(&rows[1], 0);
    lh_gl_decl_set_hidden(&rows[1], lh_bool_true);

    lh_gl_decl_fault_t fault;
    ASSERT_TRUE(lh_gl_decl_build(rows, 2, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root, &fault) ==
                lh_bool_true);
    EXPECT_EQ(lh_gl_entity_is_hidden(nodes[0]), lh_bool_false);
    EXPECT_EQ(lh_gl_entity_is_hidden(nodes[1]), lh_bool_true);
}

TEST(ui_decl, an_input_row_edits_the_buffer_it_names_and_starts_with_its_text)
{
    lh_gl_entity_t *nodes[2] = {};
    lh_gl_entity_t *root = nullptr;
    lh_gl_decl_t rows[2] = {};
    static lh_char_t buffer[9];

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_container);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_w(&rows[0], 100);
    lh_gl_decl_set_h(&rows[0], 100);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_input);
    lh_gl_decl_set_parent(&rows[1], 0);
    lh_gl_decl_set_w(&rows[1], 80);
    lh_gl_decl_set_h(&rows[1], 20);
    lh_gl_decl_set_input_buffer(&rows[1], buffer);
    lh_gl_decl_set_input_capacity(&rows[1], 8);
    lh_gl_decl_set_input_text(&rows[1], "hello");

    lh_gl_decl_fault_t fault;
    ASSERT_TRUE(lh_gl_decl_build(rows, 2, nodes, g_storage, (lh_u32_t)sizeof(g_storage), &root, &fault) ==
                lh_bool_true);
    EXPECT_STREQ(buffer, "hello");
    EXPECT_STREQ(lh_gl_input_get_text(lh_ptr_rcast(lh_gl_input_t, nodes[1])), "hello");
}

/* -- The phases of a build ------------------------------------------------- */

/* The measure is every row's widget laid end to end, and nothing is placed. */
TEST(ui_decl, the_measure_is_the_widgets_end_to_end_and_places_nothing)
{
    lh_gl_entity_t *nodes[2] = {};
    lh_gl_decl_t rows[2] = {};
    lh_u32_t bytes = 0U;

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_container);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_label);
    lh_gl_decl_set_parent(&rows[1], 0);
    ASSERT_TRUE(lh_gl_decl_try_measure(rows, 2, &bytes, nullptr) == lh_bool_true);
    EXPECT_EQ(bytes, lh_gl_decl_size_of(lh_gl_decl_kind_container) +
                         lh_gl_decl_size_of(lh_gl_decl_kind_label));
    EXPECT_EQ(nodes[0], nullptr) << "measuring placed a widget";
}

TEST(ui_decl, the_measure_refuses_two_roots_and_names_no_row)
{
    lh_gl_decl_t rows[2] = {};
    lh_u32_t bytes = 7U;

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_entity);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_entity);
    lh_gl_decl_set_parent(&rows[1], LH_GL_DECL_ROOT);
    lh_gl_decl_fault_t fault;
    EXPECT_FALSE(lh_gl_decl_try_measure(rows, 2, &bytes, &fault) == lh_bool_true);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_roots);
    EXPECT_EQ(bytes, 7U) << "a refused table leaves the count alone";
}

/* Placing builds every widget and wires nothing; the link is what makes it a tree. */
TEST(ui_decl, place_builds_every_row_and_link_wires_them_into_one_tree)
{
    lh_gl_entity_t *nodes[2] = {};
    lh_gl_decl_t rows[2] = {};

    lh_gl_decl_set_kind(&rows[0], lh_gl_decl_kind_container);
    lh_gl_decl_set_parent(&rows[0], LH_GL_DECL_ROOT);
    lh_gl_decl_set_w(&rows[0], 100);
    lh_gl_decl_set_h(&rows[0], 100);
    lh_gl_decl_set_kind(&rows[1], lh_gl_decl_kind_entity);
    lh_gl_decl_set_parent(&rows[1], 0);
    ASSERT_TRUE(lh_gl_decl_try_place(rows, 2, nodes, g_storage, nullptr) == lh_bool_true);
    ASSERT_NE(nodes[0], nullptr);
    ASSERT_NE(nodes[1], nullptr);
    EXPECT_EQ(lh_gl_entity_get_parent(nodes[1]), nullptr) << "placing wired the tree";
    EXPECT_EQ(lh_gl_decl_link(rows, 2, nodes), nodes[0]);
    EXPECT_EQ(lh_gl_entity_get_parent(nodes[1]), nodes[0]);
}

TEST(ui_decl, a_row_applies_its_hidden_and_its_style_but_a_button_aims_its_own)
{
    lh_gl_decl_t row = {};

    lh_gl_entity_t entity;
    lh_gl_entity_init(&entity, lh_test::rect_of(0, 0, 10, 10));
    lh_gl_style_t style;
    lh_gl_style_init(&style);
    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_entity);
    lh_gl_decl_set_style(&row, &style);
    lh_gl_decl_set_hidden(&row, lh_bool_true);
    lh_gl_decl_apply_row(&entity, &row);
    EXPECT_EQ(lh_gl_entity_get_style(&entity), &style);
    EXPECT_EQ(lh_gl_entity_is_hidden(&entity), lh_bool_true);

    lh_gl_entity_t other;
    lh_gl_entity_init(&other, lh_test::rect_of(0, 0, 10, 10));
    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_button);
    lh_gl_decl_set_hidden(&row, lh_bool_false);
    lh_gl_decl_apply_row(&other, &row);
    EXPECT_NE(lh_gl_entity_get_style(&other), &style) << "a button aims its entity itself";
}

TEST(ui_decl, every_field_reads_back_what_its_setter_stored)
{
    lh_gl_decl_t row = {};

    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_input);
    lh_gl_decl_set_parent(&row, 3);
    lh_gl_style_t style;
    lh_gl_decl_set_style(&row, &style);
    lh_gl_decl_set_hidden(&row, lh_bool_true);
    char buffer[8];
    lh_gl_decl_set_input_buffer(&row, buffer);
    lh_gl_decl_set_input_capacity(&row, 7U);
    EXPECT_EQ(lh_gl_decl_get_kind(&row), lh_gl_decl_kind_input);
    EXPECT_EQ(lh_gl_decl_get_parent(&row), 3);
    EXPECT_EQ(lh_gl_decl_get_style(&row), &style);
    EXPECT_EQ(lh_gl_decl_is_hidden(&row), lh_bool_true);
    EXPECT_EQ(lh_gl_decl_get_input_buffer(&row), buffer);
    EXPECT_EQ(lh_gl_decl_get_input_capacity(&row), 7U);

    lh_gl_decl_fault_t fault = {};
    lh_gl_decl_fault_set_code(&fault, lh_gl_decl_error_kind);
    lh_gl_decl_fault_set_index(&fault, 5U);
    EXPECT_EQ(lh_gl_decl_fault_get_code(&fault), lh_gl_decl_error_kind);
    EXPECT_EQ(lh_gl_decl_fault_get_index(&fault), 5U);
}

TEST(ui_decl, the_rect_of_a_row_is_its_four_fields)
{
    lh_gl_decl_t row = {};

    lh_gl_decl_set_x(&row, lh_gl_scalar(1));
    lh_gl_decl_set_y(&row, lh_gl_scalar(2));
    lh_gl_decl_set_w(&row, lh_gl_scalar(30));
    lh_gl_decl_set_h(&row, lh_gl_scalar(40));
    lh_gl_rect_t rect;
    lh_gl_decl_get_rect(&row, &rect);
    EXPECT_TRUE(rect_is(rect, rect_of(1, 2, 30, 40)));
}

TEST(ui_decl, each_kind_places_its_own_block_and_leaves_the_rest)
{
    lh_gl_decl_t row = {};
    lh_gl_decl_set_w(&row, lh_gl_scalar(10));
    lh_gl_decl_set_h(&row, lh_gl_scalar(10));

    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_label);
    lh_gl_decl_set_label_text(&row, "hi");
    lh_gl_label_t label;
    EXPECT_EQ(lh_gl_decl_place_label(&row, (lh_byte_ptr)&label), lh_gl_label_as_entity(&label));
    EXPECT_STREQ(lh_gl_label_get_text(&label), "hi");

    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_toggle);
    lh_gl_decl_set_toggle_checked(&row, lh_bool_true);
    lh_gl_toggle_t toggle;
    EXPECT_EQ(lh_gl_decl_place_toggle(&row, (lh_byte_ptr)&toggle),
              lh_gl_toggle_as_entity(&toggle));
    EXPECT_EQ(lh_gl_toggle_is_checked(&toggle), lh_bool_true);

    lh_gl_color_t tint;
    lh_gl_color_init(&tint, 1, 2, 3, 255);
    lh_gl_decl_set_kind(&row, lh_gl_decl_kind_image);
    lh_gl_decl_set_image_tint(&row, &tint);
    lh_gl_image_t image;
    EXPECT_EQ(lh_gl_decl_place_image(&row, (lh_byte_ptr)&image), lh_gl_image_as_entity(&image));
    EXPECT_EQ(lh_gl_color_get_g(lh_gl_image_get_tint(&image)), 2);

    lh_gl_decl_set_kind(&row, lh_cast_static(lh_gl_decl_kind_t, 99));
    EXPECT_EQ(lh_gl_decl_place_row(&row, (lh_byte_ptr)&image, lh_null), lh_null);
}
