/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_file_attribute_data_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILE_ATTRIBUTE_DATA_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILE_ATTRIBUTE_DATA_FIELDS_H

/**
 * @def lh_os_system_win_file_attribute_data_fields(dword_type, filetime_type)
 * @brief `WIN32_FILE_ATTRIBUTE_DATA` (`GetFileExInfoStandard`).
 *
 * @param dword_type    ::lh_os_system_win_dword_t.
 * @param filetime_type ::lh_os_system_win_filetime_t.
 */
#define lh_os_system_win_file_attribute_data_fields(dword_type, filetime_type)                     \
    dword_type dwFileAttributes;                                                                   \
    filetime_type ftCreationTime;                                                                  \
    filetime_type ftLastAccessTime;                                                                \
    filetime_type ftLastWriteTime;                                                                 \
    dword_type nFileSizeHigh;                                                                      \
    dword_type nFileSizeLow

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_FILE_ATTRIBUTE_DATA_FIELDS_H */
