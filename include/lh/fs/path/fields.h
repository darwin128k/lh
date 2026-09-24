/**
 * @file fields.h
 * @brief Member fields of ::lh_fs_path_t.
 */

#ifndef LH_FS_PATH_FIELDS_H
#define LH_FS_PATH_FIELDS_H

/**
 * @def lh_fs_path_fields(root_kind_type, root_drive_type, segments_type)
 * @brief Root (kind + drive letter) plus real-name segments.
 *
 * @param root_kind_type  Type of `root_kind` (::lh_fs_path_root_kind_t).
 * @param root_drive_type Type of `root_drive` (::lh_char_t).
 * @param segments_type   Type of `segments` (::lh_str_list_t).
 */
#define lh_fs_path_fields(root_kind_type, root_drive_type, segments_type)                           \
    root_kind_type root_kind;                                                                      \
    root_drive_type root_drive;                                                                    \
    segments_type segments

#endif /* LH_FS_PATH_FIELDS_H */
