/**
 * @file frame.h
 * @brief One frame — what a paint cycle drew against what the platform asked for:
 *        ::lh_ui_frame_t.
 *
 * A frame is the moment the window system (or whatever wakes the paint) and the
 * painter met. One side knew what was actually stale — a list of dirty
 * rectangles, possibly summary unknown — the other knew what it was about to
 * clear and present. A single struct carries both, plus the timing, so the
 * stats can read both at once and tell a caller their ratio.
 *
 * The aggregator over many frames lives one level up —
 * ::lh_ui_frame_stats_t in ::lh_ui_frame_stats.h. This is the value type: no
 * allocations, no callbacks, no behaviour.
 */

#ifndef LH_UI_FRAME_H
#define LH_UI_FRAME_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/ui/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_ui_frame
 * @brief One frame: what was drawn, what was asked for, and how long it took.
 */
typedef struct lh_ui_frame
{
    lh_ui_rect_t drawn;    /**< the rectangle the frame cleared, drew and presented */
    lh_bool_t asked_known; /**< false when the platform could not say what was asked */
    lh_u64_t asked_px;     /**< pixels of the request; meaningless unless @ref asked_known */
    lh_u32_t rects;        /**< rectangles the request is made of; ditto */
    lh_u64_t us;           /**< time spent drawing and presenting it */
} lh_ui_frame_t; /**< One frame: what was drawn, what was asked for, and how long it took. */

/**
 * @brief The pixel count of @p rect, 0 for an empty one.
 *
 * A helper that lives with the frame because the only thing a frame draws is
 * its `drawn` rectangle — and the only thing the stats aggregate is its area
 * against the request's. Self-contained, no platform talk.
 */
lh_u64_t
lh_ui_frame_area(const lh_ui_rect_t *rect);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FRAME_H */
