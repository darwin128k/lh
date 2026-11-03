/**
 * @file cursor.h
 * @brief HTTP token and optional whitespace scanning on a borrowed text cursor.
 *
 * These operations advance a caller-owned string cursor over HTTP syntax;
 * they neither allocate nor own the input. They are shared by protocol
 * parsers such as chunked transfer coding.
 */

#ifndef LH_HTTP_CURSOR_H
#define LH_HTTP_CURSOR_H

#include <lh/str/cursor.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Skip HTTP optional whitespace (space and horizontal tab).
 * @param self Cursor over valid borrowed text, advanced past the whitespace.
 * @return The number of bytes skipped. Nothing is allocated or owned.
 * @pre The cursor is valid and its position is within its text.
 */
lh_usize_t
lh_http_cursor_skip_ows(lh_str_cursor_t *self);

/**
 * @brief Skip a run of HTTP token characters.
 * @param self Cursor over valid borrowed text, advanced past the token.
 * @return The number of bytes skipped, or zero if no token starts here.
 * @pre The cursor is valid and its position is within its text.
 */
lh_usize_t
lh_http_cursor_skip_token(lh_str_cursor_t *self);

/**
 * @brief Try to skip a quoted HTTP string including its quotes.
 *
 * Validates field bytes and quoted pairs. The cursor is borrowed and may be
 * advanced on failure; it owns no text.
 * @param self Cursor over valid borrowed text.
 * @return True when a complete quoted string was skipped, false otherwise.
 * @pre The cursor is valid and its position is within its text.
 */
lh_bool_t
lh_http_cursor_try_skip_quoted(lh_str_cursor_t *self);

/**
 * @brief Skip a quoted string and check ::lh_http_cursor_try_skip_quoted.
 * @param self Borrowed cursor with a valid quoted string at its position.
 * @fails ::lh_runtime_error_code_invalid_range The string is invalid or incomplete.
 */
lh_void
lh_http_cursor_skip_quoted(lh_str_cursor_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_CURSOR_H */
