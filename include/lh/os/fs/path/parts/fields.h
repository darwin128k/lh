/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_path_parts_t.
 */

#ifndef LH_OS_FS_PATH_PARTS_FIELDS_H
#define LH_OS_FS_PATH_PARTS_FIELDS_H

/**
 * @def lh_os_fs_path_parts_fields(vector_type)
 * @brief The offset table (::lh_vector_t of ::lh_os_fs_path_span_t).
 *
 * @param vector_type Type of the table (::lh_vector_t).
 */
#define lh_os_fs_path_parts_fields(vector_type) vector_type items

#endif /* LH_OS_FS_PATH_PARTS_FIELDS_H */
