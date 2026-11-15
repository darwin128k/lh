/**
 * @file count.h
 * @brief How many stops a gradient uses: ::lh_gl_gradient_stop_count_t.
 */

#ifndef LH_GL_GRADIENT_STOP_COUNT_H
#define LH_GL_GRADIENT_STOP_COUNT_H

#include <lh/core/byte.h>

/**
 * @typedef lh_gl_gradient_stop_count_t
 * @brief Number of used stops (`0` … ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS).
 *
 * Alias for ::lh_byte_t. Use this name for stop counts and indices into the
 * stop table; keep ::lh_byte_t for raw bytes and for ::lh_gl_gradient_stop_t
 * frac positions.
 */
typedef lh_byte_t lh_gl_gradient_stop_count_t;

#endif /* LH_GL_GRADIENT_STOP_COUNT_H */
