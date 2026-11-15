/**
 * @file range.c
 * @brief Implementation of `lh/gl/range.h`.
 */

#include <lh/core/math.h>
#include <lh/gl/range.h>
#include <lh/core/util/return.h>

lh_gl_scalar_t
lh_gl_range_window_length(lh_gl_scalar_t track, lh_gl_scalar_t view, lh_gl_scalar_t content,
                          lh_gl_scalar_t min)
{
    lh_return_if(content <= view, track);
    /* content > view >= 0, so content is not 0. */
    lh_gl_scalar_t length = track * view / content;
    return lh_math_max(length, lh_math_min(min, track));
}

lh_gl_scalar_t
lh_gl_range_window_start(lh_gl_scalar_t track, lh_gl_scalar_t length, lh_gl_scalar_t offset,
                         lh_gl_scalar_t max)
{
    lh_return_if(max <= lh_gl_scalar(0), lh_gl_scalar(0));
    return (track - length) * offset / max;
}

lh_gl_scalar_t
lh_gl_range_offset_from_window_start(lh_gl_scalar_t track, lh_gl_scalar_t length,
                                     lh_gl_scalar_t start, lh_gl_scalar_t max)
{
    lh_gl_scalar_t travel = track - length;
    lh_return_if(travel <= lh_gl_scalar(0) || max <= lh_gl_scalar(0), lh_gl_scalar(0));
    start = lh_math_clamp(start, lh_gl_scalar(0), travel);
    return start * max / travel;
}
