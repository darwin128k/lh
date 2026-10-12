/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_paint_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_PAINT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_PAINT_FIELDS_H

/**
 * @def lh_os_system_posix_paint_fields
 * @brief Everything a present needs to put pixels in a window, and the area the
 *        paint was asked for.
 */
#define lh_os_system_posix_paint_fields                                                            \
    const lh_os_system_posix_x11_t *x11;                                                           \
    lh_os_system_posix_display_t *display;                                                         \
    lh_os_system_posix_xid_t window;                                                               \
    lh_os_system_posix_gc_t *gc;                                                                   \
    int depth;                                                                                     \
    lh_u64_t area;                                                                                 \
    lh_u32_t rects

#endif /* LH_SRC_OS_SYSTEM_POSIX_PAINT_FIELDS_H */
