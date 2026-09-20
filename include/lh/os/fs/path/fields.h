/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_path_t.
 */

#ifndef LH_OS_FS_PATH_FIELDS_H
#define LH_OS_FS_PATH_FIELDS_H

/**
 * @def LH_OS_FS_PATH_MAX
 * @brief Longest path buffer, including NUL.
 */
#define LH_OS_FS_PATH_MAX 4096U

/**
 * @def lh_os_fs_path_fields(sep_type, text_type, parts_type)
 * @brief Injects the separator, the path bytes, and name-offset table.
 *
 * @param sep_type   Type of the separator (::lh_char_t).
 * @param text_type  Type of the path buffer (::lh_str_t).
 * @param parts_type Type of the offset table (::lh_os_fs_path_parts_t).
 */
#define lh_os_fs_path_fields(sep_type, text_type, parts_type)                                      \
    sep_type sep;                                                                                  \
    text_type text;                                                                                \
    parts_type parts

#endif /* LH_OS_FS_PATH_FIELDS_H */
