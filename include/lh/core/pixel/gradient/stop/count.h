/**
 * @file count.h
 * @brief How many stops a gradient uses: ::lh_pixel_gradient_stop_count_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_STOP_COUNT_H
#define LH_CORE_PIXEL_GRADIENT_STOP_COUNT_H

#include <lh/core/byte.h>

/**
 * @typedef lh_pixel_gradient_stop_count_t
 * @brief A number of stops, or an index into a gradient's stops
 *        (`0` … ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS).
 *
 * A byte: a gradient has at most 255 stops. The position of a stop along the
 * gradient is a byte too, but a different one (::lh_pixel_gradient_stop_get_frac).
 */
typedef lh_byte_t lh_pixel_gradient_stop_count_t;

#endif /* LH_CORE_PIXEL_GRADIENT_STOP_COUNT_H */
