/**
 * @file token.h
 * @brief One token of JavaScript source: what it is, the text it was made of,
 *        and the number it carries.
 */

#ifndef LH_JS_LEXER_TOKEN_H
#define LH_JS_LEXER_TOKEN_H

#include <lh/compiler/extern/c.h>
#include <lh/js/lexer/kind.h>
#include <lh/js/lexer/token/fields.h>
#include <lh/real.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_js_token
 * @brief One token of JavaScript source. Fields via ::lh_js_token_fields.
 *
 * The text of a token is the source text the token was made of — a pointer
 * into the source and a length, not owned, not copied, not decoded. A string
 * token's text is the literal as written, quotes and escapes included; the
 * values of the engine decode it later.
 */
struct lh_js_token
{
    /** Members: ::lh_js_token_fields. */
    lh_js_token_fields(lh_js_token_kind_t, const lh_str_ptr, lh_usize_t, lh_real_t);
};
/** @copybrief lh_js_token */
typedef struct lh_js_token lh_js_token_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What @p self is (level 1).
 *
 * @param self Token to read.
 * @return The token's kind.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_js_token_kind_t
lh_js_token_get_kind(const lh_js_token_t *self);

/**
 * @brief Store the kind of @p self (level 1).
 *
 * @param self Token to write.
 * @param kind The kind to store.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_token_set_kind(lh_js_token_t *self, lh_js_token_kind_t kind);

/**
 * @brief The source text @p self was made of (level 1).
 *
 * The text is not owned: it points into the source the lexer reads, and it
 * lives as long as that source does.
 *
 * @param self Token to read.
 * @return The token's text; ::lh_null when the token has none.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_str_ptr
lh_js_token_get_text(const lh_js_token_t *self);

/**
 * @brief Store the source text of @p self (level 1).
 *
 * The text is stored as it is given: the caller keeps it alive.
 *
 * @param self Token to write.
 * @param text The text to store.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_token_set_text(lh_js_token_t *self, const lh_str_ptr text);

/**
 * @brief The length of the text of @p self, in bytes (level 1).
 *
 * @param self Token to read.
 * @return The text's length; `0` when the token has no text.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_token_get_text_len(const lh_js_token_t *self);

/**
 * @brief Store the length of the text of @p self (level 1).
 *
 * @param self Token to write.
 * @param len The length to store, in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_token_set_text_len(lh_js_token_t *self, lh_usize_t len);

/**
 * @brief The number @p self carries (level 1).
 *
 * Only a number token carries one; the other tokens carry `0`.
 *
 * @param self Token to read.
 * @return The token's number.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_real_t
lh_js_token_get_value(const lh_js_token_t *self);

/**
 * @brief Store the number @p self carries (level 1).
 *
 * @param self Token to write.
 * @param value The number to store.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_token_set_value(lh_js_token_t *self, lh_real_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JS_LEXER_TOKEN_H */
