#include <lh/json/minify.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/json/reader.h>
#include <lh/math/ops.h>
#include <lh/null.h>
#include <lh/runtime/error.h>

lh_bool_t
lh_json_minify_is_at(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos, lh_char_t first,
                     lh_char_t second)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(lh_math_add_one(pos), size) &&
                                         lh_math_eq(text[pos], first) &&
                                         lh_math_eq(text[lh_math_add_one(pos)], second));
}

lh_usize_t
lh_json_minify_skip_line_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos)
{
    /* Up to the line feed, which is whitespace and goes next. */
    while (lh_math_lt(pos, size) && lh_math_ne(text[pos], lh_char_map_lf))
    {
        pos = lh_math_add_one(pos);
    }
    return pos;
}

lh_usize_t
lh_json_minify_skip_block_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos)
{
    pos = lh_math_add(pos, 2U);
    while (lh_math_lt(pos, size) &&
           !lh_json_minify_is_at(text, size, pos, lh_char_map_asterisk, lh_char_map_slash))
    {
        pos = lh_math_add_one(pos);
    }
    return lh_math_min(lh_math_add(pos, 2U), size);
}

lh_void
lh_json_minify_copy_string(lh_str_ptr text, lh_usize_t size, lh_usize_t *read, lh_usize_t *write)
{
    lh_usize_t from = lh_ptr_deref(read);
    lh_usize_t to = lh_ptr_deref(write);
    lh_bool_t escaped = lh_bool_false;

    /* The opening quote, then everything up to an unescaped closing one. */
    text[to] = text[from];
    to = lh_math_add_one(to);
    from = lh_math_add_one(from);
    while (lh_math_lt(from, size))
    {
        const lh_char_t ch = text[from];

        text[to] = ch;
        to = lh_math_add_one(to);
        from = lh_math_add_one(from);
        if (!escaped && lh_math_eq(ch, lh_char_map_dquot))
        {
            break;
        }
        escaped = lh_cast_static(lh_bool_t, !escaped && lh_math_eq(ch, lh_char_map_backslash));
    }
    lh_ptr_deref(read) = from;
    lh_ptr_deref(write) = to;
}

lh_usize_t
lh_json_minify(lh_str_ptr text, lh_usize_t size)
{
    lh_usize_t read = 0U;
    lh_usize_t write = 0U;

    lh_assert_runtime_if(lh_null_eq(text) && !lh_math_is_zero(size),
                         lh_runtime_error_code_null_pointer);

    while (lh_math_lt(read, size))
    {
        if (lh_json_reader_is_whitespace(text[read]))
        {
            read = lh_math_add_one(read);
        }
        else if (lh_json_minify_is_at(text, size, read, lh_char_map_slash, lh_char_map_slash))
        {
            read = lh_json_minify_skip_line_comment(text, size, read);
        }
        else if (lh_json_minify_is_at(text, size, read, lh_char_map_slash, lh_char_map_asterisk))
        {
            read = lh_json_minify_skip_block_comment(text, size, read);
        }
        else if (lh_math_eq(text[read], lh_char_map_dquot))
        {
            lh_json_minify_copy_string(text, size, lh_addr_of(read), lh_addr_of(write));
        }
        else
        {
            text[write] = text[read];
            write = lh_math_add_one(write);
            read = lh_math_add_one(read);
        }
    }
    return write;
}
