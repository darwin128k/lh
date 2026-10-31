/**
 * @file fields.h
 * @brief Member fields of ::lh_js_token_t.
 */

#ifndef LH_JS_LEXER_TOKEN_FIELDS_H
#define LH_JS_LEXER_TOKEN_FIELDS_H

/**
 * @def lh_js_token_fields(kind_type, text_type, size_type, value_type)
 * @brief One token of JavaScript source:
 *
 * - `kind` — what the token is (::lh_js_token_kind_t);
 * - `text` — the source text the token was made of (not owned);
 * - `text_len` — its length in bytes;
 * - `value` — the number the token carries (::lh_real_t).
 */
#define lh_js_token_fields(kind_type, text_type, size_type, value_type)                            \
    kind_type kind;                                                                                \
    text_type text;                                                                                \
    size_type text_len;                                                                            \
    value_type value

#endif /* LH_JS_LEXER_TOKEN_FIELDS_H */
