#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/tiny_font.h>

#include <lh/bool.h>
#include <lh/ui/entity.h>
#include <lh/ui/label.h>
#include <lh/ui/layout.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;
using lh_test::tiny_font;

/* Placement is the whole of what a flow decides, so a failed comparison has to
   say what it got: an assertion that only says "false" is a question, not a
   measurement. */
bool
rect_at(const lh_ui_entity_t *entity, int x, int y, int w, int h)
{
    const lh_ui_rect_t got = lh_ui_entity_get_rect(entity);

    if (!rect_is(got, rect_of(x, y, w, h)))
    {
        fprintf(stderr, "        got (%d,%d %dx%d), wanted (%d,%d %dx%d)\n",
                (int)lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(got))),
                (int)lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(got))),
                (int)lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(got))),
                (int)lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(got))), x, y, w, h);
        return false;
    }
    return true;
}

void
place_child(lh_ui_entity_t *child, lh_ui_place_size_t mode, lh_ui_scalar_t size,
            lh_ui_place_align_t align)
{
    lh_ui_place_t place;

    lh_ui_place_init(&place, mode, size);
    lh_ui_place_set_align(&place, align);
    lh_ui_entity_set_place(child, &place);
}

/* parent (100,50) 240x160, padding 8, three children of different sizes. The
   content box is (108,58) 224x144, and every number below is read off it. */
struct layout_fixture
{
    lh_ui_entity_t parent;
    lh_ui_entity_t kids[3];
    lh_ui_style_t style;

    layout_fixture()
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

/* A column of fixed heights: the pass asks each child for its own length along
   the flow and leaves it across, exactly where it was. */
TEST(ui_layout, a_column_places_children_by_their_own_size)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(20), lh_ui_place_align_start);
    place_child(&f.kids[1], lh_ui_place_size_fixed, lh_ui_scalar(25), lh_ui_place_align_start);
    place_child(&f.kids[2], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_start);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_apply(&layout, &f.parent);

    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&f.kids[1], 108, 82, 40, 25));
    EXPECT_TRUE(rect_at(&f.kids[2], 108, 111, 50, 30));
}

/* A row across the same content box: the widths are the children's own. */
TEST(ui_layout, a_row_places_children_by_their_own_width)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_start);
    place_child(&f.kids[1], lh_ui_place_size_fixed, lh_ui_scalar(40), lh_ui_place_align_start);
    lh_ui_layout_init(&layout, lh_ui_axis_horizontal, lh_ui_scalar(2));
    lh_ui_layout_apply(&layout, &f.parent);

    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&f.kids[1], 140, 58, 40, 25))
        << "the second child did not follow the first along the row";
}

/* `fill` across the flow is what the old stack called stretch, and it says it
   per child instead of for the whole row. */
TEST(ui_layout, align_fill_takes_the_whole_cross_side)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(20), lh_ui_place_align_fill);
    place_child(&f.kids[1], lh_ui_place_size_fixed, lh_ui_scalar(25), lh_ui_place_align_fill);
    place_child(&f.kids[2], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_fill);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_apply(&layout, &f.parent);

    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 224, 20));
    EXPECT_TRUE(rect_at(&f.kids[1], 108, 82, 224, 25));
    EXPECT_TRUE(rect_at(&f.kids[2], 108, 111, 224, 30));
}

/* What is left over after everyone has said what they want goes to the children
   that asked to fill, and nobody after them moves. */
TEST(ui_layout, a_fill_child_takes_what_is_left_over)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(20), lh_ui_place_align_start);
    place_child(&f.kids[1], lh_ui_place_size_fill, lh_ui_scalar(0), lh_ui_place_align_start);
    place_child(&f.kids[2], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_start);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_apply(&layout, &f.parent);

    /* 144 of content, 20 + 30 + two gaps of 4 spent: 86 is what is left. */
    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&f.kids[1], 108, 82, 40, 86));
    EXPECT_TRUE(rect_at(&f.kids[2], 108, 172, 50, 30))
        << "the child after the filler did not move out of its way";
}

TEST(ui_layout, hidden_children_take_no_room)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(20), lh_ui_place_align_start);
    place_child(&f.kids[1], lh_ui_place_size_fixed, lh_ui_scalar(25), lh_ui_place_align_start);
    place_child(&f.kids[2], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_start);
    lh_ui_entity_set_hidden(&f.kids[1], lh_bool_true);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_layout_apply(&layout, &f.parent);

    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&f.kids[2], 108, 82, 50, 30))
        << "the gap of a hidden child is still being counted";
}

