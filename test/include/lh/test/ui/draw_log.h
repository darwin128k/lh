/**
 * @file draw_log.h
 * @brief Test helper: a canvas backend that records what reaches it.
 *
 * ::lh_test::draw_log keeps every `fill_rect` and `fill_round_rect` (rect,
 * color, radius), every `set_clip` and every `fill_mask` (origin, size) the
 * canvas sends. Eight tables cover the optional slots: with or without
 * `fill_round_rect`, `set_clip` and `fill_mask`.
 */

#ifndef LH_TEST_UI_DRAW_LOG_H
#define LH_TEST_UI_DRAW_LOG_H

#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace lh_test
{

struct draw_log
{
    static const int capacity = 256;

    lh_ui_rect_t fills[capacity];
    lh_ui_color_t fill_colors[capacity];
    int fill_count;

    lh_ui_rect_t round_fills[capacity];
    lh_ui_scalar_t round_radii[capacity];
    int round_count;

    lh_ui_rect_t clip; /* last set_clip rect, when not null */
    int clip_count;    /* set_clip calls with a rect */
    int unclip_count;  /* set_clip calls with null */

    lh_ui_rect_t masks[capacity]; /* each fill_mask: origin and mask size */
    int mask_count;
};

inline lh_void
draw_log_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    draw_log *log = lh_ptr_rcast(draw_log, context);
    if (log->fill_count < draw_log::capacity)
    {
        log->fills[log->fill_count] = *rect;
        log->fill_colors[log->fill_count] = *color;
    }
    ++log->fill_count;
}

inline lh_bool_t
draw_log_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                         const lh_ui_color_t *color)
{
    draw_log *log = lh_ptr_rcast(draw_log, context);
    (void)color;
    if (log->round_count < draw_log::capacity)
    {
        log->round_fills[log->round_count] = *rect;
        log->round_radii[log->round_count] = radius;
    }
    ++log->round_count;
    return lh_bool_true;
}

inline lh_void
draw_log_set_clip(lh_ptr context, const lh_ui_rect_t *clip)
{
    draw_log *log = lh_ptr_rcast(draw_log, context);
    if (clip == nullptr)
    {
        ++log->unclip_count;
        return;
    }
    log->clip = *clip;
    ++log->clip_count;
}

inline lh_bool_t
draw_log_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                   const lh_ui_color_t *color)
{
    draw_log *log = lh_ptr_rcast(draw_log, context);
    (void)color;
    if (log->mask_count < draw_log::capacity)
    {
        log->masks[log->mask_count] = lh_ui_mask_get_rect(mask, *origin);
    }
    ++log->mask_count;
    return lh_bool_true;
}

/* One backend table: fill_rect, plus the optional slots picked by the flags. */
#define LH_TEST_DRAW_LOG_BACKEND(round, clip, mask)                                                 \
    {                                                                                               \
        nullptr, nullptr, nullptr, draw_log_fill_rect, (round) ? draw_log_fill_round_rect : nullptr, \
            (clip) ? draw_log_set_clip : nullptr, (mask) ? draw_log_fill_mask : nullptr             \
    }

/** Backend for @p log: `fill_rect` always, the optional slots when asked. */
inline const lh_ui_canvas_backend_t *
draw_log_backend(bool round, bool clip, bool mask = false)
{
    static const lh_ui_canvas_backend_t tables[8] = {
        LH_TEST_DRAW_LOG_BACKEND(false, false, false), LH_TEST_DRAW_LOG_BACKEND(true, false, false),
        LH_TEST_DRAW_LOG_BACKEND(false, true, false),  LH_TEST_DRAW_LOG_BACKEND(true, true, false),
        LH_TEST_DRAW_LOG_BACKEND(false, false, true),  LH_TEST_DRAW_LOG_BACKEND(true, false, true),
        LH_TEST_DRAW_LOG_BACKEND(false, true, true),   LH_TEST_DRAW_LOG_BACKEND(true, true, true),
    };
    return &tables[(round ? 1 : 0) + (clip ? 2 : 0) + (mask ? 4 : 0)];
}

/** Point @p canvas at an empty @p log through draw_log_backend(@p round, @p clip, @p mask). */
inline lh_void
draw_log_init(draw_log *log, lh_ui_canvas_t *canvas, bool round, bool clip, bool mask = false)
{
    *log = draw_log{};
    lh_ui_canvas_init(canvas, draw_log_backend(round, clip, mask), log);
}

/** A rect literal for comparisons. */
inline lh_ui_rect_t
rect_of(int x, int y, int w, int h)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), x, y, w, h);
    return rect;
}

/** True when @p a and @p b have the same origin and size. */
inline bool
rect_is(const lh_ui_rect_t &a, const lh_ui_rect_t &b)
{
    return lh_ui_rect_eq(lh_addr_of(a), lh_addr_of(b)) != 0;
}

} // namespace lh_test

#endif /* LH_TEST_UI_DRAW_LOG_H */
