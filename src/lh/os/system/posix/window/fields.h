/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_window_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_FIELDS_H

/**
 * @def lh_os_system_posix_window_fields
 * @brief One X window and what the backend keeps for it that X does not: the
 *        area waiting to be painted, the tick, and whether input is let through.
 */
#define lh_os_system_posix_window_fields                                                           \
    struct lh_os_system_posix_window *next;                                                        \
    struct lh_os_window *user;                                                                     \
    struct lh_os_system_posix_window *owner;                                                       \
    lh_os_system_posix_xid_t window;                                                               \
    lh_os_system_posix_gc_t *gc;                                                                   \
    int width;                                                                                     \
    int height;                                                                                    \
    int corner;                                                                                    \
    int depth;                                                                                     \
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
