#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/tiny_font.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/text.h>
#include <lh/ui/text/align.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;
using lh_test::tiny_font;

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;
    lh_ui_point_init(lh_addr_of(point), x, y);
    return point;
}

TEST(ui_text, next_code_reads_one_code_and_stops_at_the_end)
{
    const lh_char_t *cursor = "Ab";

    EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), static_cast<lh_u32_t>('A'));
    EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), static_cast<lh_u32_t>('b'));
    EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), 0U);
    EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), 0U);
    EXPECT_EQ(*cursor, '\0');
}

TEST(ui_text, is_line_end_takes_newline_and_end)
{
    EXPECT_EQ(lh_ui_text_is_line_end('\n'), lh_bool_true);
    EXPECT_EQ(lh_ui_text_is_line_end(0U), lh_bool_true);
    EXPECT_EQ(lh_ui_text_is_line_end('A'), lh_bool_false);
}

TEST(ui_text, lines_split_on_newline)
{
    const lh_char_t *text = "AB\nA\n";

    EXPECT_EQ(lh_ui_text_get_line_end(text), text + 2);
    EXPECT_EQ(lh_ui_text_get_next_line(text), text + 3);
    EXPECT_EQ(lh_ui_text_get_next_line(text + 3), text + 5);
    EXPECT_TRUE(lh_null_eq(lh_ui_text_get_next_line(text + 5)));
    EXPECT_EQ(lh_ui_text_count_lines(text), 3U);
    EXPECT_EQ(lh_ui_text_count_lines(""), 1U);
    EXPECT_EQ(lh_ui_text_count_lines(nullptr), 0U);
}

TEST(ui_text, width_sums_advances_and_takes_the_widest_line)
{
    EXPECT_EQ(lh_ui_text_get_line_width(tiny_font(), "AB\nA"), lh_ui_scalar(7));
    EXPECT_EQ(lh_ui_text_get_line_width(tiny_font(), "AZB"), lh_ui_scalar(7));
    EXPECT_EQ(lh_ui_text_get_width(tiny_font(), "A\nBB\nA"), lh_ui_scalar(8));
    EXPECT_EQ(lh_ui_text_get_width(tiny_font(), nullptr), lh_ui_scalar(0));
}

TEST(ui_text, size_and_rect_are_widest_line_by_lines)
{
    const lh_ui_size_t size = lh_ui_text_get_size(tiny_font(), "AB\nA");

    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(size)), lh_ui_scalar(7));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(size)), lh_ui_scalar(4));
    EXPECT_TRUE(rect_is(lh_ui_text_get_rect(tiny_font(), "AB\nA", point_of(5, 6)), rect_of(5, 6, 7, 4)));
    EXPECT_TRUE(rect_is(lh_ui_text_get_line_rect(tiny_font(), "B\nAA", point_of(1, 2)), rect_of(1, 2, 4, 2)));
}

TEST(ui_text, the_ink_is_shorter_than_the_line_and_starts_below_its_top)
{
    const lh_ui_font_t *font = lh_ui_font_get_default();
    const lh_ui_point_t origin = point_of(0, 0);
    const lh_ui_size_t size = lh_ui_text_get_size(font, "Hide panel");
    const lh_ui_rect_t ink = lh_ui_text_get_ink_rect(font, "Hide panel", origin);

    /* The whole reason the rule exists: Roboto 16 px is a 22 px line whose ink
       starts five rows down, so a size that answered "the line" was centring
       the padding and drew every caption low. */
    EXPECT_LT(lh_ui_size_get_height(lh_addr_of(size)),
              lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font)));
    EXPECT_GT(lh_ui_text_get_ink_top(font, "Hide panel"), lh_ui_scalar(0));
    /* @p origin is where the ink goes, and the rect says so. */
    EXPECT_EQ(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink))),
              lh_ui_point_get_y(lh_addr_of(origin)));
}

/* The box a text is centred in is the cap line down to the baseline — a metric
   of the font, the same for every word it draws. The ink is the other end of
   the same mistake: it is as tall as the tallest and the lowest letter of *this*
   word, so a caption whose text has a descender sat a row lower than the same
   caption without one, and an odd ink in an even box has no exact middle to sit
   on at all. */
