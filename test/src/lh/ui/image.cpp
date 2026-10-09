#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/fill_probe.h>

#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/container.h>
#include <lh/ui/entity.h>
#include <lh/ui/image.h>
#include <lh/ui/label.h>
#include <lh/ui/layout.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{
using lh_test::draw_log;
using lh_test::draw_log_init;
using lh_test::rect_is;
using lh_test::rect_of;

/* 4 x 3 at 8 bpp, three rows of four bytes: a solid block. Every number below
   is read off it. */
const lh_byte_t g_block[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                             0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

lh_ui_mask_t
block_mask()
{
    lh_ui_mask_t mask;

    lh_ui_mask_init(&mask, g_block, 4, 3, 4, 8);
    return mask;
}

lh_ui_style_t
plain_style(lh_ui_scalar_t padding)
{
    lh_ui_style_t style;

    lh_ui_style_init(&style);
    lh_ui_style_set_padding(&style, padding);
    return style;
}
} // namespace

TEST(entity_image, init_keeps_the_mask_the_rect_and_the_class)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    lh_ui_size_t size;

    lh_ui_image_init(&image, rect_of(10, 20, 16, 16), &mask);
    size = lh_ui_image_get_size(&image);

    EXPECT_TRUE(rect_is(lh_ui_entity_get_rect(lh_ui_image_as_entity(&image)), rect_of(10, 20, 16, 16)));
    EXPECT_EQ(lh_ui_image_get_mask(&image), &mask);
    EXPECT_EQ(lh_ui_entity_get_class(lh_ui_image_as_entity(&image)), &lh_ui_image_class);
    EXPECT_EQ(lh_ui_size_get_width(&size), lh_ui_scalar(4)) << "the picture is not the mask";
    EXPECT_EQ(lh_ui_size_get_height(&size), lh_ui_scalar(3));

    /* White and opaque: an image with no style of its own still paints. */
    const lh_ui_color_t *tint = lh_ui_image_get_tint(&image);
    EXPECT_EQ(lh_ui_color_get_r(tint), 255);
    EXPECT_EQ(lh_ui_color_get_a(tint), 255);
}

TEST(entity_image, as_image_tells_an_image_from_anything_else)
{
    lh_ui_image_t image;
    lh_ui_entity_t plain;

    lh_ui_image_init(&image, rect_of(0, 0, 4, 3), lh_null);
    lh_ui_entity_init(&plain, rect_of(0, 0, 4, 3));

    EXPECT_EQ(lh_ui_entity_as_image(lh_ui_image_as_entity(&image)), &image);
    EXPECT_EQ(lh_ui_entity_as_image(&plain), lh_null) << "a plain entity is not an image";
    EXPECT_EQ(lh_ui_entity_as_image(lh_null), lh_null);
}

/* A mask is not scaled: the box is 16 x 16 and the picture is 4 x 3 at its top
   left corner, so the rest of the box stays empty rather than stretching. */
TEST(entity_image, the_picture_is_drawn_as_it_is_at_the_corner_of_the_box)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    lh_ui_canvas_t canvas;
    draw_log log;

    lh_ui_image_init(&image, rect_of(10, 20, 16, 16), &mask);
    draw_log_init(&log, &canvas, false, false, true);

    lh_ui_entity_draw(lh_ui_image_as_entity(&image), &canvas);

    EXPECT_EQ(log.mask_count, 1) << "the picture did not reach the backend";
    EXPECT_TRUE(rect_is(log.masks[0], rect_of(10, 20, 4, 3)));
    EXPECT_EQ(log.fill_count, 0) << "no style was set, so nothing should have been filled";
}

/* The picture keeps the padding that keeps it off the edges, the same origin a
   label's text gets. */
