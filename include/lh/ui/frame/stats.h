/**
 * @file stats.h
 * @brief Running totals over the frames a window recorded: ::lh_ui_frame_stats_t.
 *
 * **A frame is a rectangle and a request is a region.** A window collects what was
 * invalidated exactly -- a status line at the bottom and a row at the top are two small
 * rectangles -- and hands the paint the **hull** of them, which is what a renderer that
 * draws one rectangle clears, draws and presents. How much bigger the one is than the
 * other is the cost of drawing rectangles, and the number of rectangles the request was
 * made of is what a damage *list* would buy back. The struct the renderer hands back
 * is a single ::lh_ui_frame_t; this is the running aggregate that turns it into a
 * measurement, kept here.
 *
 * The instrument that tells a renderer that converges from one that does not:
 * a paint that asks for the next paint shows up here as thousands of frames a second of
 * the same rectangle, which is how a window that "works" turned out to have asked its
 * device nothing for eight seconds.
 *
 * Pure bookkeeping: nothing here talks to a platform. A window fills it in
 * (::lh_os_window_set_frame_stats), and so can a strip renderer on a device with no
 * window at all. What to do with each frame -- print it, log it, keep the worst -- is the
 * caller's, through ::lh_ui_frame_stats_set_on_frame.
 */

#ifndef LH_UI_FRAME_STATS_H
#define LH_UI_FRAME_STATS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/ui/frame.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_ui_frame_stats_on_frame_cb
 * @brief Called with every frame ::lh_ui_frame_stats_record is given, after it is counted.
 */
typedef lh_void (*lh_ui_frame_stats_on_frame_cb)(const lh_ui_frame_t *frame, lh_ptr context);

/**
 * @struct lh_ui_frame_stats
 * @brief Running totals over frames. Read through the getters.
 */
typedef struct lh_ui_frame_stats
{
    lh_u32_t frames;   /**< frames recorded, whether their request was known or not */
    lh_u32_t unknown;  /**< of those, frames whose request could not be read */
    lh_u32_t wasteful; /**< frames that drew at least twice what they were asked for */
    lh_u64_t drawn_px; /**< drawn pixels, over frames whose request was known */
    lh_u64_t asked_px; /**< asked pixels, over the same frames */
    lh_u64_t rects;    /**< rectangles in the requests, over the same frames */
    lh_u64_t us;       /**< time, over every frame */
    lh_u64_t worst_us; /**< the longest single frame */
    /** called after each frame is counted; may be ::lh_null */
    lh_ui_frame_stats_on_frame_cb on_frame;
    lh_ptr on_frame_context; /**< passed to ::on_frame */
} lh_ui_frame_stats_t; /**< Running totals over frames. */

/** @brief Zero every total and drop the callback. */
lh_void
lh_ui_frame_stats_init(lh_ui_frame_stats_t *self);

/** @brief Call @p cb with every frame from now on; ::lh_null stops it. */
lh_void
lh_ui_frame_stats_set_on_frame(lh_ui_frame_stats_t *self, lh_ui_frame_stats_on_frame_cb cb, lh_ptr context);

/** @brief Count @p frame, then hand it to the callback. */
lh_void
lh_ui_frame_stats_record(lh_ui_frame_stats_t *self, const lh_ui_frame_t *frame);

/** @brief Frames recorded, including those whose request was unknown. */
lh_u32_t
lh_ui_frame_stats_get_frames(const lh_ui_frame_stats_t *self);

/** @brief Frames whose request could not be read. */
lh_u32_t
lh_ui_frame_stats_get_unknown(const lh_ui_frame_stats_t *self);

/** @brief Frames that drew at least twice the pixels they were asked for. */
lh_u32_t
lh_ui_frame_stats_get_wasteful(const lh_ui_frame_stats_t *self);

/** @brief Pixels drawn, over the frames whose request was known. */
lh_u64_t
lh_ui_frame_stats_get_drawn_px(const lh_ui_frame_stats_t *self);

/** @brief Pixels asked for, over the same frames. */
lh_u64_t
lh_ui_frame_stats_get_asked_px(const lh_ui_frame_stats_t *self);

/** @brief Rectangles the requests were made of, over the same frames. */
lh_u64_t
lh_ui_frame_stats_get_rects(const lh_ui_frame_stats_t *self);

/** @brief Time spent in every frame, in microseconds. */
lh_u64_t
lh_ui_frame_stats_get_us(const lh_ui_frame_stats_t *self);

/** @brief The longest single frame, in microseconds. */
lh_u64_t
lh_ui_frame_stats_get_worst_us(const lh_ui_frame_stats_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FRAME_STATS_H */
