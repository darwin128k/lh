#include <cstring>

#include <gtest/gtest.h>

#include <lh/test/ui/fill_probe.h>
#include <lh/test/ui/tiny_font.h>

#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/font.h>
#include <lh/ui/entity.h>
#include <lh/ui/label.h>
#include <lh/ui/view.h>
#include <lh/ui/paint.h>
#include <lh/ui/pixmap.h>
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
    lh_ui_label_t label;
    lh_ui_label_init(lh_addr_of(label), rect, text);
    lh_ui_entity_t *entity = lh_ui_label_as_entity(lh_addr_of(label));
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(entity);

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_label_get_text(lh_addr_of(label)), text);
    EXPECT_EQ(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_label_class));
    EXPECT_EQ(lh_ui_label_as_container(lh_addr_of(label)), lh_addr_of(label.container));
    EXPECT_EQ(lh_ui_container_as_entity(lh_ui_label_as_container(lh_addr_of(label))),
              entity);
}

TEST(entity_label, set_text_replaces_the_pointer)
{
    lh_ui_label_t label;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_label_init(lh_addr_of(label), rect, "one");
    const lh_char_t *text = "two";

    lh_ui_label_set_text(lh_addr_of(label), text);

    EXPECT_EQ(lh_ui_label_get_text(lh_addr_of(label)), text);
}

TEST(entity_label, draw_goes_through_the_embedded_entity)
{
    lh_ui_label_t label;
    lh_ui_rect_t rect;
    lh_ui_canvas_t canvas;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_label_init(lh_addr_of(label), rect, "x");
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);

    lh_ui_entity_draw(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));
}

/* Contract: a derived class keeps the base fill by calling
 * lh_ui_entity_class_event_base. The label class does. */
TEST(entity_label, draw_keeps_the_base_class_fill)
{
    lh_ui_label_t label;
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
    lh_ui_label_init(lh_addr_of(label), rect, "x");
    lh_ui_entity_set_style(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(style));
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_backend(), rect,
                             color);

    lh_ui_entity_draw(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));

    EXPECT_EQ(probe.matches, 1);
}

TEST(entity_label, padding_moves_the_text_in_from_the_corner)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_style_set_padding(&style, lh_ui_scalar(6));
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_label_get_text_origin(&label);
    EXPECT_EQ(lh_ui_point_get_x(&origin), lh_ui_scalar(16));
    EXPECT_EQ(lh_ui_point_get_y(&origin), lh_ui_scalar(26));
}

TEST(entity_label, padding_sides_move_the_text_by_left_and_top)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_insets_t sides;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_insets_init(&sides, 3, 5, 30, 50);
    lh_ui_style_set_padding_insets(&style, &sides);
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_label_get_text_origin(&label);
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
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_label_init(&label, rect, text);
    lh_ui_style_init(&style);
    lh_ui_style_set_align_h(&style, horizontal);
    lh_ui_style_set_align_v(&style, vertical);
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);
    return lh_ui_label_get_text_origin(&label);
}

TEST(entity_label, a_style_that_says_nothing_keeps_the_text_at_the_corner)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;
    lh_ui_insets_t sides;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_insets_init(&sides, 6, 6, 6, 6);
    lh_ui_label_init(&label, rect, "x");
    lh_ui_style_init(&style);
    lh_ui_style_set_padding_insets(&style, &sides);
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);

    const lh_ui_point_t origin = lh_ui_label_get_text_origin(&label);
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

/* What "centre" has to mean to be worth anything: the middle of the box is the
   middle of the text that stands on it. The first version centred the font's line
   box (Roboto 16 px: 22 rows) and drew every caption a few pixels low; the second
   centred the ink, which is as tall as the tallest and the lowest letter of *this*
   word, so 15 rows of ink in a 28-row box is a coin toss that truncation always
   lost — measured on the demo's button, the text sat on 193.5 against a middle of
   194.0 and read high. The cap line to the baseline is 12 rows and 28 - 12 is
   even, so it lands on the middle with nothing left over, and it is the same for
   every word the font draws. */
