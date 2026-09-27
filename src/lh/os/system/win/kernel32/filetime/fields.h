/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_filetime_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILETIME_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILETIME_FIELDS_H

/**
 * @def lh_os_system_win_filetime_fields(dword_type)
 * @brief `FILETIME`: 100-ns ticks since 1601-01-01 UTC, split in two halves.
 *
 * @param dword_type Type of each half (::lh_os_system_win_dword_t).
 */
#define lh_os_system_win_filetime_fields(dword_type)                                               \
    dword_type dwLowDateTime;                                                                      \
    dword_type dwHighDateTime

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILETIME_FIELDS_H */
