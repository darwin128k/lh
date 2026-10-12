/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_ximage_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XIMAGE_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XIMAGE_FIELDS_H

/**
 * @def lh_os_system_posix_ximage_fields
 * @brief `XImage`, filled in by us and completed by `XInitImage`: a description of client
 *        memory that `XPutImage` copies to a window. `f` is the six function pointers
 *        `XInitImage` fills in; nothing here calls them.
 */
#define lh_os_system_posix_ximage_fields \
    int width;                                                                                     \
    int height;                                                                                    \
    int xoffset;                                                                                   \
    int format;                                                                                    \
    char *data;                                                                                    \
    int byte_order;                                                                                \
    int bitmap_unit;                                                                               \
    int bitmap_bit_order;                                                                          \
    int bitmap_pad;                                                                                \
    int depth;                                                                                     \
    int bytes_per_line;                                                                            \
    int bits_per_pixel;                                                                            \
    unsigned long red_mask;                                                                        \
    unsigned long green_mask;                                                                      \
    unsigned long blue_mask;                                                                       \
    char *obdata;                                                                                  \
    void *f[6]

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XIMAGE_FIELDS_H */