TEST(entity_label, centring_puts_the_cap_line_on_the_middle_of_the_box)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;
    lh_ui_rect_t ink;
    lh_ui_size_t size;
    lh_ui_point_t origin;
    lh_ui_scalar_t middle;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 132, 28);
    lh_ui_label_init(lh_addr_of(label), rect, "Hide panel");
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_align_h(lh_addr_of(style), lh_ui_text_align_h_center);
    lh_ui_style_set_align_v(lh_addr_of(style), lh_ui_text_align_v_center);
    lh_ui_entity_set_style(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(style));

    size = lh_ui_text_get_size(lh_ui_font_get_default(), "Hide panel");
    origin = lh_ui_label_get_text_origin(lh_addr_of(label));
    ink = lh_ui_label_get_text_rect(lh_addr_of(label));
    middle = lh_ui_scalar(14);

    /* Exactly, not nearly: the leftover of 28 - 12 is even, and this is the whole
       reason the cap line was chosen over both the line box and the ink. */
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(origin)) + lh_ui_size_get_height(lh_addr_of(size)) / 2, middle);
    /* And the two are genuinely different things, so a pass here cannot be the ink
       passing: the tail of the 'p' hangs below the box. */
    EXPECT_GT(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink))),
              lh_ui_size_get_height(lh_addr_of(size)));
    EXPECT_EQ(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink))),
              lh_ui_point_get_y(lh_addr_of(origin)));
}

/* The box a label is centred in is the cap line to the baseline, and the ink
   hangs below it: the tail of a 'p' sits under a box that says twelve. A clip
   that catches only that tail catches no part of the box, so a cull that asked
   about the box alone threw the whole label away and the tail of that frame came
   out bare — the same defect as a shadow past its own rect, one layer down. */
/* The box a label is centred in is the cap line to the baseline, and the pixels
   hang below it. Whatever the box is for — centring, the flow's cross axis, the
   cull — it is not a knife: the tail of a 'p' drawn inside a twelve-row box has
   to come out whole below it. */
TEST(entity_label, the_ink_is_not_cut_by_the_box_the_text_is_centred_in)
{
    lh_u32_t words[64 * 40];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t box;
    lh_ui_rect_t ink;
    lh_ui_color_t ink_color;
    lh_ui_paint_t text_paint;
    lh_ui_scalar_t box_bottom;
    lh_ui_scalar_t lit_below;
    lh_u32_t w;

    for (lh_u32_t &word : words)
    {
        word = 0x00204060u;
    }
    lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), lh_ui_scalar(64), lh_ui_scalar(40),
                      lh_ui_scalar(64 * 4), lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_init(lh_addr_of(sw));
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(sw), lh_addr_of(pixmap));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_sw), lh_addr_of(sw));

    lh_ui_rect_init(lh_addr_of(box), 0, 0, 132, 12);
    lh_ui_label_init(lh_addr_of(label), box, "Hide panel");
    lh_ui_style_init(lh_addr_of(style));
    /* White on purpose: a style starts with black text, and a test that counts
       bright pixels of black text counts nothing and would pass on a label that
       draws no tail at all. */
    lh_ui_color_init(lh_addr_of(ink_color), 255, 255, 255, 255);
    lh_ui_paint_init_color(lh_addr_of(text_paint), lh_addr_of(ink_color));
    lh_ui_style_set_text(lh_addr_of(style), lh_addr_of(text_paint));
    lh_ui_entity_set_style(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(style));
    ink = lh_ui_label_get_text_rect(lh_addr_of(label));
    box_bottom = lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(box))) +
                 lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(box)));

    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_entity_draw(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));

    /* Denominator first: the tail has to reach below the box, or nothing below it
       would prove anything. */
    EXPECT_GT(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink))) +
                  lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink))),
              box_bottom);
    lit_below = lh_ui_scalar(0);
    for (int y = static_cast<int>(box_bottom); y < 40; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            w = words[y * 64 + x];
            if (((w & 0x00FFFFFFu) >> 16) > 140)
            {
                ++lit_below;
            }
        }
    }
    EXPECT_GT(lit_below, lh_ui_scalar(0))
        << "no pixel of the tail was drawn under the box: the box is cutting the ink";
}

