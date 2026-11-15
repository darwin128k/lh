/**
 * @file channel.h
 * @brief One color channel: ::lh_gl_color_channel_t.
 */

#ifndef LH_GL_COLOR_CHANNEL_H
#define LH_GL_COLOR_CHANNEL_H

#include <lh/core/byte.h>

/**
 * @typedef lh_gl_color_channel_t
 * @brief One 8-bit color channel (`0..255`).
 *
 * Alias for ::lh_byte_t. Use this name in color APIs; keep ::lh_byte_t for
 * raw buffers and numeric unpack.
 */
typedef lh_byte_t lh_gl_color_channel_t;

#endif /* LH_GL_COLOR_CHANNEL_H */
