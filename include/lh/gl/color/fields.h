/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_color_t.
 */

#ifndef LH_GL_COLOR_FIELDS_H
#define LH_GL_COLOR_FIELDS_H

/**
 * @def lh_gl_color_fields(channel_type)
 * @brief Straight 8-bit channels. Alpha 255 covers the pixel.
 *
 * @param channel_type ::lh_gl_color_channel_t.
 */
#define lh_gl_color_fields(channel_type)                                                           \
    channel_type r;                                                                                \
    channel_type g;                                                                                \
    channel_type b;                                                                                \
    channel_type a

#endif /* LH_GL_COLOR_FIELDS_H */
