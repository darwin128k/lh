#include <gtest/gtest.h>

#include <lh/test/ui/fill_probe.h>

#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity/label.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/ui/text.h>
#include <lh/ui/text/align.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(entity_label, init_keeps_the_rect_and_the_text_pointer)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    const lh_char_t *text = "Hi";
    lh_ui_entity_label_t label;
    lh_ui_entity_label_init(lh_addr_of(label), rect, text);
    lh_ui_entity_t *entity = lh_ui_entity_label_as_entity(lh_addr_of(label));
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(entity);

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
    EXPECT_EQ(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_entity_label_class));
    EXPECT_EQ(lh_ui_entity_label_as_container(lh_addr_of(label)), lh_addr_of(label.container));
    EXPECT_EQ(lh_ui_entity_container_as_entity(lh_ui_entity_label_as_container(lh_addr_of(label))),
              entity);
}

TEST(entity_label, set_text_replaces_the_pointer)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_label_init(lh_addr_of(label), rect, "one");
    const lh_char_t *text = "two";

    lh_ui_entity_label_set_text(lh_addr_of(label), text);

    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
}

TEST(entity_label, draw_goes_through_the_embedded_entity)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_canvas_t canvas;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_label_init(lh_addr_of(label), rect, "x");
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);

    lh_ui_entity_draw(lh_ui_entity_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));
}

/* Contract: a derived class keeps the base fill by calling
 * lh_ui_entity_class_event_base. The label class does. */
TEST(entity_label, draw_keeps_the_base_class_fill)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_canvas_t canvas;
    lh_test::fill_probe probe;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_label_init(lh_addr_of(label), rect, "x");
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(lh_addr_of(label)), lh_addr_of(style));
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_backend(), rect,
                             color);

    lh_ui_entity_draw(lh_ui_entity_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));

    EXPECT_EQ(probe.matches, 1);
}

TEST(entity_label, padding_moves_the_text_in_from_the_corner)
{
    lh_ui_entity_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_entity_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_style_set_padding(&style, lh_ui_scalar(6));
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_entity_label_get_text_origin(&label);
    EXPECT_EQ(lh_ui_point_get_x(&origin), lh_ui_scalar(16));
    EXPECT_EQ(lh_ui_point_get_y(&origin), lh_ui_scalar(26));
}

TEST(entity_label, padding_sides_move_the_text_by_left_and_top)
{
    lh_ui_entity_label_t label;
    lh_ui_style_t style;
    lh_ui_insets_t sides;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_entity_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_insets_init(&sides, 3, 5, 30, 50);
    lh_ui_style_set_padding_insets(&style, &sides);
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_entity_label_get_text_origin(&label);
    EXPECT_EQ(lh_ui_point_get_x(&origin), lh_ui_scalar(13));
    EXPECT_EQ(lh_ui_point_get_y(&origin), lh_ui_scalar(25));
}

/* Alignment is the style's to say and the label's to obey. What has to hold is
   that a style that says nothing keeps drawing where it always did, and that the
   text really lands where lh_ui_text_align_get_origin promised — the same point
   the drawn glyphs start at, not merely a plausible one. */

lh_ui_point_t
text_origin_with(lh_ui_text_align_h_t horizontal, lh_ui_text_align_v_t vertical, const lh_char_t *text)
{
    lh_ui_entity_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_entity_label_init(&label, rect, text);
    lh_ui_style_init(&style);
    lh_ui_style_set_align_h(&style, horizontal);
    lh_ui_style_set_align_v(&style, vertical);
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(&label), &style);
    return lh_ui_entity_label_get_text_origin(&label);
}

TEST(entity_label, a_style_that_says_nothing_keeps_the_text_at_the_corner)
{
    lh_ui_entity_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;
    lh_ui_insets_t sides;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_insets_init(&sides, 6, 6, 6, 6);
    lh_ui_entity_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_style_set_padding_insets(&style, &sides);
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_entity_label_get_text_origin(&label);
    EXPECT_EQ(lh_ui_point_get_x(&origin), lh_ui_scalar(16));
    EXPECT_EQ(lh_ui_point_get_y(&origin), lh_ui_scalar(26));
}

TEST(entity_label, align_h_moves_the_text_across_the_padded_box)
{
    const lh_ui_scalar_t width = lh_ui_text_get_width(lh_ui_font_get_default(), "abc");
    const lh_ui_scalar_t room = lh_ui_scalar(100);
    /* The label's box starts at x 10, so the origin carries it and the alignment
       only decides how far past the left inset the text starts. */
    const lh_ui_scalar_t left = lh_ui_scalar(10);

    /* Centring leaves (room - width) / 2 before the text; right leaves all of it. */
    const lh_ui_point_t centred = text_origin_with(lh_ui_text_align_h_center, lh_ui_text_align_v_top, "abc");
    const lh_ui_point_t right = text_origin_with(lh_ui_text_align_h_right, lh_ui_text_align_v_top, "abc");

    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(centred)), left + (room - width) / 2);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(right)), left + room - width);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(centred)), lh_ui_scalar(20));
}

TEST(entity_label, align_v_moves_the_text_down_the_padded_box)
{
    const lh_ui_size_t size = lh_ui_text_get_size(lh_ui_font_get_default(), "abc");
    const lh_ui_scalar_t height = lh_ui_size_get_height(lh_addr_of(size));
    const lh_ui_scalar_t top = lh_ui_scalar(20);

    const lh_ui_point_t centred = text_origin_with(lh_ui_text_align_h_left, lh_ui_text_align_v_center, "abc");
    const lh_ui_point_t bottom = text_origin_with(lh_ui_text_align_h_left, lh_ui_text_align_v_bottom, "abc");

    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(centred)), top + (lh_ui_scalar(50) - height) / 2);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(bottom)), top + lh_ui_scalar(50) - height);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(centred)), lh_ui_scalar(10));
}
