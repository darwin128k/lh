/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_path_span_t.
 */

#ifndef LH_OS_FS_PATH_SPAN_FIELDS_H
#define LH_OS_FS_PATH_SPAN_FIELDS_H

/**
 * @def lh_os_fs_path_span_fields(offset_type, size_type)
 * @brief Offset and length of one name inside the path buffer.
 *
 * @param offset_type Type of `offset` (::lh_usize_t).
 * @param size_type   Type of `size` (::lh_usize_t).
 */
#define lh_os_fs_path_span_fields(offset_type, size_type)                                          \
    offset_type offset;                                                                            \
    size_type size

#endif /* LH_OS_FS_PATH_SPAN_FIELDS_H */
