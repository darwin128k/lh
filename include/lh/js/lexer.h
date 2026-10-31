/**
 * @file lexer.h
 * @brief The JavaScript lexer: source text in, tokens out.
 *
 * The scanner reads one source text and hands out its tokens one by one. It
 * owns nothing: the text belongs to the caller, and every token's text points
 * into it.
 *
 * What it recognizes: names, keywords, numbers (decimal, hexadecimal, octal
 * and binary), strings with their escapes, comments (skipped), the operators
 * and the brackets of JavaScript, and the line terminators — a newline is a
 * token of its own, because the parser's automatic semicolon insertion needs
 * to see it.
 *
 * A regular expression literal is not scanned here: only the parser knows
 * whether a `/` divides or opens a regexp, and that is the next slice.
 */

#ifndef LH_JS_LEXER_H
#define LH_JS_LEXER_H

#include <lh/bool.h>
#include <lh/js/lexer/fields.h>
#include <lh/js/lexer/token.h>

/**
 * @struct lh_js_lexer
 * @brief The scanner over one JavaScript source text. Fields via
 *        ::lh_js_lexer_fields.
 */
struct lh_js_lexer
{
    /** Members: ::lh_js_lexer_fields. */
    lh_js_lexer_fields(const lh_str_ptr, lh_usize_t, lh_js_token_t, lh_str_ptr, lh_char_t);
};
/** @copybrief lh_js_lexer */
typedef struct lh_js_lexer lh_js_lexer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The source text @p self reads (level 1).
 *
 * The text is not owned: it belongs to the caller, and every token's text
 * points into it.
 *
 * @param self Lexer to read.
 * @return The source text.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_str_ptr
lh_js_lexer_get_text(const lh_js_lexer_t *self);

/**
 * @brief Store the source text of @p self (level 1).
 *
 * The text is stored as it is given: the caller keeps it alive.
 *
 * @param self Lexer to write.
 * @param text The source text.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_text(lh_js_lexer_t *self, const lh_str_ptr text);

/**
 * @brief The length of the source text of @p self, in bytes (level 1).
 *
 * @param self Lexer to read.
 * @return The source text's length.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_text_len(const lh_js_lexer_t *self);

/**
 * @brief Store the length of the source text of @p self (level 1).
 *
 * @param self Lexer to write.
 * @param len The length to store, in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_text_len(lh_js_lexer_t *self, lh_usize_t len);

/**
 * @brief Where @p self is in the source text, in bytes from the start (level 1).
 *
 * @param self Lexer to read.
 * @return The scanner's offset.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_pos(const lh_js_lexer_t *self);

/**
 * @brief Store where @p self is in the source text (level 1).
 *
 * @param self Lexer to write.
 * @param pos The offset to store, in bytes from the start.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_pos(lh_js_lexer_t *self, lh_usize_t pos);

/**
 * @brief The token @p self scanned last (level 1).
 *
 * The token belongs to the lexer: it is read-only for the caller, and the
 * next scan replaces it.
 *
 * @param self Lexer to read.
 * @return The last token scanned.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_js_token_t *
lh_js_lexer_get_token(const lh_js_lexer_t *self);

/**
 * @brief Store the token of @p self (level 1).
 *
 * The token is copied as it is given; its text still points where it pointed.
 *
 * @param self Lexer to write.
 * @param token The token to store.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p token is ::lh_null.
 */
lh_void
lh_js_lexer_set_token(lh_js_lexer_t *self, const lh_js_token_t *token);

