/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_color_t.
 */

#ifndef LH_UI_COLOR_FIELDS_H
#define LH_UI_COLOR_FIELDS_H

/**
 * @def lh_ui_color_fields(channel_type)
 * @brief Red, green, blue and alpha channels.
 *
 * @param channel_type Type of each channel.
 */
#define lh_ui_color_fields(channel_type)                                                            \
    channel_type r;                                                                                 \
    channel_type g;                                                                                 \
    channel_type b;                                                                                 \
    channel_type a

#endif /* LH_UI_COLOR_FIELDS_H */
