/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_systemtime_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_SYSTEMTIME_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_SYSTEMTIME_FIELDS_H

/**
 * @def lh_os_system_win_systemtime_fields(word_type)
 * @brief `SYSTEMTIME`: a calendar date and a time of day, field by field.
 *
 * @param word_type Type of each field (::lh_os_system_win_word_t).
 */
#define lh_os_system_win_systemtime_fields(word_type)                                              \
    word_type wYear;                                                                               \
    word_type wMonth;                                                                              \
    word_type wDayOfWeek;                                                                          \
    word_type wDay;                                                                                \
    word_type wHour;                                                                               \
    word_type wMinute;                                                                             \
    word_type wSecond;                                                                             \
    word_type wMilliseconds

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_SYSTEMTIME_FIELDS_H */
