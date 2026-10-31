/**
 * @file private.h
 * @brief Private to `lexer.c`: the steps of the scan, below its public API.
 *
 * The character tests, the tables of keywords and operators, and the scanners
 * that walk the source text. They are not in `lh/js/lexer.h`: the caller of
 * the lexer scans tokens, it does not scan pieces of them.
 */

#ifndef LH_SRC_JS_LEXER_PRIVATE_H
#define LH_SRC_JS_LEXER_PRIVATE_H

#include <lh/js/lexer.h>

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The character the scanner is on; `'\0'` at the end of the text (level 2).
 */
lh_char_t
lh_js_lexer_get_char(const lh_js_lexer_t *self);

/**
 * @brief The character @p offset bytes after the scanner (level 2).
 *
 * @param self   Lexer to read.
 * @param offset How far to look, in bytes.
 * @param out    Receives the character.
 *
 * @return ::lh_bool_true when the character is there, ::lh_bool_false when the
 *         text ends before it.
 */
lh_bool_t
lh_js_lexer_try_get_char_at(const lh_js_lexer_t *self, lh_usize_t offset, lh_char_t *out);

/**
 * @brief Whether the scanner has reached the end of the text (level 2).
 */
lh_bool_t
lh_js_lexer_is_at_end(const lh_js_lexer_t *self);

/**
 * @brief The text the scanner has left (level 2).
 *
 * @param self Lexer to read.
 * @param out  Receives the text from the scanner's position to the end.
 */
lh_void
lh_js_lexer_get_rest(const lh_js_lexer_t *self, lh_str_view_t *out);

/**
 * @brief Move the scanner one character on (level 2).
 *
 * A line terminator moves the line on and puts the column back to `1`; a
 * CRLF counts as one terminator.
 */
lh_void
lh_js_lexer_advance(lh_js_lexer_t *self);

/**
 * @brief The token of @p self, to change in place (level 1).
 */
lh_js_token_t *
lh_js_lexer_get_token_mut(lh_js_lexer_t *self);

/**
 * @brief The buffer the error message is kept in, to write in (level 1).
 */
lh_str_ptr
lh_js_lexer_get_error_buffer_mut(lh_js_lexer_t *self);

/**
 * @brief Whether @p ch starts a name (level 1).
 */
lh_bool_t
lh_js_lexer_is_name_start(lh_char_t ch);

/**
 * @brief Whether @p ch goes on a name (level 1).
 */
lh_bool_t
lh_js_lexer_is_name_part(lh_char_t ch);

/**
 * @brief Whether @p text is a keyword of JavaScript (level 1).
 *
 * @param text The text to look at.
 */
lh_bool_t
lh_js_lexer_is_keyword(const lh_str_view_t *text);

/**
 * @brief The operator @p text starts with (level 1).
 *
 * @param text The text to look at.
 *
 * @return The length of the operator in bytes; `0` when the text starts with
 *         no operator of JavaScript.
 */
lh_usize_t
lh_js_lexer_find_punctuation(const lh_str_view_t *text);

/**
 * @brief The radix of a number prefix (level 1).
 *
 * @param prefix The character after the `0` of a number.
 *
 * @return The base of the number; `0` when the prefix is not one.
 */
lh_uint_t
lh_js_lexer_find_radix(lh_char_t prefix);

/**
 * @brief Skip the whitespace and the comments before the next token (level 3).
 *
 * @return ::lh_bool_true when the scan may go on, ::lh_bool_false on a
 *         malformed comment.
 */
lh_bool_t
lh_js_lexer_skip_space(lh_js_lexer_t *self);

/**
 * @brief Scan a name or a keyword (level 3).
 */
lh_bool_t
lh_js_lexer_scan_name(lh_js_lexer_t *self);

/**
 * @brief Scan a number (level 3).
 */
lh_bool_t
lh_js_lexer_scan_number(lh_js_lexer_t *self);

/**
 * @brief Scan a number in the base @p radix (level 3).
 *
 * @param self  Lexer to scan.
 * @param radix The base of the number: `2`, `8` or `16`.
 */
lh_bool_t
lh_js_lexer_scan_radix_number(lh_js_lexer_t *self, lh_uint_t radix);

/**
 * @brief Scan a number in base ten (level 3).
 */
lh_bool_t
lh_js_lexer_scan_decimal_number(lh_js_lexer_t *self);

/**
 * @brief Scan a string literal (level 3).
 */
lh_bool_t
lh_js_lexer_scan_string(lh_js_lexer_t *self);

/**
 * @brief Scan the escape sequence after a backslash (level 3).
 *
 * The backslash itself is already behind the scanner.
 */
lh_bool_t
lh_js_lexer_scan_escape(lh_js_lexer_t *self);

/**
 * @brief Scan @p count hexadecimal digits (level 3).
 *
 * @param self  Lexer to scan.
 * @param count How many digits the escape asks for.
 */
lh_bool_t
lh_js_lexer_scan_hex_digits(lh_js_lexer_t *self, lh_usize_t count);

/**
 * @brief Scan the code point of a `\u{...}` escape (level 3).
 */
lh_bool_t
lh_js_lexer_scan_code_point(lh_js_lexer_t *self);

/**
 * @brief Scan a line terminator (level 3).
 */
lh_bool_t
lh_js_lexer_scan_newline(lh_js_lexer_t *self);

/**
 * @brief Scan an operator or a bracket (level 3).
 */
lh_bool_t
lh_js_lexer_scan_punctuation(lh_js_lexer_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_JS_LEXER_PRIVATE_H */