/* What is left over with nobody to fill it goes where the layout says. */
TEST(ui_layout, justify_puts_what_is_left_over)
{
    layout_fixture f;
    lh_ui_layout_t layout;

    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(30), lh_ui_place_align_start);
    lh_ui_layout_init(&layout, lh_ui_axis_horizontal, lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_layout_get_justify(&layout), lh_ui_justify_start);
    lh_ui_layout_set_justify(&layout, lh_ui_justify_center);
    lh_ui_layout_apply(&layout, &f.parent);
    EXPECT_TRUE(rect_at(&f.kids[0], 205, 58, 30, 20))
        << "a 30 wide child in a 224 wide box belongs at x 108 + (224 - 30) / 2";

    lh_ui_layout_set_justify(&layout, lh_ui_justify_end);
    lh_ui_layout_apply(&layout, &f.parent);
    EXPECT_TRUE(rect_at(&f.kids[0], 302, 58, 30, 20));
}

/* A button is a background, a picture and a caption; what the flow has to do is
   get out of the way when the caption is not there. */
struct button_fixture
{
    lh_ui_entity_t parent;
    lh_ui_entity_t picture; /* an image entity does not exist yet; a plain one
                             * with a size is the same thing to the flow */
    lh_ui_label_t caption;
    lh_ui_style_t style;
    lh_ui_style_t text_style;
    lh_ui_layout_t layout;

    button_fixture()
    {
        lh_ui_style_init(&style);
        lh_ui_style_set_padding(&style, lh_ui_scalar(8));
        lh_ui_style_init(&text_style);
        lh_ui_style_set_font(&text_style, tiny_font());
        lh_ui_entity_init(&parent, rect_of(100, 50, 240, 160));
        lh_ui_entity_set_style(&parent, &style);
        lh_ui_entity_init(&picture, rect_of(0, 0, 16, 16));
        lh_ui_label_init(&caption, rect_of(0, 0, 0, 4), "");
        lh_ui_entity_set_style(lh_ui_label_as_entity(&caption), &text_style);
        lh_ui_entity_add_child(&parent, &picture);
        lh_ui_entity_add_child(&parent, lh_ui_label_as_entity(&caption));
        place_child(&picture, lh_ui_place_size_fixed, lh_ui_scalar(16), lh_ui_place_align_center);
        place_child(lh_ui_label_as_entity(&caption), lh_ui_place_size_wrap, lh_ui_scalar(0),
                    lh_ui_place_align_center);
        lh_ui_layout_init(&layout, lh_ui_axis_horizontal, lh_ui_scalar(8));
        lh_ui_layout_set_justify(&layout, lh_ui_justify_center);
    }
};

TEST(ui_layout, a_child_that_wraps_to_nothing_collapses_and_leaves_no_gap)
{
    button_fixture f;
    lh_ui_rect_t caption_rect;

    lh_ui_layout_apply(&f.layout, &f.parent);
    caption_rect = lh_ui_entity_get_rect(lh_ui_label_as_entity(&f.caption));

    /* 16 of picture in a 224 wide box, centred on its own: x 108 + (224 - 16) / 2,
       and 16 tall in a 144 tall box: y 58 + (144 - 16) / 2. */
    EXPECT_TRUE(rect_at(&f.picture, 212, 122, 16, 16));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(caption_rect))), 0)
        << "the empty caption took room";
}

TEST(ui_layout, the_same_flow_centres_the_row_once_the_caption_has_text)
{
    button_fixture f;
    lh_ui_rect_t picture_rect;
    lh_ui_rect_t caption_rect;
    lh_ui_point_t picture_at;

    lh_ui_label_set_text(&f.caption, "AB");
    lh_ui_layout_apply(&f.layout, &f.parent);

    /* 16 + 8 + the width of "AB" is what the row takes, centred as a whole, so
       the picture is no longer in the middle of the button. */
    picture_rect = lh_ui_entity_get_rect(&f.picture);
    caption_rect = lh_ui_entity_get_rect(lh_ui_label_as_entity(&f.caption));
    picture_at = *lh_ui_rect_get_origin_as_const(lh_addr_of(picture_rect));

    EXPECT_LT(lh_ui_point_get_x(&picture_at), 212) << "the picture did not move aside";
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(caption_rect))), 7)
        << "the caption did not wrap to its text";
}

/* A moved child takes its subtree along: the pass writes absolute rects, so
   whatever sits under one has to move with it. */
