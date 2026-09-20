/**
 * @file fields.h
 * @brief Member fields of ::lh_str_list_t.
 */

#ifndef LH_STR_LIST_FIELDS_H
#define LH_STR_LIST_FIELDS_H

/**
 * @def lh_str_list_fields(vector_type)
 * @brief The backing storage (::lh_vector_t of ::lh_str_t).
 *
 * @param vector_type Type of the backing storage (::lh_vector_t).
 */
#define lh_str_list_fields(vector_type) vector_type items

#endif /* LH_STR_LIST_FIELDS_H */
