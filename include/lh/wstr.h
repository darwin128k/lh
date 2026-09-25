/**
 * @file wstr.h
 * @brief Owning, growable, NUL-terminated wide string (::lh_wstr_t).
 *
 * Wide counterpart of ::lh_str_t — same shape, ::lh_wchar_t elements
 * instead of ::lh_char_t. ::lh_str_format / ::lh_str_format_v have no
 * counterpart here: they depend on the narrow `lh_str_ptr_format_text_*`
 * formatter, and no wide equivalent exists in the library yet.
 *
 * @see lh_str_t
 * @see lh_wstr_view_t
 */

#ifndef LH_WSTR_H
#define LH_WSTR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/vector.h>
#include <lh/wchar.h>
#include <lh/wstr/ptr.h>
#include <lh/wstr/view.h>

/**
 * @struct lh_wstr
 * @brief Owning, growable, NUL-terminated wide string.
 *
 * A direct typedef of ::lh_vector_t; element type is ::lh_wchar_t
 * (`type_size == sizeof(lh_wchar_t)`).
 * Public typedef: ::lh_wstr_t.
 */
typedef lh_vector_t lh_wstr_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self as an empty, NUL-terminated wide string.
 *        Allocates nothing until the first append.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_init(lh_wstr_t *self);

/**
 * @brief Free the buffer owned by @p self and reset it to empty.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_deinit(lh_wstr_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return a pointer to the NUL-terminated character data.
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_cptr
lh_wstr_get_data(const lh_wstr_t *self);

/**
 * @brief Return the number of characters in @p self (excluding the terminator).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_get_size(const lh_wstr_t *self);

/**
 * @brief True when @p self has no characters.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_is_empty(const lh_wstr_t *self);

/* ── mutation ────────────────────────────────────────────────────────────── */

/**
 * @brief Append @p ch to the end of @p self, growing it if needed.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_push_back(lh_wstr_t *self, lh_wchar_t ch);

/**
 * @brief Append @p count characters from @p text to the end of @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_append(lh_wstr_t *self, lh_wstr_cptr text, lh_usize_t count);

/**
 * @brief Append the characters of @p view to @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_append_view(lh_wstr_t *self, lh_wstr_view_t view);

/**
 * @brief Append the characters of @p other to @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_append_str(lh_wstr_t *self, const lh_wstr_t *other);

/**
 * @brief Replace @p self with a copy of @p other.
 *
 * No-op when @p self is @p other.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_assign(lh_wstr_t *self, const lh_wstr_t *other);

/**
 * @brief Replace @p self with a copy of @p view.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_assign_view(lh_wstr_t *self, lh_wstr_view_t view);

/**
 * @brief Replace @p self with @p count views glued by @p sep.
 *
 * Empty views are kept (two empties yield a lone separator).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_join(lh_wstr_t *self, const lh_wstr_view_t *parts, lh_usize_t count, lh_wchar_t sep);

/**
 * @brief Empty @p self without releasing its buffer.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_clear(lh_wstr_t *self);

/**
 * @brief Shrink @p self to @p n characters and keep it NUL-terminated.
 *
 * @param n New size; must be <= ::lh_wstr_get_size.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_wstr_truncate(lh_wstr_t *self, lh_usize_t n);

/* ── view ────────────────────────────────────────────────────────────────── */

/**
 * @brief Return a non-owning view over @p self's current contents.
 *
 * The view is only valid as long as @p self is not mutated or deinitialized
 * afterwards.
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_view_t
lh_wstr_as_view(const lh_wstr_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WSTR_H */
