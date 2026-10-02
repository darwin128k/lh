/**
 * @file fields.h
 * @brief Member fields of ::lh_mat4_t.
 */

#ifndef LH_MAT4_FIELDS_H
#define LH_MAT4_FIELDS_H

/**
 * @def lh_mat4_fields(column_type)
 * @brief The 4 columns, left to right.
 *
 * `columns[c].x .. .w` are rows 0..3 of column `c`; for an affine transform
 * `columns[0..2]` are the transformed axes and `columns[3]` the translation.
 *
 * @param column_type Type of each column (::lh_vec4_t).
 */
#define lh_mat4_fields(column_type) column_type columns[4]

#endif /* LH_MAT4_FIELDS_H */
