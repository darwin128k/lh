/**
 * @file list.h
 * @brief Growable list of strings in one shared buffer (::lh_str_list_t).
 *
 * Every element's characters live back to back in a single ::lh_str_t, each
 * followed by a NUL, with a side table of (offset, size) per element. Adding
 * an element is an append to that buffer — no allocation of its own — and
 * reading one is a view into it (::lh_str_list_get) or a NUL-terminated
 * pointer (::lh_str_list_get_data), never a copy.
 *
 * Views and pointers handed out stay valid until @p self is next modified
 * or deinitialized: growing the buffer may move it. Elements are not
 * mutable in place.
 */

#ifndef LH_STR_LIST_H
#define LH_STR_LIST_H

#include <lh/str.h>
#include <lh/str/list/fields.h>
#include <lh/str/list/span.h>

/**
 * @struct lh_str_list
 * @brief Shared character buffer plus per-element spans. Fields via
 *        ::lh_str_list_fields.
 */
struct lh_str_list
{
    /** Members: ::lh_str_list_fields. */
    lh_str_list_fields(lh_str_t, lh_array_t);
};
/** @copybrief lh_str_list */
typedef struct lh_str_list lh_str_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The shared character buffer. */
lh_str_t *
lh_str_list_get_chars_mut(lh_str_list_t *self);

/** @brief The shared character buffer, read-only. */
const lh_str_t *
lh_str_list_get_chars(const lh_str_list_t *self);

/** @brief The table of ::lh_str_list_span_t, one per element. */
lh_array_t *
lh_str_list_get_spans_mut(lh_str_list_t *self);

/** @brief The table of ::lh_str_list_span_t, read-only. */
const lh_array_t *
lh_str_list_get_spans(const lh_str_list_t *self);

/**
 * @brief Write where element @p index sits in the buffer to @p out.
 *
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::lh_str_list_get_size.
 */
lh_void
lh_str_list_get_span(const lh_str_list_t *self, lh_uindex_t index, lh_str_list_span_t *out);

/**
 * @brief Empty list. Allocates nothing until the first element.
 */
lh_void
lh_str_list_init(lh_str_list_t *self);

/**
 * @brief Release the shared buffer and the span table.
 */
lh_void
lh_str_list_deinit(lh_str_list_t *self);

/**
 * @brief Remove every element. Keeps both allocations for reuse.
 */
lh_void
lh_str_list_clear(lh_str_list_t *self);

/**
 * @brief True when @p self has no elements.
 */
lh_bool_t
lh_str_list_is_empty(const lh_str_list_t *self);

/**
 * @brief Number of elements in @p self.
 */
lh_usize_t
lh_str_list_get_size(const lh_str_list_t *self);

/**
 * @brief Append a copy of @p text as a new element.
 * @return Index of the new element.
 */
lh_uindex_t
lh_str_list_push_back(lh_str_list_t *self, lh_str_view_t text);

/**
 * @brief Make @p self a copy of @p other (two buffer copies, no per-element
 *        work).
 */
lh_void
lh_str_list_assign(lh_str_list_t *self, const lh_str_list_t *other);

/**
 * @brief Element @p index as a view into the shared buffer (empty view for
 *        an empty element).
 *
 * @fails ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_str_list_get_size.
 */
lh_void
lh_str_list_get(const lh_str_list_t *self, lh_uindex_t index, lh_str_view_t *out);

/**
 * @brief Element @p index as a NUL-terminated pointer into the shared
 *        buffer — ready for a C / OS API without a copy.
 *
 * @fails ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_str_list_get_size.
 */
const lh_str_ptr
lh_str_list_get_data(const lh_str_list_t *self, lh_uindex_t index);

/**
 * @brief Append a copy of @p value as a new element.
 * @return Index of the new element.
 */
lh_uindex_t
lh_str_list_push_back_str(lh_str_list_t *self, const lh_str_t *value);

/**
 * @brief Append copies of every element of @p other to @p self.
 *
 * @p other may be @p self.
 */
lh_void
lh_str_list_append(lh_str_list_t *self, const lh_str_list_t *other);

/**
 * @brief Split @p text on any of @p delims, appending one element to
 *        @p self per non-empty piece.
 *
 * Inverse of ::lh_str_list_join for the common "segments" shape (a
 * filesystem path, a URL path, ...): leading, trailing, and consecutive
 * delimiters produce no empty elements.
 *
 * Does not clear @p self first — it appends, mirroring ::lh_str_list_join.
 */
lh_void
lh_str_list_split_of(lh_str_list_t *self, lh_str_view_t text, const lh_str_ptr delims,
                     lh_usize_t delim_count);

/**
 * @brief Append every element of @p self to @p out, separated by @p sep.
 */
lh_void
lh_str_list_join(const lh_str_list_t *self, lh_str_t *out, lh_char_t sep);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_LIST_H */
