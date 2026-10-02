/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_rect_t (`RECT`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_RECT_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_RECT_FIELDS_H

/**
 * @def lh_os_system_win_rect_fields(int_type)
 * @brief `RECT`: left, top, right, bottom (all inclusive-exclusive in lh).
 *
 * @param int_type  signed integer type (`LONG` on Windows; we use ::lh_int_t).
 */
#define lh_os_system_win_rect_fields(int_type)                                                   \
    int_type left;                                                                               \
    int_type top;                                                                                \
    int_type right;                                                                              \
    int_type bottom

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_RECT_FIELDS_H */