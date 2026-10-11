/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xrender_color_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_COLOR_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_COLOR_FIELDS_H

/**
 * @def lh_os_system_posix_xrender_color_fields
 * @brief `XRenderColor`: 16 bits per channel, alpha premultiplied.
 */
#define lh_os_system_posix_xrender_color_fields \
    unsigned short red;                                                                            \
    unsigned short green;                                                                          \
    unsigned short blue;                                                                           \
    unsigned short alpha

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_COLOR_FIELDS_H */
