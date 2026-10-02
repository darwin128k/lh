/**
 * @file fields.h
 * @brief Member fields of ::lh_color_t.
 */

#ifndef LH_COLOR_FIELDS_H
#define LH_COLOR_FIELDS_H

/**
 * @def lh_color_fields(channel_type)
 * @brief Red, green, blue and alpha channels.
 *
 * @param channel_type Type of each channel (::lh_uchar_t).
 */
#define lh_color_fields(channel_type)                                                              \
    channel_type r;                                                                                \
    channel_type g;                                                                                \
    channel_type b;                                                                                \
    channel_type a

#endif /* LH_COLOR_FIELDS_H */
