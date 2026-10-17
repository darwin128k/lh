/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xset_window_attributes_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XSET_WINDOW_ATTRIBUTES_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XSET_WINDOW_ATTRIBUTES_FIELDS_H

/**
 * @def lh_os_system_posix_xset_window_attributes_fields
 * @brief `XSetWindowAttributes`, all of it: `XCreateWindow` reads the fields its value
 *        mask names, at the offsets Xlib has them, so none may be left out.
 */
#define lh_os_system_posix_xset_window_attributes_fields                                           \
    lh_os_system_posix_xid_t background_pixmap;                                                    \
    unsigned long background_pixel;                                                                \
    lh_os_system_posix_xid_t border_pixmap;                                                        \
    unsigned long border_pixel;                                                                    \
    int bit_gravity;                                                                               \
    int win_gravity;                                                                               \
    int backing_store;                                                                             \
    unsigned long backing_planes;                                                                  \
    unsigned long backing_pixel;                                                                   \
    int save_under;                                                                                \
    long event_mask;                                                                               \
    long do_not_propagate_mask;                                                                    \
    int override_redirect;                                                                         \
    lh_os_system_posix_xid_t colormap;                                                             \
    lh_os_system_posix_xid_t cursor

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XSET_WINDOW_ATTRIBUTES_FIELDS_H */