TEST(entity_image, the_picture_starts_inside_the_padding)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    const lh_ui_style_t style = plain_style(lh_ui_scalar(2));
    lh_ui_canvas_t canvas;
    draw_log log;

    lh_ui_image_init(&image, rect_of(10, 20, 16, 16), &mask);
    lh_ui_entity_set_style(lh_ui_image_as_entity(&image), &style);
    draw_log_init(&log, &canvas, false, false, true);

    lh_ui_entity_draw(lh_ui_image_as_entity(&image), &canvas);

    EXPECT_TRUE(rect_is(log.masks[0], rect_of(12, 22, 4, 3)));
}

/* Nothing to show is nothing drawn: a mask is a pointer, and the one that is not
   there must not reach the backend or be asked for its size. */
TEST(entity_image, nothing_is_drawn_without_a_mask)
{
    lh_ui_image_t image;
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    draw_log log;

    lh_ui_image_init(&image, rect_of(10, 20, 16, 16), lh_null);
    draw_log_init(&log, &canvas, false, false, true);

    lh_ui_entity_draw(lh_ui_image_as_entity(&image), &canvas);
    size = lh_ui_image_get_size(&image);

    EXPECT_EQ(log.mask_count, 0);
    EXPECT_EQ(lh_ui_size_get_width(&size), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(&size), lh_ui_scalar(0));
}

TEST(entity_image, the_tint_is_the_colour_the_picture_is_drawn_in)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    lh_ui_color_t red;
    lh_ui_canvas_t canvas;
    draw_log log;

    lh_ui_color_init(&red, 10, 20, 30, 255);
    lh_ui_image_init(&image, rect_of(0, 0, 4, 3), &mask);
    lh_ui_image_set_tint(&image, &red);
    draw_log_init(&log, &canvas, false, false, true);

    lh_ui_entity_draw(lh_ui_image_as_entity(&image), &canvas);

    EXPECT_EQ(log.mask_count, 1);
    EXPECT_EQ(lh_ui_color_get_r(&log.mask_colors[0]), 10);
    EXPECT_EQ(lh_ui_color_get_g(&log.mask_colors[0]), 20);
    EXPECT_EQ(lh_ui_color_get_b(&log.mask_colors[0]), 30);
}

/* Contract of a derived class: the base fill still goes through, so a card with
   a picture on it is one style and one class rather than two entities. */
TEST(entity_image, draw_keeps_the_base_class_fill)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_canvas_t canvas;
    lh_test::fill_probe probe;

    lh_ui_color_init(&color, 1, 2, 3, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_init(&style);
    lh_ui_style_set_fill(&style, &paint);
    lh_ui_image_init(&image, rect_of(0, 0, 4, 3), &mask);
    lh_ui_entity_set_style(lh_ui_image_as_entity(&image), &style);
    lh_test::fill_probe_init(&probe, &canvas, lh_test::fill_probe_backend(), rect_of(0, 0, 4, 3), color);

    lh_ui_entity_draw(lh_ui_image_as_entity(&image), &canvas);

    EXPECT_EQ(probe.matches, 1);
}

/* What `wrap` asks and gets back: the picture, padding included. This is what a
   flow measures, so a row of an image and a caption gives the image its own
   width and nothing more. */
TEST(entity_image, an_image_measures_as_its_mask)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();
    lh_ui_rect_t bounds;

    lh_ui_image_init(&image, rect_of(10, 20, 16, 16), &mask);
    bounds = lh_ui_entity_get_content_bounds(lh_ui_image_as_entity(&image));
    EXPECT_TRUE(rect_is(bounds, rect_of(10, 20, 4, 3)))
        << "the image measured the box it was given instead of the picture it shows";

    const lh_ui_style_t style = plain_style(lh_ui_scalar(2));
    lh_ui_entity_set_style(lh_ui_image_as_entity(&image), &style);
    bounds = lh_ui_entity_get_content_bounds(lh_ui_image_as_entity(&image));
    EXPECT_TRUE(rect_is(bounds, rect_of(12, 22, 4, 3))) << "the padding was not measured with it";
}

/* The point of the whole piece, measured through a flow: a picture that wraps
   takes its own width and the caption follows it. The box said 16 wide, the
   picture is 4 wide, and the flow believes the picture. */
