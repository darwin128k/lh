/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xvisual_info_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XVISUAL_INFO_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XVISUAL_INFO_FIELDS_H

/**
 * @def lh_os_system_posix_xvisual_info_fields
 * @brief `XVisualInfo`: a visual, and what its pixels are made of.
 */
#define lh_os_system_posix_xvisual_info_fields                                                     \
    lh_os_system_posix_visual_t *visual;                                                           \
    unsigned long visualid;                                                                        \
    int screen;                                                                                    \
    int depth;                                                                                     \
    int klass;                                                                                     \
    unsigned long red_mask;                                                                        \
    unsigned long green_mask;                                                                      \
    unsigned long blue_mask;                                                                       \
    int colormap_size;                                                                             \
    int bits_per_rgb

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XVISUAL_INFO_FIELDS_H */
