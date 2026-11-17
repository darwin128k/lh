/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_os_window_backend_t.
 */

#ifndef LH_OS_WINDOW_BACKEND_FIELDS_H
#define LH_OS_WINDOW_BACKEND_FIELDS_H

/**
 * @def lh_os_window_backend_fields(open_fn, close_fn, invalidate_fn, invalidate_rect_fn, now_fn,
 * set_timer_fn, set_enabled_fn, minimize_fn, set_maximized_fn, set_corner_radius_fn,
 * get_client_size_fn, get_position_fn, get_paint_region_fn, now_us_fn)
 * @brief The slots of the contract: one pointer per call.
 *
 * Each parameter is a function type; the members are pointers to it. The
 * backend's context is not a field: whoever holds the table holds the context
 * beside it and passes it into every call.
 *
 * @param open_fn ::lh_os_window_backend_open_fn.
 * @param close_fn ::lh_os_window_backend_close_fn.
 * @param invalidate_fn ::lh_os_window_backend_invalidate_fn.
 * @param invalidate_rect_fn ::lh_os_window_backend_invalidate_rect_fn.
 * @param now_fn ::lh_timer_clock_fn.
 * @param set_timer_fn ::lh_os_window_backend_set_timer_fn.
 * @param set_enabled_fn ::lh_os_window_backend_set_enabled_fn.
 * @param minimize_fn ::lh_os_window_backend_minimize_fn.
 * @param set_maximized_fn ::lh_os_window_backend_set_maximized_fn.
 * @param set_corner_radius_fn ::lh_os_window_backend_set_corner_radius_fn.
 * @param get_client_size_fn ::lh_os_window_backend_get_client_size_fn.
 * @param get_position_fn ::lh_os_window_backend_get_position_fn.
 * @param get_paint_region_fn ::lh_os_window_backend_get_paint_region_fn.
 * @param now_us_fn ::lh_os_window_backend_now_us_fn.
 */
#define lh_os_window_backend_fields(open_fn, close_fn, invalidate_fn, invalidate_rect_fn, now_fn,  \
                                    set_timer_fn, set_enabled_fn, minimize_fn, set_maximized_fn,   \
                                    set_corner_radius_fn, get_client_size_fn, get_position_fn,     \
                                    get_paint_region_fn, now_us_fn)                                \
    open_fn *open;                                                                                 \
    close_fn *close;                                                                               \
    invalidate_fn *invalidate;                                                                     \
    invalidate_rect_fn *invalidate_rect;                                                           \
    now_fn *now;                                                                                   \
    set_timer_fn *set_timer;                                                                       \
    set_enabled_fn *set_enabled;                                                                   \
    minimize_fn *minimize;                                                                         \
    set_maximized_fn *set_maximized;                                                               \
    set_corner_radius_fn *set_corner_radius;                                                       \
    get_client_size_fn *get_client_size;                                                           \
    get_position_fn *get_position;                                                                 \
    get_paint_region_fn *get_paint_region;                                                         \
    now_us_fn *now_us

#endif /* LH_OS_WINDOW_BACKEND_FIELDS_H */