TEST(ui_layout, a_moved_child_takes_its_subtree_along)
{
    layout_fixture f;
    lh_ui_layout_t layout;
    lh_ui_entity_t grandchild;

    lh_ui_entity_init(&grandchild, rect_of(5, 6, 3, 3));
    lh_ui_entity_add_child(&f.kids[0], &grandchild);
    place_child(&f.kids[0], lh_ui_place_size_fixed, lh_ui_scalar(20), lh_ui_place_align_start);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(0));
    lh_ui_layout_apply(&layout, &f.parent);

    EXPECT_TRUE(rect_at(&f.kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&grandchild, 113, 64, 3, 3));
}

/* The contract this exists for: placement is the container's own job, so nothing
   outside has to run a pass. Asking for the children transform is what a frame
   does before it draws them, and the children are placed there and then. */
TEST(ui_layout, a_container_places_its_own_children_and_takes_them_along_when_it_moves)
{
    lh_ui_container_t box;
    lh_ui_entity_t kids[2];
    lh_ui_style_t style;
    lh_ui_layout_t layout;
    lh_ui_point_t offset;

    lh_ui_style_init(&style);
    lh_ui_style_set_padding(&style, lh_ui_scalar(8));
    lh_ui_container_init(&box, rect_of(100, 50, 240, 160));
    lh_ui_entity_set_style(lh_ui_container_as_entity(&box), &style);
    lh_ui_layout_init(&layout, lh_ui_axis_vertical, lh_ui_scalar(4));
    lh_ui_container_set_layout(&box, &layout);
    for (int i = 0; i < 2; ++i)
    {
        lh_ui_entity_init(&kids[i], rect_of(0, 0, 30, 20 + i * 10));
        place_child(&kids[i], lh_ui_place_size_fixed, lh_ui_scalar(20 + i * 10),
                    lh_ui_place_align_start);
        lh_ui_entity_add_child(lh_ui_container_as_entity(&box), &kids[i]);
    }

    lh_ui_entity_get_children_transform(lh_ui_container_as_entity(&box), &offset);
    EXPECT_TRUE(rect_at(&kids[0], 108, 58, 30, 20));
    EXPECT_TRUE(rect_at(&kids[1], 108, 82, 30, 30));

    /* The container moves. Nobody runs anything; the next frame is the pass. */
    lh_ui_entity_set_rect(lh_ui_container_as_entity(&box), rect_of(200, 150, 240, 160));
    EXPECT_TRUE(rect_at(&kids[0], 108, 58, 30, 20)) << "something placed the children too early";
    lh_ui_entity_get_children_transform(lh_ui_container_as_entity(&box), &offset);
    EXPECT_TRUE(rect_at(&kids[0], 208, 158, 30, 20))
        << "the children stayed where the box used to be";
    EXPECT_TRUE(rect_at(&kids[1], 208, 182, 30, 30));
}

TEST(ui_place, an_entity_carries_what_it_wants_and_gets_it_back)
{
    lh_ui_entity_t entity;
    lh_ui_place_t place;

    lh_ui_place_init(&place, lh_ui_place_size_fixed, lh_ui_scalar(12));
    EXPECT_EQ(lh_ui_place_get_size_mode(&place), lh_ui_place_size_fixed);
    EXPECT_EQ(lh_ui_place_get_size(&place), lh_ui_scalar(12));
    EXPECT_EQ(lh_ui_place_get_align(&place), lh_ui_place_align_start);

    lh_ui_place_set_align(&place, lh_ui_place_align_fill);
    lh_ui_place_set_size(&place, lh_ui_place_size_wrap, lh_ui_scalar(0));
    lh_ui_entity_init(&entity, rect_of(0, 0, 10, 10));
    EXPECT_EQ(lh_ui_place_get_align(lh_ui_entity_get_place(&entity)), lh_ui_place_align_start)
        << "a new entity did not start at the beginning";
    lh_ui_entity_set_place(&entity, &place);
    EXPECT_EQ(lh_ui_place_get_align(lh_ui_entity_get_place(&entity)), lh_ui_place_align_fill);
    EXPECT_EQ(lh_ui_place_get_size_mode(lh_ui_entity_get_place(&entity)), lh_ui_place_size_wrap);
}

TEST(ui_axis, cross_swaps_the_axes)
{
    EXPECT_EQ(lh_ui_axis_get_cross(lh_ui_axis_horizontal), lh_ui_axis_vertical);
    EXPECT_EQ(lh_ui_axis_get_cross(lh_ui_axis_vertical), lh_ui_axis_horizontal);
}