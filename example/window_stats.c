/* example/window_stats.c
 *
 * A runnable window that prints the live numbers from ::lh_ui_frame_stats_t
 * back into itself. The hook is what every faster-than-LVGL renderer rests on:
 * a paint that sees both what the window system asked for (the union of small
 * dirty rects) and what the renderer actually drew (often a single bounding
 * rectangle around the request). The ratio `drawn / asked` is the cost of
 * choosing one rectangle over a list of them, and the
 * `lh_ui_frame_stats_get_wasteful` count is the frames that ratio exceeds.
 *
 * Two invalidations far apart still arrive as one paint; this window forces
 * that by invalidating only a quarter band at the top of the client every
 * 100 ms. The painter clears the whole paint rectangle to make the metric
 * visible. Closing the window tears the app down through the close hook.
 */

#include <stdio.h>

#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/math/rect.h>
#include <lh/ui/frame/stats.h>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif

/*
 * Paint the running totals into the window's client area.
 *
 * The paint DC and the paint rectangle are read through the window -- they are
 * valid only while on_paint runs, and the window records this frame for the
 * stats before we get here.
 */
static lh_void
on_paint(lh_os_window_t *self, lh_ptr context)
{
    (void)context;

#if defined(_WIN32)
    HDC dc = (HDC)lh_os_window_get_paint_dc(self);
    int l = 0, t = 0, r = 0, b = 0;
    lh_os_window_get_paint_rect(self, &l, &t, &r, &b);
    RECT rc;
    rc.left = l;
    rc.top = t;
    rc.right = r;
    rc.bottom = b;
    FillRect(dc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));

    struct lh_ui_frame_stats *stats = lh_os_window_get_frame_stats(self);
    if (stats == NULL)
    {
        return;
    }

    lh_u32_t frames = lh_ui_frame_stats_get_frames(stats);
    lh_u32_t wasteful = lh_ui_frame_stats_get_wasteful(stats);
    lh_u64_t drawn = lh_ui_frame_stats_get_drawn_px(stats);
    lh_u64_t asked = lh_ui_frame_stats_get_asked_px(stats);
    lh_u64_t us = lh_ui_frame_stats_get_us(stats);
    lh_u64_t worst = lh_ui_frame_stats_get_worst_us(stats);

    double ratio = (asked != 0) ? ((double)drawn / (double)asked) : 0.0;
    double pct = (frames != 0u) ? (100.0 * (double)wasteful / (double)frames) : 0.0;

    char buf[256];
    int n = _snprintf(buf,
                      sizeof buf,
                      "frames %u  wasteful %u (%.1f%%)  drawn %llu  asked %llu"
                      "  ratio %.2fx  us %llu  worst %llu",
                      (unsigned)frames,
                      (unsigned)wasteful,
                      pct,
                      (unsigned long long)drawn,
                      (unsigned long long)asked,
                      ratio,
                      (unsigned long long)us,
                      (unsigned long long)worst);

    SetBkMode(dc, TRANSPARENT);
    TextOutA(dc, 8, 8, buf, n);
#else
    (void)self;
#endif
}

/*
 * Periodic repaint of a narrow band at the top of the client, so the
 * ask-the-paint DC, draw-on-hull shape is repeated and the metric moves.
 *
 * A paint that runs at 10 Hz against a 32 Hz timer resolution on Win32 lands
 * here ten times a second; each fires an invalidate_rect of a strip the size
 * of which the painter clears. The stats hook measures the hull it took.
 */
static lh_void
on_tick(lh_os_window_t *self, lh_ptr context)
{
    (void)context;
    int w = 0;
    int h = 0;
    if (!lh_os_window_get_client_size(self, &w, &h))
    {
        return;
    }
    int band = h / 4;
    if (band < 1)
    {
        band = 1;
    }
    lh_os_window_invalidate_rect(self, 0, 0, w, band);
}

/*
 * Log every frame the window drew, so the same numbers live in the run log
 * as on the screen. Useful when the window is hidden under another one.
 */
static lh_void
on_frame(const lh_ui_frame_t *frame, lh_ptr context)
{
    (void)context;
    const lh_math_point_t *origin = lh_math_rect_get_origin_as_const(&frame->drawn);
    const lh_math_size_t *size = lh_math_rect_get_size_as_const(&frame->drawn);
    long long x = (long long)lh_math_point_get_x(origin);
    long long y = (long long)lh_math_point_get_y(origin);
    long long w = (long long)lh_math_size_get_width(size);
    long long h = (long long)lh_math_size_get_height(size);
    fprintf(stderr,
            "frame draw=[%lld,%lld,%lld,%lld] asked=%llu rects=%u us=%llu\n",
            x,
            y,
            x + w,
            y + h,
            (unsigned long long)frame->asked_px,
            (unsigned)frame->rects,
            (unsigned long long)frame->us);
}

/* Close the app when the user closes the window. */
static lh_void
on_close(lh_os_window_t *self, lh_os_window_close_reason_t reason, lh_ptr context)
{
    (void)reason;
    (void)context;
    struct lh_os_app *app = lh_os_window_get_app(self);
    if (app != NULL)
    {
        lh_os_app_quit(app);
    }
}

int
main(void)
{
    lh_os_app_t app;
    lh_os_window_t win;
    lh_ui_frame_stats_t stats;

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(win));
    lh_ui_frame_stats_init(lh_addr_of(stats));

    lh_ui_frame_stats_set_on_frame(lh_addr_of(stats), on_frame, NULL);
    lh_os_window_set_frame_stats(lh_addr_of(win), lh_addr_of(stats));
    lh_os_window_set_on_paint(lh_addr_of(win), on_paint, NULL);
    lh_os_window_set_on_tick(lh_addr_of(win), on_tick, NULL, 100);
    lh_os_window_set_on_close(lh_addr_of(win), on_close, NULL);

    if (!lh_os_window_open(lh_addr_of(app), lh_addr_of(win), "lh frame stats", 720, 60))
    {
        fprintf(stderr, "lh_os_window_open failed\n");
        return 1;
    }

    lh_os_app_run(lh_addr_of(app));

    lh_os_window_deinit(lh_addr_of(win));
    lh_os_app_deinit(lh_addr_of(app));
    return 0;
}
