/**
 * @file channel.h
 * @brief One color channel: ::lh_ui_color_channel_t.
 */

#ifndef LH_UI_COLOR_CHANNEL_H
#define LH_UI_COLOR_CHANNEL_H

#include <lh/byte.h>

/**
 * @typedef lh_ui_color_channel_t
 * @brief One 8-bit color channel (`0..255`).
 *
 * Alias for ::lh_byte_t. Use this name in color APIs; keep ::lh_byte_t for
 * raw buffers and numeric unpack.
 */
typedef lh_byte_t lh_ui_color_channel_t;

#endif /* LH_UI_COLOR_CHANNEL_H */
