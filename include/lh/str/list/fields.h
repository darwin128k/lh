/**
 * @file fields.h
 * @brief Member fields of ::lh_str_list_t (and ::lh_wstr_list_t).
 */

#ifndef LH_STR_LIST_FIELDS_H
#define LH_STR_LIST_FIELDS_H

/**
 * @def lh_str_list_fields(chars_type, spans_type)
 * @brief One character buffer shared by every element, plus where each
 *        element sits in it.
 *
 * `chars` holds the elements back to back, each followed by a NUL; `spans`
 * holds one (offset, size) pair per element. Offsets, not pointers, so the
 * buffer may move when it grows.
 *
 * @param chars_type Type of the shared buffer (::lh_str_t / ::lh_wstr_t).
 * @param spans_type Type of the per-element table (::lh_vector_t).
 */
#define lh_str_list_fields(chars_type, spans_type)                                                 \
    chars_type chars;                                                                              \
    spans_type spans

#endif /* LH_STR_LIST_FIELDS_H */