TEST(entity_label, a_label_shows_on_a_clip_that_catches_only_the_ink_below_its_box)
{
    lh_test::fill_probe probe;
    lh_ui_canvas_t canvas;
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t box;
    lh_ui_rect_t ink;
    lh_ui_rect_t tail;
    lh_ui_point_t none;
    lh_ui_entity_t *entity;
    lh_ui_scalar_t box_bottom;
    lh_ui_scalar_t ink_bottom;

    lh_ui_rect_init(lh_addr_of(box), 0, 0, 132, 12);
    lh_ui_label_init(lh_addr_of(label), box, "Hide panel");
    /* The font is a field of the style, so a label with none has no ink to
       overflow and the whole test would pass on a label that draws nothing. */
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_entity_set_style(lh_ui_label_as_entity(lh_addr_of(label)), lh_addr_of(style));
    entity = lh_ui_label_as_entity(lh_addr_of(label));
    ink = lh_ui_label_get_text_rect(lh_addr_of(label));

    /* Denominator first: the tail has to be under the box, or the clip below
       catches the box too and the whole test would pass on its own. */
    box_bottom = lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(box))) +
                 lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(box)));
    ink_bottom = lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink))) +
                 lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink)));
    EXPECT_GT(ink_bottom, box_bottom);

    lh_ui_rect_init(lh_addr_of(tail), 0, box_bottom, 132, ink_bottom - box_bottom);
    EXPECT_EQ(lh_ui_rect_intersects(lh_addr_of(box), lh_addr_of(tail)), lh_bool_false);

    lh_ui_point_init(lh_addr_of(none), 0, 0);
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_backend(), box,
                             lh_ui_color_t{0, 0, 0, 255});
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_push(lh_addr_of(canvas), none, lh_addr_of(tail));
    EXPECT_EQ(lh_ui_entity_shows_on(entity, lh_addr_of(canvas)), lh_bool_true)
        << "the label was culled with only its tail in the clip";
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
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

/* ── The label's baseline ────────────────────────────────────────────────────
   A row of an icon and a caption stands on one line rather than in the middle of
   the same room twice (::lh_ui_place_align_baseline), and what the label answers
   is the row the letters are standing on: measured down from the top of its own
   box, so a row of a caption and an icon reads the same wherever the caption is.

   The tiny font makes the arithmetic exact: "A" is 2 rows of cap, its ink starts
   at the very top of the line (ascent 2, top -2), and the line is therefore 2 rows
   down from the top of the box the text is drawn in. */

TEST(entity_label, the_baseline_is_a_cap_height_below_where_the_ink_goes)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;
    lh_ui_point_t origin;

    lh_ui_rect_init(&rect, 10, 20, 100, 50);
    lh_ui_label_init(&label, rect, "A");
    lh_ui_style_init(&style);
    lh_ui_style_set_font(&style, lh_test::tiny_font());
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);

    origin = lh_ui_label_get_text_origin(&label);
    fprintf(stderr, "        ink at y %d, baseline %d\n", (int)lh_ui_point_get_y(&origin),
            (int)lh_ui_label_get_baseline(&label));
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(2));
}

/* The line the letters stand on is where the **tallest letter of this run**
   ends, which is not always the cap line. A 'd' rises above the cap height, so the
   ink starts one row higher and the baseline that goes with it is one row lower
   than the cap block's bottom — measured on ::lh_test::cap_font (line 8, ascent 6,
   cap 4): "H" gives 4 and "d" gives 5, and both letters' ink ends on row 6 = the
   ascent, which is what a baseline is.

   This is the row that tells the cap baseline from the real one, and it needs the
   second font: the tiny font's ascent, cap and line are all 2, so it would hand
   both rules the same answer and stay green either way. */
