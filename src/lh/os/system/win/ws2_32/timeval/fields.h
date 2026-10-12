/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_timeval_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_TIMEVAL_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_TIMEVAL_FIELDS_H

/**
 * @def lh_os_system_win_timeval_fields(long_type)
 * @brief `TIMEVAL`: seconds and microseconds, both 32-bit `long` on Windows.
 *
 * @param long_type `long` (::lh_long_t).
 */
#define lh_os_system_win_timeval_fields(long_type)                                                 \
    long_type tv_sec;                                                                              \
    long_type tv_usec

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_TIMEVAL_FIELDS_H */
