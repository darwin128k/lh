/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_find_data_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_FIND_DATA_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_FIND_DATA_FIELDS_H

#include <lh/os/system/win/types.h>

/**
 * @def lh_os_system_win_find_data_fields(dword_type, filetime_type, char_type)
 * @brief `WIN32_FIND_DATAA`. `dwReserved0` holds the reparse tag when
 *        `dwFileAttributes` has `FILE_ATTRIBUTE_REPARSE_POINT`.
 *
 * @param dword_type    ::lh_os_system_win_dword_t.
 * @param filetime_type ::lh_os_system_win_filetime_t.
 * @param char_type     Narrow character type (::lh_char_t).
 */
#define lh_os_system_win_find_data_fields(dword_type, filetime_type, char_type)                    \
    dword_type dwFileAttributes;                                                                   \
    filetime_type ftCreationTime;                                                                  \
    filetime_type ftLastAccessTime;                                                                \
    filetime_type ftLastWriteTime;                                                                 \
    dword_type nFileSizeHigh;                                                                      \
    dword_type nFileSizeLow;                                                                       \
    dword_type dwReserved0;                                                                        \
    dword_type dwReserved1;                                                                        \
    char_type cFileName[LH_OS_SYSTEM_WIN_MAX_PATH];                                                \
    char_type cAlternateFileName[14]

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_FIND_DATA_FIELDS_H */
