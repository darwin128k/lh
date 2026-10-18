/**
 * @file fields.h
 * @brief Member fields of ::lh_str_list_span_t.
 */

#ifndef LH_STR_LIST_SPAN_FIELDS_H
#define LH_STR_LIST_SPAN_FIELDS_H

/**
 * @def lh_str_list_span_fields(size_type)
 * @brief Where one element of a string list sits in its shared buffer.
 *
 * - `offset`: where the element starts, in characters.
 * - `size`: its length in characters, without the NUL that follows it.
 *
 * @param size_type Type of both fields (::lh_usize_t).
 */
#define lh_str_list_span_fields(size_type)                                                         \
    size_type offset;                                                                              \
    size_type size

#endif /* LH_STR_LIST_SPAN_FIELDS_H */
