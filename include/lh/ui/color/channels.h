/**
 * @file channels.h
 * @brief Fixed RGBA channel pack: ::lh_ui_color_channels_t.
 */

#ifndef LH_UI_COLOR_CHANNELS_H
#define LH_UI_COLOR_CHANNELS_H

#include <lh/ui/color/channel.h>

/**
 * @def LH_UI_COLOR_CHANNELS_SIZE
 * @brief Number of channels in ::lh_ui_color_channels_t (RGBA).
 */
#define LH_UI_COLOR_CHANNELS_SIZE 4u

/**
 * @typedef lh_ui_color_channels_t
 * @brief Four ::lh_ui_color_channel_t values in order `r, g, b, a`.
 */
typedef lh_ui_color_channel_t lh_ui_color_channels_t[LH_UI_COLOR_CHANNELS_SIZE];

#endif /* LH_UI_COLOR_CHANNELS_H */