/**
 * @brief The line @p self is on, counting from `1` (level 1).
 *
 * @param self Lexer to read.
 * @return The scanner's line.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_line(const lh_js_lexer_t *self);

/**
 * @brief Store the line of @p self (level 1).
 *
 * @param self Lexer to write.
 * @param line The line to store, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_line(lh_js_lexer_t *self, lh_usize_t line);

/**
 * @brief The column @p self is on, counting from `1` (level 1).
 *
 * The column counts bytes, not characters: the scanner reads bytes.
 *
 * @param self Lexer to read.
 * @return The scanner's column.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_column(const lh_js_lexer_t *self);

/**
 * @brief Store the column of @p self (level 1).
 *
 * @param self Lexer to write.
 * @param column The column to store, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_column(lh_js_lexer_t *self, lh_usize_t column);

/**
 * @brief The message of the last failure (level 1).
 *
 * The message is the lexer's own text: the caller must not free it, and it
 * lives as long as the lexer does.
 *
 * @param self Lexer to read.
 * @return The failure's message; ::lh_null when there is no failure.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_str_ptr
lh_js_lexer_get_error_message(const lh_js_lexer_t *self);

/**
 * @brief Store the message of the last failure (level 1).
 *
 * The message is stored as it is given: the caller keeps it alive.
 *
 * @param self Lexer to write.
 * @param message The message to store.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_error_message(lh_js_lexer_t *self, lh_str_ptr message);

/**
 * @brief The length of the last failure's message, in bytes (level 1).
 *
 * @param self Lexer to read.
 * @return The message's length; `0` when there is no failure.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_error_message_len(const lh_js_lexer_t *self);

/**
 * @brief Store the length of the last failure's message (level 1).
 *
 * @param self Lexer to write.
 * @param len The length to store, in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_error_message_len(lh_js_lexer_t *self, lh_usize_t len);

/**
 * @brief The line the last failure was found on (level 1).
 *
 * @param self Lexer to read.
 * @return The failure's line, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_error_line(const lh_js_lexer_t *self);

/**
 * @brief Store the line the last failure was found on (level 1).
 *
 * @param self Lexer to write.
 * @param line The line to store, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_error_line(lh_js_lexer_t *self, lh_usize_t line);

/**
 * @brief The column the last failure was found on (level 1).
 *
 * @param self Lexer to read.
 * @return The failure's column, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_js_lexer_get_error_column(const lh_js_lexer_t *self);

/**
 * @brief Store the column the last failure was found on (level 1).
 *
 * @param self Lexer to write.
 * @param column The column to store, counting from `1`.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_error_column(lh_js_lexer_t *self, lh_usize_t column);

/**
 * @brief Start @p self at the beginning of @p text (level 2).
 *
 * The text is not owned: it belongs to the caller, and every token's text
 * points into it. The line and the column start at `1`, the position at `0`,
 * and the failure state is empty.
 *
 * @param self Lexer to start.
 * @param text The source text to read.
 * @param len  The source text's length, in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_init(lh_js_lexer_t *self, const lh_str_ptr text, lh_usize_t len);

/**
 * @brief The last failure of @p self, if there is one (level 2).
 *
 * The message is the lexer's own text: the caller must not free it.
 *
 * @param self    Lexer to read.
 * @param message Receives the message; untouched when there is no failure.
 * @param len     Receives the message's length in bytes; untouched when there
 *                is no failure.
 *
 * @return ::lh_bool_true when there is a failure, ::lh_bool_false otherwise.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_js_lexer_get_error(const lh_js_lexer_t *self, lh_str_ptr *message, lh_usize_t *len);

/**
 * @brief Record a failure on @p self (level 2).
 *
 * The message is copied into the lexer's own buffer, so it stays as long as
 * the lexer does; a message longer than
 * ::LH_JS_LEXER_ERROR_MESSAGE_MAX is cut short. The failure is placed where
 * the scanner is.
 *
 * @param self    Lexer to write.
 * @param message The message of the failure.
 * @param len     The message's length, in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_set_error(lh_js_lexer_t *self, const lh_str_ptr message, lh_usize_t len);

/**
 * @brief Forget the last failure of @p self (level 2).
 *
 * @param self Lexer to write.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_js_lexer_clear_error(lh_js_lexer_t *self);

/**
 * @brief Scan the next token of @p self into it (level 3).
 *
 * Whitespace and comments are skipped; a line terminator inside a block
 * comment still moves the line on. The failure state is cleared before the
 * scan, so a failure is the one of the last scan.
 *
 * @param self Lexer to scan.
 *
 * @return ::lh_bool_true when a token was scanned, ::lh_bool_false at the end
 *         of the text and on a malformed token — the failure state says
 *         which.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_js_lexer_next(lh_js_lexer_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JS_LEXER_H */
