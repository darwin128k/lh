#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/tiny_font.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/text.h>
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

} // namespace
