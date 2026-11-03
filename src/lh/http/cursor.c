/**
 * @file cursor.c
 * @brief HTTP syntax scanning on a string cursor.
 */

#include <lh/http/cursor.h>
#include <lh/char/map.h>
#include <lh/assert.h>
#include <lh/http/char.h>

lh_usize_t
lh_http_cursor_skip_ows(lh_str_cursor_t *self)
{
    const lh_char_t chars[] = {lh_char_map_space, lh_char_map_ht};
    const lh_usize_t start = lh_str_cursor_get_pos(self);
    while (
        !lh_str_cursor_is_end(self) &&
        lh_str_ptr_contains_char(chars, lh_array_ptr_get_size(chars), lh_str_cursor_get_char(self)))
    {
        lh_str_cursor_advance(self, 1U);
    }
    return lh_math_sub(lh_str_cursor_get_pos(self), start);
}

lh_usize_t
lh_http_cursor_skip_token(lh_str_cursor_t *self)
{
    const lh_usize_t start = lh_str_cursor_get_pos(self);
    while (!lh_str_cursor_is_end(self) && lh_http_is_token_char(lh_str_cursor_get_char(self)))
    {
        lh_str_cursor_advance(self, 1U);
    }
    return lh_math_sub(lh_str_cursor_get_pos(self), start);
}

lh_bool_t
lh_http_cursor_try_skip_quoted(lh_str_cursor_t *self)
{
    if (!lh_str_cursor_skip_char(self, lh_char_map_dquot))
    {
        return lh_bool_false;
    }
    while (!lh_str_cursor_is_end(self) &&
           lh_math_ne(lh_str_cursor_get_char(self), lh_char_map_dquot))
    {
        if (lh_str_cursor_skip_char(self, lh_char_map_backslash) && lh_str_cursor_is_end(self))
        {
            return lh_bool_false;
        }
        if (!lh_http_is_field_char(lh_str_cursor_get_char(self)))
        {
            return lh_bool_false;
        }
        lh_str_cursor_advance(self, 1U);
    }
    return lh_str_cursor_skip_char(self, lh_char_map_dquot);
}

lh_void
lh_http_cursor_skip_quoted(lh_str_cursor_t *self)
{
    lh_runtime_check_ifn(lh_http_cursor_try_skip_quoted(self), lh_runtime_error_code_invalid_range);
}