TEST(entity_image, a_wrap_place_gets_the_picture_its_own_width)
{
    lh_ui_container_t box;
    lh_ui_image_t picture;
    lh_ui_label_t caption;
    const lh_ui_mask_t mask = block_mask();
    const lh_ui_style_t style = plain_style(lh_ui_scalar(8));
    lh_ui_layout_t layout;
    lh_ui_place_t place;

    lh_ui_container_init(&box, rect_of(100, 50, 240, 160));
    lh_ui_entity_set_style(lh_ui_container_as_entity(&box), &style);
    lh_ui_image_init(&picture, rect_of(0, 0, 16, 3), &mask);
    lh_ui_label_init(&caption, rect_of(0, 0, 0, 16), "");
    lh_ui_place_init(&place, lh_ui_place_size_wrap, lh_ui_scalar(0));
    lh_ui_place_set_align(&place, lh_ui_place_align_center);
    lh_ui_entity_set_place(lh_ui_image_as_entity(&picture), &place);
    lh_ui_place_init(&place, lh_ui_place_size_fixed, lh_ui_scalar(20));
    lh_ui_place_set_align(&place, lh_ui_place_align_center);
    lh_ui_entity_set_place(lh_ui_label_as_entity(&caption), &place);
    lh_ui_entity_add_child(lh_ui_container_as_entity(&box), lh_ui_image_as_entity(&picture));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&box), lh_ui_label_as_entity(&caption));
    lh_ui_layout_init(&layout, lh_ui_axis_horizontal, lh_ui_scalar(4));
    lh_ui_layout_set_justify(&layout, lh_ui_justify_center);
    lh_ui_container_set_layout(&box, &layout);

    /* Nothing runs the pass by hand: the frame asks the box for its children. */
    lh_ui_point_t offset;
    lh_ui_entity_get_children_transform(lh_ui_container_as_entity(&box), &offset);

    const lh_ui_rect_t picture_rect = lh_ui_entity_get_rect(lh_ui_image_as_entity(&picture));
    const lh_ui_rect_t caption_rect = lh_ui_entity_get_rect(lh_ui_label_as_entity(&caption));
    /* 4 of picture, 4 of gap, 20 of caption, in a 224 wide content box. */
    const int start = 108 + (224 - 28) / 2;

    /* Along the row the picture is as long as what it shows. Across the row a
       child keeps the size it was given — that is the flow's rule, not the
       image's, so the height here is the 3 it was handed. */
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(&picture_rect)), 4)
        << "the flow stretched the picture to the box it was given";
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(&picture_rect)), 3);
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(&caption_rect)), start + 4 + 4)
        << "the caption did not follow the picture";
}

/* A mask is cropped to ink and knows nothing about the type it came out of, so a
   picture starts with no line at all — and saying so is the contract: a row that
   lines up on a baseline (::lh_ui_place_align_baseline) falls back to centring for
   a child that answers -1, because -1 is not a line to stand on. */
TEST(entity_image, a_picture_has_no_baseline_until_it_is_given_one)
{
    lh_ui_image_t image;
    const lh_ui_mask_t mask = block_mask();

    lh_ui_image_init(&image, rect_of(0, 0, 4, 3), &mask);

    EXPECT_EQ(lh_ui_image_get_baseline(&image), lh_ui_scalar(-1));
    EXPECT_EQ(lh_ui_entity_get_baseline(lh_ui_image_as_entity(&image)), lh_ui_scalar(-1))
        << "the entity answered for itself instead of asking the picture";

    lh_ui_image_set_baseline(&image, lh_ui_scalar(3));

    EXPECT_EQ(lh_ui_image_get_baseline(&image), lh_ui_scalar(3));
    EXPECT_EQ(lh_ui_entity_get_baseline(lh_ui_image_as_entity(&image)), lh_ui_scalar(3))
        << "the baseline did not get out through the event";
}