TEST(ui_text, the_size_is_the_cap_line_to_the_baseline_and_not_the_ink)
{
    const lh_ui_font_t *font = lh_ui_font_get_default();
    const lh_ui_scalar_t cap = lh_ui_scalar(lh_ui_font_get_cap_height(font));
    const lh_ui_point_t origin = point_of(0, 0);
    const lh_ui_size_t with_tail = lh_ui_text_get_size(font, "Hide panel");
    const lh_ui_size_t without = lh_ui_text_get_size(font, "Hide");
    const lh_ui_rect_t tail_ink = lh_ui_text_get_ink_rect(font, "Hide panel", origin);
    const lh_ui_rect_t ink = lh_ui_text_get_ink_rect(font, "Hide", origin);

    EXPECT_GT(cap, lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(with_tail)), cap);
    /* A descender hangs below the box instead of pushing it: the box is where the
       letters stand, and the ink is bigger than the box says. */
    EXPECT_LT(lh_ui_size_get_height(lh_addr_of(with_tail)),
              lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(tail_ink))));
    /* With no descender the two are the same, which is what keeps the box honest
       for the many strings that have none. */
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(without)),
              lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink))));
    /* The width is still the pen's either way. */
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(with_tail)),
              lh_ui_text_get_line_width(font, "Hide panel"));
}

/* Nothing inked is nothing drawn, whatever the font is: a label with a space in
   it has to collapse like an empty one, or every gap in a layout holds a row. */
TEST(ui_text, a_text_with_no_ink_in_it_has_no_height)
{
    const lh_ui_size_t spaces = lh_ui_text_get_size(tiny_font(), "  ");
    const lh_ui_size_t empty = lh_ui_text_get_size(tiny_font(), "");
    const lh_ui_size_t none = lh_ui_text_get_size(tiny_font(), nullptr);

    EXPECT_EQ(lh_ui_scalar(lh_ui_text_count_lines("")), lh_ui_scalar(1));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(spaces)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(empty)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(none)), lh_ui_scalar(0));
}

TEST(ui_text, the_pixels_land_where_the_ink_rect_says)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_font_t *font = lh_ui_font_get_default();
    const lh_ui_point_t origin = point_of(3, 4);
    const lh_ui_rect_t ink = lh_ui_text_get_ink_rect(font, "Hide panel", origin);
    lh_s32_t top = 0;
    lh_s32_t bottom = 0;
    int i;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false, true);

    lh_ui_text_draw(lh_addr_of(canvas), font, "Hide panel", origin, lh_addr_of(color));

    ASSERT_GT(log.mask_count, 0);
    for (i = 0; i < log.mask_count; ++i)
    {
        const lh_ui_point_t *at = lh_ui_rect_get_origin_as_const(lh_addr_of(log.masks[i]));
        const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(lh_addr_of(log.masks[i]));
        const lh_s32_t y = static_cast<lh_s32_t>(lh_ui_point_get_y(at));
        const lh_s32_t tail = y + static_cast<lh_s32_t>(lh_ui_size_get_height(size));

        top = i == 0 || y < top ? y : top;
        bottom = i == 0 || tail > bottom ? tail : bottom;
    }
    /* Measure and draw are one rule: the pixels span the measured rect exactly,
       top to bottom. Drawing from the line box would start them rows higher. */
    EXPECT_EQ(top, lh_cast_static(lh_s32_t, lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink)))));
    EXPECT_EQ(bottom,
              lh_cast_static(lh_s32_t, lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink))) +
                  lh_cast_static(lh_s32_t, lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink))))));
}

TEST(ui_text, draw_places_each_glyph_at_the_pen)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false, true);

    lh_ui_text_draw(lh_addr_of(canvas), tiny_font(), "AZB\nB", point_of(10, 20), lh_addr_of(color));

    ASSERT_EQ(log.mask_count, 3);
    EXPECT_TRUE(rect_is(log.masks[0], rect_of(10, 20, 2, 2)));
    EXPECT_TRUE(rect_is(log.masks[1], rect_of(13, 20, 1, 1)));
    EXPECT_TRUE(rect_is(log.masks[2], rect_of(10, 22, 1, 1)));
}

TEST(ui_text, draw_code_returns_the_advance)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false, true);

    EXPECT_EQ(lh_ui_text_draw_code(lh_addr_of(canvas), tiny_font(), 'B', point_of(0, 0), lh_addr_of(color)),
              lh_ui_scalar(4));
    EXPECT_EQ(lh_ui_text_draw_code(lh_addr_of(canvas), tiny_font(), 'Z', point_of(0, 0), lh_addr_of(color)),
              lh_ui_scalar(0));
    EXPECT_EQ(log.mask_count, 1);
}

