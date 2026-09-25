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

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/index.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/list/fields.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>
#include <lh/vector.h>

/**
 * @struct lh_str_list
 * @brief Shared character buffer plus per-element spans. Fields via
 *        ::lh_str_list_fields.
 */
typedef struct lh_str_list
{
    lh_str_list_fields(lh_str_t, lh_vector_t);
} lh_str_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty list. Allocates nothing until the first element.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_init(lh_str_list_t *self);

/**
 * @brief Release the shared buffer and the span table.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_deinit(lh_str_list_t *self);

/**
 * @brief Remove every element. Keeps both allocations for reuse.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_clear(lh_str_list_t *self);

/**
 * @brief True when @p self has no elements.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_list_is_empty(const lh_str_list_t *self);

/**
 * @brief Number of elements in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_list_get_size(const lh_str_list_t *self);

/**
 * @brief Element @p index as a view into the shared buffer (empty view for
 *        an empty element).
 *
 * @throw ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_str_list_get_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_str_list_get(const lh_str_list_t *self, lh_uindex_t index);

/**
 * @brief Element @p index as a NUL-terminated pointer into the shared
 *        buffer — ready for a C / OS API without a copy.
 *
 * @throw ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_str_list_get_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_list_get_data(const lh_str_list_t *self, lh_uindex_t index);

/**
 * @brief Append a copy of @p text as a new element.
 * @return Index of the new element.
 */
LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_str_list_push_back(lh_str_list_t *self, lh_str_view_t text);

/**
 * @brief Append a copy of @p value as a new element.
 * @return Index of the new element.
 */
LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_str_list_push_back_str(lh_str_list_t *self, const lh_str_t *value);

/**
 * @brief Make @p self a copy of @p other (two buffer copies, no per-element
 *        work).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_assign(lh_str_list_t *self, const lh_str_list_t *other);

/**
 * @brief Append copies of every element of @p other to @p self.
 *
 * @p other may be @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_append(lh_str_list_t *self, const lh_str_list_t *other);

/**
 * @brief Append every element of @p self to @p out, separated by @p sep.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_join(const lh_str_list_t *self, lh_str_t *out, lh_char_t sep);

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
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_split_of(lh_str_list_t *self, lh_str_view_t text, lh_str_cptr delims, lh_usize_t delim_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_LIST_H */