TEST(entity_label, the_baseline_is_where_the_tallest_letter_of_the_run_ends)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;
    const lh_ui_font_t *font = lh_test::cap_font();
    lh_ui_point_t origin;

    lh_ui_rect_init(&rect, 0, 0, 100, 20);
    lh_ui_style_init(&style);
    lh_ui_style_set_font(&style, font);

    lh_ui_label_init(&label, rect, "f");
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);
    fprintf(stderr, "        cap %d, ascent %d, line %d\n", (int)lh_ui_font_get_cap_height(font),
            (int)lh_ui_font_get_ascent(font), (int)lh_ui_font_get_line_height(font));
    origin = lh_ui_label_get_text_origin(&label);
    fprintf(stderr, "        \"f\" ink at y %d, baseline %d\n", (int)lh_ui_point_get_y(&origin),
            (int)lh_ui_label_get_baseline(&label));
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(4)) << "a capital is the cap line";

    lh_ui_label_set_text(&label, "d");
    fprintf(stderr, "        \"d\" baseline %d\n", (int)lh_ui_label_get_baseline(&label));
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(5))
        << "an ascender rises above the cap line and takes the baseline with it";
    EXPECT_NE(lh_ui_label_get_baseline(&label), lh_cast_static(lh_ui_scalar_t,
                                                              lh_ui_font_get_cap_height(font)))
        << "the baseline is the cap line whatever the run says";
}

/* Rows down from its own box, not from the top of the page: a caption that has
   been moved by the flow carries its line with it, which is the whole point — the
   icon beside it asks the caption, not the screen. */
TEST(entity_label, the_baseline_is_measured_from_the_box_and_not_from_the_page)
{
    lh_ui_label_t here;
    lh_ui_label_t there;
    lh_ui_style_t style;
    lh_ui_rect_t near_rect;
    lh_ui_rect_t far_rect;

    lh_ui_rect_init(&near_rect, 0, 0, 100, 20);
    lh_ui_rect_init(&far_rect, 0, 180, 100, 20);
    lh_ui_label_init(&here, near_rect, "A");
    lh_ui_label_init(&there, far_rect, "A");
    lh_ui_style_init(&style);
    lh_ui_style_set_font(&style, lh_test::tiny_font());
    lh_ui_entity_set_style(lh_ui_label_as_entity(&here), &style);
    lh_ui_entity_set_style(lh_ui_label_as_entity(&there), &style);

    EXPECT_EQ(lh_ui_label_get_baseline(&there), lh_ui_label_get_baseline(&here))
        << "moving a caption moved its line with it";
}

/* Padding is part of the box, so it moves the line with the text: a label padded
   5 rows in from the top has its text 5 rows down and its line 7. */
TEST(entity_label, padding_moves_the_baseline_with_the_text)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 100, 20);
    lh_ui_label_init(&label, rect, "A");
    lh_ui_style_init(&style);
    lh_ui_style_set_font(&style, lh_test::tiny_font());
    lh_ui_style_set_padding(&style, lh_ui_scalar(5));
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);

    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(7));
}

/* Nothing drawn stands on nothing: a label with no text, with an empty one, or
   with no style to give it a font has no line, and a row that has only these has
   none either and centres them as it did before. */