TEST(ui_text, lines_outside_the_clip_are_skipped)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t clip = rect_of(0, 0, 100, 3);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, true, true);
    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(clip));

    /* Lines at y 0, 2, 4, 6: the clip ends at 3, so only the first two show. */
    lh_ui_text_draw(lh_addr_of(canvas), tiny_font(), "A\nA\nA\nA", point_of(0, 0), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));

    EXPECT_EQ(log.mask_count, 2);
}

/* The alignment turns a box and a measured text into one point. What matters is
   that the padding is taken off first, that centre really is the middle, and that
   text with nowhere to go starts at the near edge instead of going off it. */

lh_ui_point_t
aligned_at(int box_x, int box_y, int box_w, int box_h, int pad, lh_ui_scalar_t text_w, lh_ui_scalar_t text_h,
           lh_ui_text_align_h_t horizontal, lh_ui_text_align_v_t vertical)
{
    lh_ui_rect_t box;
    lh_ui_insets_t padding;
    lh_ui_size_t size;

    lh_ui_rect_init(lh_addr_of(box), box_x, box_y, box_w, box_h);
    lh_ui_insets_init_all(lh_addr_of(padding), pad);
    lh_ui_size_init(lh_addr_of(size), text_w, text_h);
    return lh_ui_text_align_get_origin(lh_addr_of(box), lh_addr_of(padding), size, horizontal, vertical);
}

void
expect_origin(const lh_ui_point_t &at, int x, int y)
{
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(at)), x);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(at)), y);
}

TEST(ui_text_align, left_and_top_are_the_padded_corner)
{
    expect_origin(aligned_at(10, 20, 100, 40, 5, 30, 10, lh_ui_text_align_h_left, lh_ui_text_align_v_top), 15, 25);
    expect_origin(aligned_at(10, 20, 100, 40, 0, 30, 10, lh_ui_text_align_h_left, lh_ui_text_align_v_top), 10, 20);
}

TEST(ui_text_align, centre_is_the_middle_of_the_room_the_padding_leaves)
{
    /* 100 wide less 5 and 5 leaves 90, text 30: (90 - 30) / 2 = 30 past the left
       inset, so x is 15 + 30. Vertically 40 less 5 and 5 leaves 30, text 10, so
       (30 - 10) / 2 = 10 and y is 25 + 10. */
    expect_origin(aligned_at(10, 20, 100, 40, 5, 30, 10, lh_ui_text_align_h_center, lh_ui_text_align_v_center), 45, 35);
}

TEST(ui_text_align, right_and_bottom_end_at_the_far_edge)
{
    expect_origin(aligned_at(10, 20, 100, 40, 5, 30, 10, lh_ui_text_align_h_right, lh_ui_text_align_v_bottom), 75,
                  45);
}

TEST(ui_text_align, text_wider_or_taller_than_its_box_starts_at_the_near_edge)
{
    /* There is nowhere else to start, and a negative offset would push the first
       glyph off the box and out of the clip with it. */
    expect_origin(aligned_at(10, 20, 40, 12, 0, 100, 40, lh_ui_text_align_h_center, lh_ui_text_align_v_center), 10,
                  20);
    expect_origin(aligned_at(10, 20, 40, 12, 0, 100, 40, lh_ui_text_align_h_right, lh_ui_text_align_v_bottom), 10,
                  20);
}

TEST(ui_text_align, a_null_box_is_the_empty_one_and_a_null_padding_is_none)
{
    lh_ui_size_t size;
    lh_ui_rect_t box;

    lh_ui_size_init(lh_addr_of(size), 10, 10);
    lh_ui_rect_init(lh_addr_of(box), 7, 9, 30, 20);

    expect_origin(lh_ui_text_align_get_origin(lh_addr_of(box), (const lh_ui_insets_t *)lh_null, size,
                                               lh_ui_text_align_h_left, lh_ui_text_align_v_top),
                  7, 9);
    expect_origin(lh_ui_text_align_get_origin((const lh_ui_rect_t *)lh_null, (const lh_ui_insets_t *)lh_null, size,
                                               lh_ui_text_align_h_center, lh_ui_text_align_v_center),
                  0, 0);
}

} // namespace
