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

#include <lh/ui/rect.h>

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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The pixel count of @p rect, 0 for an empty one.
 *
 * A helper that lives with the frame because the only thing a frame draws is
 * its `drawn` rectangle — and the only thing the stats aggregate is its area
 * against the request's. Self-contained, no platform talk.
 */
lh_u64_t
lh_ui_frame_area(const lh_ui_rect_t *rect);

/**
 * @brief The rectangle the frame drew (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_frame_get_drawn(const lh_ui_frame_t *self);

/**
 * @brief The rectangle the frame drew, to change in place (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_frame_get_drawn_mut(lh_ui_frame_t *self);

/**
 * @brief Store the rectangle the frame drew (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param drawn The new value.
 */
lh_void
lh_ui_frame_set_drawn(lh_ui_frame_t *self, lh_ui_rect_t drawn);

/**
 * @brief Whether the area the system asked to be drawn is known (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_frame_is_asked_known(const lh_ui_frame_t *self);

/**
 * @brief Store whether the area the system asked to be drawn is known (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param asked_known The new value.
 */
lh_void
lh_ui_frame_set_asked_known(lh_ui_frame_t *self, lh_bool_t asked_known);

/**
 * @brief How many pixels the system asked to be drawn (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_ui_frame_get_asked_px(const lh_ui_frame_t *self);

/**
 * @brief Store how many pixels the system asked to be drawn (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param asked_px The new value.
 */
lh_void
lh_ui_frame_set_asked_px(lh_ui_frame_t *self, lh_u64_t asked_px);

/**
 * @brief How many rectangles the frame drew (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_ui_frame_get_rects(const lh_ui_frame_t *self);

/**
 * @brief Store how many rectangles the frame drew (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param rects The new value.
 */
lh_void
lh_ui_frame_set_rects(lh_ui_frame_t *self, lh_u32_t rects);

/**
 * @brief How many microseconds the frame took (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_ui_frame_get_us(const lh_ui_frame_t *self);

/**
 * @brief Store how many microseconds the frame took (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param us The new value.
 */
lh_void
lh_ui_frame_set_us(lh_ui_frame_t *self, lh_u64_t us);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FRAME_H */
