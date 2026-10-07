#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/ui/entity.h>
#include <lh/ui/layout/stack.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{

lh_ui_rect_t
rect_of(int x, int y, int w, int h)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(&rect, x, y, w, h);
    return rect;
}

bool
rect_is(const lh_ui_entity_t &e, int x, int y, int w, int h)
{
    const lh_ui_rect_t want = rect_of(x, y, w, h);
    const lh_ui_rect_t got = lh_ui_entity_get_rect(&e);
    return lh_ui_rect_eq(&got, &want) != 0;
}

/* parent (100,50) 240x160, padding 8, three children of different heights. */
struct stack_fixture
{
    lh_ui_entity_t parent;
    lh_ui_entity_t kids[3];
    lh_ui_style_t style;

    stack_fixture()
    {
        lh_ui_style_init(&style);
        lh_ui_style_set_padding(&style, lh_ui_scalar(8));
        lh_ui_entity_init(&parent, rect_of(100, 50, 240, 160));
        lh_ui_entity_set_style(&parent, &style);
        for (int i = 0; i < 3; ++i)
        {
            lh_ui_entity_init(&kids[i], rect_of(0, 0, 30 + i * 10, 20 + i * 5));
            lh_ui_entity_add_child(&parent, &kids[i]);
        }
    }
};

} // namespace

TEST(ui_layout_stack, column_starts_inside_the_padding_and_keeps_the_gap)
{
    stack_fixture f;
    lh_ui_layout_stack_t stack;

    lh_ui_layout_stack_init(&stack, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_stack_apply(&stack, &f.parent);

    EXPECT_TRUE(rect_is(f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_is(f.kids[1], 108, 82, 40, 25));
    EXPECT_TRUE(rect_is(f.kids[2], 108, 111, 50, 30));
}

TEST(ui_layout_stack, row_with_stretch_takes_the_content_height)
{
    stack_fixture f;
    lh_ui_layout_stack_t stack;

    lh_ui_layout_stack_init(&stack, lh_ui_axis_horizontal, lh_ui_scalar(2));
    lh_ui_layout_stack_set_stretch(&stack, lh_bool_true);
    EXPECT_EQ(lh_ui_layout_stack_is_stretch(&stack), lh_bool_true);
    lh_ui_layout_stack_apply(&stack, &f.parent);

    EXPECT_TRUE(rect_is(f.kids[0], 108, 58, 30, 144));
    EXPECT_TRUE(rect_is(f.kids[1], 140, 58, 40, 144));
    EXPECT_TRUE(rect_is(f.kids[2], 182, 58, 50, 144));
}

TEST(ui_layout_stack, hidden_children_take_no_room)
{
    stack_fixture f;
    lh_ui_layout_stack_t stack;

    lh_ui_entity_set_hidden(&f.kids[1], lh_bool_true);
    lh_ui_layout_stack_init(&stack, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_stack_apply(&stack, &f.parent);

    EXPECT_TRUE(rect_is(f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_is(f.kids[2], 108, 82, 50, 30));
}

TEST(ui_layout_stack, a_moved_child_takes_its_subtree_along)
{
    stack_fixture f;
    lh_ui_layout_stack_t stack;
    lh_ui_entity_t grandchild;

    lh_ui_entity_init(&grandchild, rect_of(5, 6, 3, 3));
    lh_ui_entity_add_child(&f.kids[0], &grandchild);
    lh_ui_layout_stack_init(&stack, lh_ui_axis_vertical, lh_ui_scalar(0));
    lh_ui_layout_stack_apply(&stack, &f.parent);

    EXPECT_TRUE(rect_is(f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_is(grandchild, 113, 64, 3, 3));
}

TEST(ui_axis, cross_swaps_the_axes)
{
    EXPECT_EQ(lh_ui_axis_get_cross(lh_ui_axis_horizontal), lh_ui_axis_vertical);
    EXPECT_EQ(lh_ui_axis_get_cross(lh_ui_axis_vertical), lh_ui_axis_horizontal);
}
