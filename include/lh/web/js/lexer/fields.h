/**
 * @file fields.h
 * @brief Member fields of ::lh_web_js_lexer_t.
 */

#ifndef LH_WEB_JS_LEXER_FIELDS_H
#define LH_WEB_JS_LEXER_FIELDS_H

/**
 * @brief How many bytes the error message buffer holds, the NUL included.
 */
#define LH_WEB_JS_LEXER_ERROR_MESSAGE_MAX 64U

/**
 * @def lh_web_js_lexer_fields(cursor_type, size_type, token_type, message_type, buffer_type)
 * @brief The scanner over one JavaScript source text:
 *
 * - `cursor` — the borrowed source text and current byte position;
 * - `token` — the token just scanned;
 * - `line` — the line the scanner is on, counting from `1`;
 * - `column` — the column the scanner is on, counting from `1`;
 * - `error_message` — the last failure's message (the lexer's own buffer);
 * - `error_message_len` — its length in bytes; `0` when there is no failure;
 * - `error_line` — the line the failure was found on;
 * - `error_column` — the column the failure was found on;
 * - `error_buffer` — where the message is kept.
 *
 * @param cursor_type  Type of `cursor` (::lh_str_cursor_t).
 * @param size_type    Type of the sizes (::lh_usize_t).
 * @param token_type   Type of `token` (::lh_web_js_token_t).
 * @param message_type Type of `error_message` (::lh_str_ptr).
 * @param buffer_type  Type of `error_buffer` (::lh_char_t).
 */
#define lh_web_js_lexer_fields(cursor_type, size_type, token_type, message_type, buffer_type)      \
    cursor_type cursor;                                                                            \
    token_type token;                                                                              \
    size_type line;                                                                                \
    size_type column;                                                                              \
    message_type error_message;                                                                    \
    size_type error_message_len;                                                                   \
    size_type error_line;                                                                          \
    size_type error_column;                                                                        \
    buffer_type error_buffer[LH_WEB_JS_LEXER_ERROR_MESSAGE_MAX]

#endif /* LH_WEB_JS_LEXER_FIELDS_H */
