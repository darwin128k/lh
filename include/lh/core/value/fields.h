/**
 * @file fields.h
 * @brief Member fields of ::lh_value_t.
 */

#ifndef LH_VALUE_FIELDS_H
#define LH_VALUE_FIELDS_H

/**
 * @def lh_value_fields(kind_type, data_type)
 * @brief What is held, and the storage for it.
 *
 * @param kind_type Type of `kind` (::lh_value_kind_t).
 * @param data_type Type of `data` (::lh_value_data_t).
 */
#define lh_value_fields(kind_type, data_type)                                                      \
    kind_type kind;                                                                                \
    data_type data

#endif /* LH_VALUE_FIELDS_H */
