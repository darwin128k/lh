/**
 * @file str.h
 * @brief Owning, growable, NUL-terminated string (::lh_str_t).
 *
 * ::lh_str_t is a direct typedef of ::lh_vector_t with an ::lh_char_t
 * element (`type_size == sizeof(lh_char_t)`) — the owning, growable
 * counterpart to the non-owning ::lh_str_view_t.
 *
 * Every mutator (::lh_str_push_back, ::lh_str_append, ::lh_str_append_view,
 * ::lh_str_append_str, ::lh_str_assign, ::lh_str_assign_view, ::lh_str_join,
 * ::lh_str_format, ::lh_str_clear, ::lh_str_truncate) keeps one spare byte
 * past ::lh_str_get_size set to `'\0'`, so ::lh_str_get_data can be handed
 * straight to libc / printf-style APIs without a separate termination step.
 * The terminator does not count toward ::lh_str_get_size, matching
 * ::lh_vector_get_size semantics. A string that has never grown owns no
 * buffer (::lh_str_init allocates nothing); ::lh_str_get_data then returns a
 * static empty string.
 *
 * Because the type is layout-compatible with ::lh_vector_t, all
 * ::lh_vector functions accept an ::lh_str_t pointer without a cast —
 * but reach for those only when you don't need the NUL-termination
 * guarantee, since they don't maintain it.
 *
 * @see lh_vector_t
 * @see lh_str_view_t
 */

#ifndef LH_STR_H
#define LH_STR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/vector.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>
#include <lh/char.h>
#include <lh/size.h>
#include <lh/bool.h>

#include <stdarg.h>

/**
 * @struct lh_str
 * @brief Owning, growable, NUL-terminated string.
 *
 * A direct typedef of ::lh_vector_t; element type is ::lh_char_t
 * (`type_size == sizeof(lh_char_t)`).
 * Public typedef: ::lh_str_t.
 */
typedef lh_vector_t lh_str_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self as an empty, NUL-terminated string. Allocates
 *        nothing until the first append or reserve.
 * @param self String to initialize.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_init(lh_str_t *self);

/**
 * @brief Free the buffer owned by @p self and reset it to empty.
 * @param self String to deinitialize.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_deinit(lh_str_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return a pointer to the NUL-terminated character data.
 * @param self String to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_get_data(const lh_str_t *self);

/**
 * @brief Return the number of characters in @p self (excluding the terminator).
 * @param self String to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_get_size(const lh_str_t *self);

/**
 * @brief True when @p self has no characters.
 * @param self String to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_is_empty(const lh_str_t *self);

/* ── mutation ────────────────────────────────────────────────────────────── */

/**
 * @brief Append @p ch to the end of @p self, growing it if needed.
 * @param self String to append to.
 * @param ch   Character to append.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_push_back(lh_str_t *self, lh_char_t ch);

/**
 * @brief Append @p count characters from @p text to the end of @p self.
 *
 * @param self  String to append to.
 * @param text  Pointer to @p count contiguous characters (not null unless
 *              @p count is 0); their bytes are copied.
 * @param count Number of characters to append.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_append(lh_str_t *self, lh_str_cptr text, lh_usize_t count);

/**
 * @brief Append the characters of @p view to @p self.
 *
 * Empty @p view is a no-op.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_append_view(lh_str_t *self, lh_str_view_t view);

/**
 * @brief Append the characters of @p other to @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_append_str(lh_str_t *self, const lh_str_t *other);

/**
 * @brief Replace @p self with a copy of @p other.
 *
 * No-op when @p self is @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_assign(lh_str_t *self, const lh_str_t *other);

/**
 * @brief Replace @p self with a copy of @p view.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_assign_view(lh_str_t *self, lh_str_view_t view);

/**
 * @brief Replace @p self with @p count views glued by @p sep.
 *
 * Empty views are kept (two empties yield a lone separator).
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_join(lh_str_t *self, const lh_str_view_t *parts, lh_usize_t count, lh_char_t sep);

/**
 * @brief Format @p fmt into @p self, replacing current contents.
 *
 * Same conversions as ::lh_str_ptr_format_text. On a malformed mask @p self
 * is left unchanged and 0 is returned. An empty mask clears @p self.
 *
 * @return Characters written, or 0 on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_format_v(lh_str_t *self, lh_str_cptr fmt, va_list args);

/**
 * @brief Variadic wrapper for ::lh_str_format_v.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_format(lh_str_t *self, lh_str_cptr fmt, ...);

/**
 * @brief Make room for at least @p count characters (plus the terminator)
 *        without reallocating along the way.
 *
 * No-op when @p self already has the room. Size and contents are
 * unchanged. Use it before a known number of appends so the buffer grows
 * once instead of step by step.
 *
 * @param self  String to grow.
 * @param count Characters @p self must be able to hold, excluding the NUL.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_reserve(lh_str_t *self, lh_usize_t count);

/**
 * @brief Empty @p self without releasing its buffer.
 * @param self String to clear.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_clear(lh_str_t *self);

/**
 * @brief Shrink @p self to @p n characters and keep it NUL-terminated.
 *
 * @param self String to shrink.
 * @param n    New size; must be <= ::lh_str_get_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_truncate(lh_str_t *self, lh_usize_t n);

/* ── view ────────────────────────────────────────────────────────────────── */

/**
 * @brief Return a non-owning view over @p self's current contents.
 *
 * The view is only valid as long as @p self is not mutated or deinitialized
 * afterwards (same lifetime rule as any pointer into @p self's buffer).
 *
 * @param self String to view.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_str_as_view(const lh_str_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_H */
