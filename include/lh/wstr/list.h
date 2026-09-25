/**
 * @file list.h
 * @brief Growable list of wide strings in one shared buffer (::lh_wstr_list_t).
 *
 * Wide counterpart of ::lh_str_list_t, same layout (::lh_str_list_fields).
 *
 * Every element's characters live back to back in a single ::lh_wstr_t, each
 * followed by a NUL, with a side table of (offset, size) per element. Adding
 * an element is an append to that buffer — no allocation of its own — and
 * reading one is a view into it (::lh_wstr_list_get) or a NUL-terminated
 * pointer (::lh_wstr_list_get_data), never a copy.
 *
 * Views and pointers handed out stay valid until @p self is next modified
 * or deinitialized: growing the buffer may move it. Elements are not
 * mutable in place.
 */

#ifndef LH_WSTR_LIST_H
#define LH_WSTR_LIST_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/index.h>
#include <lh/size.h>
#include <lh/str/list/fields.h>
#include <lh/wstr.h>
#include <lh/wstr/ptr.h>
#include <lh/wstr/view.h>
#include <lh/vector.h>

/**
 * @struct lh_wstr_list
 * @brief Shared character buffer plus per-element spans. Fields via
 *        ::lh_str_list_fields.
 */
typedef struct lh_wstr_list
{
    lh_str_list_fields(lh_wstr_t, lh_vector_t);
} lh_wstr_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty list. Allocates nothing until the first element.
 */
void
lh_wstr_list_init(lh_wstr_list_t *self);

/**
 * @brief Release the shared buffer and the span table.
 */
void
lh_wstr_list_deinit(lh_wstr_list_t *self);

/**
 * @brief Remove every element. Keeps both allocations for reuse.
 */
void
lh_wstr_list_clear(lh_wstr_list_t *self);

/**
 * @brief True when @p self has no elements.
 */
lh_bool_t
lh_wstr_list_is_empty(const lh_wstr_list_t *self);

/**
 * @brief Number of elements in @p self.
 */
lh_usize_t
lh_wstr_list_get_size(const lh_wstr_list_t *self);

/**
 * @brief Element @p index as a view into the shared buffer (empty view for
 *        an empty element).
 *
 * @throw ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_wstr_list_get_size.
 */
lh_wstr_view_t
lh_wstr_list_get(const lh_wstr_list_t *self, lh_uindex_t index);

/**
 * @brief Element @p index as a NUL-terminated pointer into the shared
 *        buffer — ready for a C / OS API without a copy.
 *
 * @throw ::lh_runtime_error_code_out_of_range @p index is not below
 *        ::lh_wstr_list_get_size.
 */
lh_wstr_cptr
lh_wstr_list_get_data(const lh_wstr_list_t *self, lh_uindex_t index);

/**
 * @brief Append a copy of @p text as a new element.
 * @return Index of the new element.
 */
lh_uindex_t
lh_wstr_list_push_back(lh_wstr_list_t *self, lh_wstr_view_t text);

/**
 * @brief Append a copy of @p value as a new element.
 * @return Index of the new element.
 */
lh_uindex_t
lh_wstr_list_push_back_str(lh_wstr_list_t *self, const lh_wstr_t *value);

/**
 * @brief Make @p self a copy of @p other (two buffer copies, no per-element
 *        work).
 */
void
lh_wstr_list_assign(lh_wstr_list_t *self, const lh_wstr_list_t *other);

/**
 * @brief Append copies of every element of @p other to @p self.
 *
 * @p other may be @p self.
 */
void
lh_wstr_list_append(lh_wstr_list_t *self, const lh_wstr_list_t *other);

/**
 * @brief Append every element of @p self to @p out, separated by @p sep.
 */
void
lh_wstr_list_join(const lh_wstr_list_t *self, lh_wstr_t *out, lh_wchar_t sep);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WSTR_LIST_H */