TEST(entity_label, a_label_with_nothing_in_it_has_no_line)
{
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 100, 20);
    lh_ui_style_init(&style);
    lh_ui_style_set_font(&style, lh_test::tiny_font());

    lh_ui_label_init(&label, rect, "A");
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);
    EXPECT_NE(lh_ui_label_get_baseline(&label), lh_ui_scalar(-1)) << "a drawn label has no line";

    lh_ui_label_set_text(&label, "C"); /* the tiny font's 'C' has no ink */
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(-1))
        << "a label whose text is not drawn is standing on a line";

    lh_ui_label_set_text(&label, "");
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(-1));

    lh_ui_label_set_text(&label, "A");
    lh_ui_entity_set_style(lh_ui_label_as_entity(&label), lh_null);
    EXPECT_EQ(lh_ui_label_get_baseline(&label), lh_ui_scalar(-1))
        << "a label with no font to measure has no line";
}

/* A label draws text wider than its box rather than cut it, so what it has on screen is
   not its box. The window learned that the hard way: "Конфигуратор" in a 38-wide crumb
   was replaced by "JL205" and the tail "...игуратор" stayed, because only the box was
   recorded. Three claims, one per way that goes wrong. */
namespace
{
struct label_in_view
{
    lh_ui_entity_t root;
    lh_ui_label_t label;
    lh_ui_style_t style;
    lh_ui_view_t view;
    char buffer[16];

    label_in_view()
    {
        lh_ui_rect_t r;

        lh_ui_rect_init(&r, 0, 0, 200, 40);
        lh_ui_entity_init(&root, r);
        lh_ui_style_init(&style);
        lh_ui_style_set_font(&style, lh_test::tiny_font());
        lh_ui_rect_init(&r, 10, 10, 4, 4); /* narrower than any of the texts below */
        lh_ui_label_init(&label, r, "A");
        lh_ui_entity_set_style(lh_ui_label_as_entity(&label), &style);
        lh_ui_entity_add_child(&root, lh_ui_label_as_entity(&label));
        lh_ui_view_init(&view);
        lh_ui_view_set_root(&view, &root);
    }

    lh_bool_t
    take(lh_ui_rect_t *out)
    {
        return lh_ui_view_take_damage(&view, out);
    }

    static lh_ui_scalar_t
    right_of(const lh_ui_rect_t *r)
    {
        return lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(r)) +
               lh_ui_size_get_width(lh_ui_rect_get_size_as_const(r));
    }

    lh_ui_scalar_t
    right_of_ink(const char *text)
    {
        return 10 + lh_ui_text_get_width(lh_test::tiny_font(), text);
    }
};
} // namespace

TEST(entity_label, set_text_with_the_same_bytes_records_nothing)
{
    label_in_view t;
    lh_ui_rect_t damage;

    std::strcpy(t.buffer, "A");
    lh_ui_label_set_text(&t.label, t.buffer); /* the bytes it was built with */
    EXPECT_EQ(t.take(&damage), lh_bool_false) << "same text at a new address is no change";
}

TEST(entity_label, set_text_records_the_text_that_overflows_the_box)
{
    label_in_view t;
    lh_ui_rect_t damage;

    lh_ui_label_set_text(&t.label, "AAAAAA");
    ASSERT_EQ(t.take(&damage), lh_bool_true);
    EXPECT_GE(label_in_view::right_of(&damage), t.right_of_ink("AAAAAA") - 1)
        << "the box is 4 wide and the text is not: the damage has to reach the text";
}

TEST(entity_label, set_text_erases_a_longer_text_whose_buffer_was_rewritten_first)
{
    label_in_view t;
    lh_ui_rect_t damage;

    std::strcpy(t.buffer, "AAAAAA");
    lh_ui_label_set_text(&t.label, t.buffer);
    (void)t.take(&damage);

    /* The usual way a cell is updated: the bytes change first, then the label is told.
       By then the old text cannot be measured, and its tail is still on screen. */
    std::strcpy(t.buffer, "A");
    lh_ui_label_set_text(&t.label, t.buffer);
    ASSERT_EQ(t.take(&damage), lh_bool_true);
    EXPECT_GE(label_in_view::right_of(&damage), t.right_of_ink("AAAAAA") - 1)
        << "the old, longer text has to be erased, not only the new one drawn";
}
