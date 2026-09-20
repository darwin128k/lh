/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_path_t.
 */

#ifndef LH_OS_FS_PATH_FIELDS_H
#define LH_OS_FS_PATH_FIELDS_H

/**
 * @def lh_os_fs_path_fields(root_kind_type, root_drive_type, segments_type)
 * @brief Root (kind + drive letter) plus real-name segments.
 *
 * @param root_kind_type  Type of `root_kind` (::lh_os_fs_path_root_kind_t).
 * @param root_drive_type Type of `root_drive` (::lh_char_t).
 * @param segments_type   Type of `segments` (::lh_str_list_t).
 */
#define lh_os_fs_path_fields(root_kind_type, root_drive_type, segments_type)                       \
    root_kind_type root_kind;                                                                      \
    root_drive_type root_drive;                                                                    \
    segments_type segments

#endif /* LH_OS_FS_PATH_FIELDS_H */
