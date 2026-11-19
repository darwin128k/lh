/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_gradient_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_FIELDS_H
#define LH_CORE_PIXEL_GRADIENT_FIELDS_H

#include <lh/config.h>
#include <lh/core/assert/static.h>

lh_assert_static(LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS >= 1 &&
                     LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS <= 255,
                 "LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS must be in 1..255");

/**
 * @def lh_pixel_gradient_fields(stop_type, count_type, shape_type, angle_type)
 * @brief The stops of a gradient and how they lie over a box.
 *
 * `stops` holds up to ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS stops by value,
 * of which the first `stops_count` are used, in order of their position;
 * `shape` says whether they lie along a line or out from the middle, and
 * `angle` is the line's direction in degrees, `0 .. 359` (0 up, 90 right).
 *
 * @param stop_type  Type of a stop (::lh_pixel_gradient_stop_t).
 * @param count_type Type of the count (::lh_pixel_gradient_stop_count_t).
 * @param shape_type Type of the shape (::lh_pixel_gradient_shape_t).
 * @param angle_type Type of the angle (::lh_s16_t).
 */
#define lh_pixel_gradient_fields(stop_type, count_type, shape_type, angle_type)                    \
    stop_type stops[LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS];                                      \
    count_type stops_count;                                                                        \
    shape_type shape;                                                                              \
    angle_type angle

#endif /* LH_CORE_PIXEL_GRADIENT_FIELDS_H */
