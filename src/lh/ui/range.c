/**
 * @file range.c
 * @brief Implementation of `lh/ui/range.h`.
 */

#include <lh/math.h>
#include <lh/ui/range.h>
#include <lh/util/return.h>

lh_ui_scalar_t
lh_ui_range_window_length(lh_ui_scalar_t track, lh_ui_scalar_t view, lh_ui_scalar_t content,
                          lh_ui_scalar_t min)
{
    lh_ui_scalar_t length;

    lh_return_if(content <= view, track);
    /* content > view >= 0, so content is not 0. */
    length = track * view / content;
    return lh_math_max(length, lh_math_min(min, track));
}

lh_ui_scalar_t
lh_ui_range_window_start(lh_ui_scalar_t track, lh_ui_scalar_t length, lh_ui_scalar_t offset,
                         lh_ui_scalar_t max)
{
    lh_return_if(max <= lh_ui_scalar(0), lh_ui_scalar(0));
    return (track - length) * offset / max;
}

lh_ui_scalar_t
lh_ui_range_offset_from_window_start(lh_ui_scalar_t track, lh_ui_scalar_t length,
                                     lh_ui_scalar_t start, lh_ui_scalar_t max)
{
    lh_ui_scalar_t travel;

    travel = track - length;
    lh_return_if(travel <= lh_ui_scalar(0) || max <= lh_ui_scalar(0), lh_ui_scalar(0));
    start = lh_math_clamp(start, lh_ui_scalar(0), travel);
    return start * max / travel;
}
