/**
 * @file span.h
 * @brief One element's place in a string list's shared buffer (::lh_str_list_span_t).
 *
 * Shared by ::lh_str_list_t and ::lh_wstr_list_t: both keep a table of these,
 * one per element, next to their buffer.
 */

#ifndef LH_STR_LIST_SPAN_H
#define LH_STR_LIST_SPAN_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/void.h>
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

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Offset of the element's first character in the list's buffer. */
lh_usize_t
lh_str_list_span_get_offset(const lh_str_list_span_t *self);

/** @brief Number of characters of the element, its NUL not counted. */
lh_usize_t
lh_str_list_span_get_size(const lh_str_list_span_t *self);

/** @brief Store the offset of the element's first character. */
lh_void
lh_str_list_span_set_offset(lh_str_list_span_t *self, lh_usize_t offset);

/** @brief Store the number of characters of the element. */
lh_void
lh_str_list_span_set_size(lh_str_list_span_t *self, lh_usize_t size);

/**
 * @brief Place an element of @p size characters at @p offset.
 *
 * ::lh_str_list_span_set_offset and ::lh_str_list_span_set_size.
 */
lh_void
lh_str_list_span_init(lh_str_list_span_t *self, lh_usize_t offset, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_LIST_SPAN_H */
