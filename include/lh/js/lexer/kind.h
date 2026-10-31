/**
 * @file kind.h
 * @brief ::lh_js_token_kind_t — what a token of JavaScript source is.
 */

#ifndef LH_JS_LEXER_KIND_H
#define LH_JS_LEXER_KIND_H

/**
 * @enum lh_js_token_kind
 * @brief What a token of JavaScript source is.
 *
 * The kind says what the token is; the text it was made of says which one it
 * is — a keyword and an operator carry their own text.
 */
typedef enum lh_js_token_kind
{
    lh_js_token_kind_end_of_input = 0, /**< The end of the source: no token follows it. */
    lh_js_token_kind_identifier,       /**< A name: `count`, `$name`, `_name`. */
    lh_js_token_kind_keyword,          /**< A reserved word: `if`, `while`, `function`. */
    lh_js_token_kind_number,           /**< A number: `12`, `1.5`, `0x1f`. */
    lh_js_token_kind_string,           /**< A string literal: `'text'`, `"text"`. */
    lh_js_token_kind_punctuation,      /**< An operator or a bracket: `+`, `=>`, `(`. */
    lh_js_token_kind_regexp,           /**< A regular expression literal: `/text/flags`. */
    lh_js_token_kind_newline           /**< A line terminator: the parser's semicolon insertion. */
} lh_js_token_kind_t;                  /**< What a token of JavaScript source is. */

#endif /* LH_JS_LEXER_KIND_H */
