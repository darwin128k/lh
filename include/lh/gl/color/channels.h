/**
 * @file channels.h
 * @brief Fixed RGBA channel pack: ::lh_gl_color_channels_t.
 */

#ifndef LH_GL_COLOR_CHANNELS_H
#define LH_GL_COLOR_CHANNELS_H

#include <lh/gl/color/channel.h>

/**
 * @def LH_GL_COLOR_CHANNELS_SIZE
 * @brief Number of channels in ::lh_gl_color_channels_t (RGBA).
 */
#define LH_GL_COLOR_CHANNELS_SIZE 4u

/**
 * @typedef lh_gl_color_channels_t
 * @brief Four ::lh_gl_color_channel_t values in order `r, g, b, a`.
 */
typedef lh_gl_color_channel_t lh_gl_color_channels_t[LH_GL_COLOR_CHANNELS_SIZE];

#endif /* LH_GL_COLOR_CHANNELS_H */
