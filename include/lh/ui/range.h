/**
 * @file range.h
 * @brief A window on a range, drawn on a track: the geometry a scrollbar
 *        thumb, a slider handle or a progress bar share.
 *
 * `content` units of which `view` are seen at `offset` (`0 .. content - view`),
 * shown on a track `track` long. The window is the part of the track that
 * stands for what is seen.
 */

#ifndef LH_UI_RANGE_H
#define LH_UI_RANGE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/scalar.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Window length: `track * view / content`, at least @p min (capped by
 *        @p track); the whole @p track when @p view covers @p content.
 */
lh_ui_scalar_t
lh_ui_range_window_length(lh_ui_scalar_t track, lh_ui_scalar_t view, lh_ui_scalar_t content,
                          lh_ui_scalar_t min);

/**
 * @brief Window start past the track origin: `(track - length) * offset / max`;
 *        `0` when @p max is not positive.
 */
lh_ui_scalar_t
lh_ui_range_window_start(lh_ui_scalar_t track, lh_ui_scalar_t length, lh_ui_scalar_t offset,
                         lh_ui_scalar_t max);

/**
 * @brief Offset from a window start: the inverse of
 *        ::lh_ui_range_window_start. @p start is clamped to
 *        `0 .. track - length`. Returns `0` when there is no travel
 *        (`track <= length` or @p max is not positive).
 */
lh_ui_scalar_t
lh_ui_range_offset_from_window_start(lh_ui_scalar_t track, lh_ui_scalar_t length,
                                     lh_ui_scalar_t start, lh_ui_scalar_t max);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RANGE_H */
