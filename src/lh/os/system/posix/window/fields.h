/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_window_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_FIELDS_H

/**
 * @def lh_os_system_posix_window_fields
 * @brief One window of ours and what the backend keeps for it that X does not:
 *        the area waiting to be painted, the tick, and whether input is let through.
 *
 * Where the server has a 32-bit visual and RENDER, `window` is 32 bits deep and
 * nothing draws into it directly: a paint goes into `backing`, a pixmap on the
 * screen's own 24-bit visual, and is composited from there into the window
 * (`backing_picture` onto `window_picture`). A 24-bit source has no alpha, so
 * whatever the top byte of the app's pixels says, the window comes out opaque --
 * the top byte is ignored as it is on Windows -- and the corners outside a round
 * cut are then cleared to transparent. Without them `backing` is none and the
 * window is drawn into as it is.
 */
#define lh_os_system_posix_window_fields                                                           \
    struct lh_os_system_posix_window *next;                                                        \
    struct lh_os_window *user;                                                                     \
    struct lh_os_system_posix_window *owner;                                                       \
    lh_os_system_posix_xid_t window;                                                               \
    lh_os_system_posix_xid_t backing;                                                              \
    lh_os_system_posix_xid_t backing_picture;                                                      \
    lh_os_system_posix_xid_t window_picture;                                                       \
    lh_os_system_posix_gc_t *gc;                                                                   \
    int width;                                                                                     \
    int height;                                                                                    \
    int corner;                                                                                    \
    lh_os_system_posix_xid_t colormap;                                                             \
    lh_bool_t enabled;                                                                             \
    lh_bool_t maximized;                                                                           \
    lh_bool_t dirty;                                                                               \
    int dirty_left;                                                                                \
    int dirty_top;                                                                                 \
    int dirty_right;                                                                               \
    int dirty_bottom;                                                                              \
    lh_u32_t tick_ms;                                                                              \
    lh_u64_t tick_due_us

#endif /* LH_SRC_OS_SYSTEM_POSIX_WINDOW_FIELDS_H */
