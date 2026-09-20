/**
 * @file list.h
 * @brief Growable list of owning wide strings (::lh_wstr_list_t).
 *
 * Wide counterpart of ::lh_str_list_t — same shape (reuses
 * ::lh_str_list_fields directly, see `lh/str/list/fields.h`), element type
 * ::lh_wstr_t instead of ::lh_str_t. Same caveat applies: ::lh_vector_t
 * moves element bytes without knowing an element can own a heap buffer, so
 * every operation here that removes or overwrites an element calls
 * ::lh_wstr_deinit / ::lh_wstr_assign on it by hand instead of the raw
 * ::lh_vector_clear / ::lh_vector_assign.
 */

#ifndef LH_WSTR_LIST_H
#define LH_WSTR_LIST_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/index.h>
#include <lh/size.h>
#include <lh/str/list/fields.h>
#include <lh/vector.h>
#include <lh/wstr.h>
#include <lh/wstr/view.h>

/**
 * @struct lh_wstr_list
 * @brief Growable list of ::lh_wstr_t elements. Fields via ::lh_str_list_fields.
 */
typedef struct lh_wstr_list
{
    lh_str_list_fields(lh_vector_t);
} lh_wstr_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Underlying storage of @p self (element type ::lh_wstr_t).
 *
 * Raw escape hatch: ::lh_vector_clear / ::lh_vector_erase / ::lh_vector_assign
 * and friends do not know an element owns a heap buffer and will leak or
 * double-free it if used here instead of the ::lh_wstr_list_* equivalents.
 */
LH_ATTRIBUTE_SYMBOL
lh_vector_t *
lh_wstr_list_get_items(lh_wstr_list_t *self);

/**
 * @brief `const` counterpart to ::lh_wstr_list_get_items.
 */
LH_ATTRIBUTE_SYMBOL
const lh_vector_t *
lh_wstr_list_get_items_as_const(const lh_wstr_list_t *self);

/**
 * @brief Initialize @p self as an empty list.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_init(lh_wstr_list_t *self);

/**
 * @brief Deinit every stored string, then release the list's own storage.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_deinit(lh_wstr_list_t *self);

/**
 * @brief Deinit every stored string, keeping the list's own allocation.
 *
 * Unlike ::lh_wstr_list_deinit, @p self can be pushed to again afterwards
 * without reallocating its item storage.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_clear(lh_wstr_list_t *self);

/**
 * @brief True when @p self has no elements.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_list_is_empty(const lh_wstr_list_t *self);

/**
 * @brief Number of strings in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_list_get_size(const lh_wstr_list_t *self);

/**
 * @brief String at @p index.
 *
 * @param index Element index; must be < ::lh_wstr_list_get_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_t *
lh_wstr_list_get(lh_wstr_list_t *self, lh_uindex_t index);

/**
 * @brief `const` counterpart to ::lh_wstr_list_get.
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_t *
lh_wstr_list_get_as_const(const lh_wstr_list_t *self, lh_uindex_t index);

/**
 * @brief Append a copy of @p text as a new owned string at the end of @p self.
 * @return Index the new string was stored at (::lh_wstr_list_get_size before
 *         the call).
 */
LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_wstr_list_push_back(lh_wstr_list_t *self, lh_wstr_view_t text);

/**
 * @brief Append a copy of @p value (an existing ::lh_wstr_t) at the end of @p self.
 *
 * Equivalent to ::lh_wstr_list_push_back with ::lh_wstr_as_view(@p value).
 * @return Index the new string was stored at.
 */
LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_wstr_list_push_back_str(lh_wstr_list_t *self, const lh_wstr_t *value);

/**
 * @brief Replace @p self with a deep copy of @p other.
 *
 * Every string is copied, not aliased. No-op when @p self is @p other.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_assign(lh_wstr_list_t *self, const lh_wstr_list_t *other);

/**
 * @brief Append a copy of every string in @p other to the end of @p self.
 *
 * Safe when @p self is @p other (doubles @p self).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_append(lh_wstr_list_t *self, const lh_wstr_list_t *other);

/**
 * @brief Append every string in @p self to @p out, with @p sep between
 *        consecutive strings.
 *
 * Unlike ::lh_wstr_join, this does not clear @p out first — it appends.
 * No separator before the first string or after the last.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_list_join(const lh_wstr_list_t *self, lh_wstr_t *out, lh_wchar_t sep);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WSTR_LIST_H */
