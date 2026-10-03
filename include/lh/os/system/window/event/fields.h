/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_window_event_t.
 */

#ifndef LH_OS_SYSTEM_WINDOW_EVENT_FIELDS_H
#define LH_OS_SYSTEM_WINDOW_EVENT_FIELDS_H

/**
 * @def lh_os_system_window_event_fields(type_type, int_type)
 * @brief What happened to a window and where.
 *
 * - `type`: an `lh_os_system_window_event_*` value.
 * - `x`, `y`: pointer position, or the paint area's top-left corner, in
 *   client-area pixels.
 * - `width`, `height`: new client size (resize) or paint area size.
 * - `button`: 0 left, 1 right, 2 middle (pointer down / up).
 *
 * @param type_type Type of `type` (::lh_uint_t).
 * @param int_type  Type of the other fields (::lh_int_t).
 */
#define lh_os_system_window_event_fields(type_type, int_type)                                      \
    type_type type;                                                                                \
    int_type x;                                                                                    \
    int_type y;                                                                                    \
    int_type width;                                                                                \
    int_type height;                                                                               \
    int_type button

#endif /* LH_OS_SYSTEM_WINDOW_EVENT_FIELDS_H */
