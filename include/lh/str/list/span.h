/**
 * @file span.h
 * @brief One element's place in a string list's shared buffer (::lh_str_list_span_t).
 *
 * Shared by ::lh_str_list_t and ::lh_wstr_list_t: both keep a table of these,
 * one per element, next to their buffer.
 */

#ifndef LH_STR_LIST_SPAN_H
#define LH_STR_LIST_SPAN_H

#include <lh/size.h>
#include <lh/str/list/span/fields.h>

/**
 * @struct lh_str_list_span
 * @brief Offset and size of one element; its NUL follows at offset + size.
 *        Fields via ::lh_str_list_span_fields.
 */
struct lh_str_list_span
{
    /** Members: ::lh_str_list_span_fields. */
    lh_str_list_span_fields(lh_usize_t);
};
/** @copybrief lh_str_list_span */
typedef struct lh_str_list_span lh_str_list_span_t;

#endif /* LH_STR_LIST_SPAN_H */
