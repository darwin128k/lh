#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/frame/stats.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

namespace
{
lh_ui_frame_t
frame_of(int w, int h, lh_bool_t known, lh_u64_t asked, lh_u32_t rects, lh_u64_t us)
{
    lh_ui_frame_t f;

    lh_ui_rect_init(&f.drawn, 0, 0, w, h);
    f.asked_known = known;
    f.asked_px = asked;
    f.rects = rects;
    f.us = us;
    return f;
}

void
count_calls(const lh_ui_frame_t *frame, lh_ptr context)
{
    (void)frame;
    ++*static_cast<int *>(context);
}
} // namespace

TEST(ui_frame_stats, a_known_frame_adds_drawn_and_asked_pixels)
{
    lh_ui_frame_stats_t s;
    const lh_ui_frame_t f = frame_of(10, 4, lh_bool_true, 30, 2, 100);

    lh_ui_frame_stats_init(&s);
    lh_ui_frame_stats_record(&s, &f);

    EXPECT_EQ(lh_ui_frame_stats_get_frames(&s), 1U);
    EXPECT_EQ(lh_ui_frame_stats_get_unknown(&s), 0U);
    EXPECT_EQ(lh_ui_frame_stats_get_drawn_px(&s), 40U);
    EXPECT_EQ(lh_ui_frame_stats_get_asked_px(&s), 30U);
    EXPECT_EQ(lh_ui_frame_stats_get_rects(&s), 2U);
    EXPECT_EQ(lh_ui_frame_stats_get_us(&s), 100U);
    EXPECT_EQ(lh_ui_frame_stats_get_wasteful(&s), 0U) << "40 is not twice 30";
}

/* A frame whose request the platform could not read is still a frame and still took
   time, but its drawing is measured against nothing: counting it in the pixel totals
   would make the ratio worse by an amount nobody measured. */
TEST(ui_frame_stats, an_unknown_frame_counts_its_time_and_not_its_pixels)
{
    lh_ui_frame_stats_t s;
    const lh_ui_frame_t f = frame_of(10, 4, lh_bool_false, 0, 0, 70);

    lh_ui_frame_stats_init(&s);
    lh_ui_frame_stats_record(&s, &f);

    EXPECT_EQ(lh_ui_frame_stats_get_frames(&s), 1U);
    EXPECT_EQ(lh_ui_frame_stats_get_unknown(&s), 1U);
    EXPECT_EQ(lh_ui_frame_stats_get_drawn_px(&s), 0U);
    EXPECT_EQ(lh_ui_frame_stats_get_us(&s), 70U);
    EXPECT_EQ(lh_ui_frame_stats_get_wasteful(&s), 0U);
}

/* Two rows far apart are a small request and a big hull: that is the frame worth
   counting, and "twice" is the line, inclusive. */
TEST(ui_frame_stats, a_frame_that_drew_twice_the_request_is_wasteful)
{
    lh_ui_frame_stats_t s;
    const lh_ui_frame_t twice = frame_of(10, 4, lh_bool_true, 20, 2, 1);
    const lh_ui_frame_t under = frame_of(10, 4, lh_bool_true, 21, 2, 1);

    lh_ui_frame_stats_init(&s);
    lh_ui_frame_stats_record(&s, &twice);
    lh_ui_frame_stats_record(&s, &under);

    EXPECT_EQ(lh_ui_frame_stats_get_wasteful(&s), 1U);
}

TEST(ui_frame_stats, the_worst_frame_is_kept)
{
    lh_ui_frame_stats_t s;
    const lh_ui_frame_t a = frame_of(1, 1, lh_bool_true, 1, 1, 300);
    const lh_ui_frame_t b = frame_of(1, 1, lh_bool_true, 1, 1, 900);
    const lh_ui_frame_t c = frame_of(1, 1, lh_bool_true, 1, 1, 200);

    lh_ui_frame_stats_init(&s);
    lh_ui_frame_stats_record(&s, &a);
    lh_ui_frame_stats_record(&s, &b);
    lh_ui_frame_stats_record(&s, &c);

    EXPECT_EQ(lh_ui_frame_stats_get_worst_us(&s), 900U);
    EXPECT_EQ(lh_ui_frame_stats_get_us(&s), 1400U);
}

TEST(ui_frame_stats, the_callback_sees_every_frame_and_stops_when_cleared)
{
    lh_ui_frame_stats_t s;
    const lh_ui_frame_t f = frame_of(1, 1, lh_bool_true, 1, 1, 1);
    int calls = 0;

    lh_ui_frame_stats_init(&s);
    lh_ui_frame_stats_set_on_frame(&s, count_calls, &calls);
    lh_ui_frame_stats_record(&s, &f);
    lh_ui_frame_stats_record(&s, &f);
    lh_ui_frame_stats_set_on_frame(&s, lh_null, lh_null);
    lh_ui_frame_stats_record(&s, &f);

    EXPECT_EQ(calls, 2);
    EXPECT_EQ(lh_ui_frame_stats_get_frames(&s), 3U);
}

TEST(ui_frame_stats, an_empty_rect_has_no_area)
{
    lh_ui_rect_t r;

    lh_ui_rect_init(&r, 5, 5, 0, 10);
    EXPECT_EQ(lh_ui_frame_area(&r), 0U);
    lh_ui_rect_init(&r, 5, 5, 3, 7);
    EXPECT_EQ(lh_ui_frame_area(&r), 21U);
}
