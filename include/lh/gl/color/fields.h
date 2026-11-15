/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_color_t.
 */

#ifndef LH_GL_COLOR_FIELDS_H
#define LH_GL_COLOR_FIELDS_H

/**
 * @def lh_gl_color_fields(channel_type)
 * @brief Red, green, blue and alpha channels.
 *
 * @param channel_type Type of each channel.
 */
#define lh_gl_color_fields(channel_type)                                                           \
    channel_type r;                                                                                \
    channel_type g;                                                                                \
    channel_type b;                                                                                \
    channel_type a

#endif /* LH_GL_COLOR_FIELDS_H */
