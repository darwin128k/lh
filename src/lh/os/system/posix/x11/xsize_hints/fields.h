/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xsize_hints_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XSIZE_HINTS_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XSIZE_HINTS_FIELDS_H

/**
 * @def lh_os_system_posix_xsize_hints_fields
 * @brief `XSizeHints` (`WM_NORMAL_HINTS`): where the window asks to be put.
 */
#define lh_os_system_posix_xsize_hints_fields                                                      \
    long flags;                                                                                    \
    int x;                                                                                         \
    int y;                                                                                         \
    int width;                                                                                     \
    int height;                                                                                    \
    int min_width;                                                                                 \
    int min_height;                                                                                \
    int max_width;                                                                                 \
    int max_height;                                                                                \
    int width_inc;                                                                                 \
    int height_inc;                                                                                \
    int min_aspect_x;                                                                              \
    int min_aspect_y;                                                                              \
    int max_aspect_x;                                                                              \
    int max_aspect_y;                                                                              \
    int base_width;                                                                                \
    int base_height;                                                                               \
    int win_gravity

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XSIZE_HINTS_FIELDS_H */
