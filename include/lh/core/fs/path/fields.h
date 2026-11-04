/**
 * @file fields.h
 * @brief Member fields of ::lh_fs_path_t.
 */

#ifndef LH_FS_PATH_FIELDS_H
#define LH_FS_PATH_FIELDS_H

/**
 * @def lh_fs_path_fields(text_type, root_kind_type)
 * @brief The whole normalized path as one text, plus what it is rooted at.
 *
 * `text` holds the root prefix (`/` or `C:/`, per `root_kind`) followed by
 * the segments joined by a single `/` — no repeated or trailing
 * separator. Segments are slices of it, not strings of their own.
 *
 * @param text_type      Type of `text` (::lh_str_t).
 * @param root_kind_type Type of `root_kind` (::lh_fs_path_root_kind_t).
 */
#define lh_fs_path_fields(text_type, root_kind_type)                                               \
    text_type text;                                                                                \
    root_kind_type root_kind

#endif /* LH_FS_PATH_FIELDS_H */
