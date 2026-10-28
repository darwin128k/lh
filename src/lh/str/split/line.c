#include <lh/str/split/line.h>
#include <lh/assert.h>
#include <lh/char/map.h>

/* Every character that ends a line; `\r\n` is resolved after the hit. */
static const lh_char_t m_str_split_line_ends[] = {lh_char_map_cr, lh_char_map_lf};

lh_bool_t
lh_str_ptr_split_next_line(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *pos,
                           lh_str_cptr *line, lh_usize_t *line_size)
{
    lh_usize_t start;
    lh_str_cptr begin;
    lh_usize_t rest;
    lh_str_cptr end;
    lh_usize_t size;
    lh_usize_t next;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(line);
    lh_assert_runtime_ref(line_size);

    start = lh_ptr_deref(pos);

    if (lh_math_ge(start, str_size))
    {
        return lh_bool_false;
    }

    begin = lh_ptr_add_by_offset_unsafe(const lh_char_t, str, start);
    rest = lh_math_sub(str_size, start);
    end = lh_str_ptr_find_of_chars(begin, rest, m_str_split_line_ends,
                                   lh_array_ptr_get_size(m_str_split_line_ends));

    lh_ptr_deref(line) = begin;
    if (lh_ptr_is_null(end))
    {
        lh_ptr_deref(line_size) = rest;
        lh_ptr_deref(pos) = str_size;
        return lh_bool_true;
    }

    size = lh_ptr_udiff(end, begin);
    next = lh_math_add_one(lh_math_add(start, size));

    if (lh_math_eq(*end, lh_char_map_cr) && lh_math_lt(next, str_size) &&
        lh_math_eq(str[next], lh_char_map_lf))
    {
        next = lh_math_add_one(next);
    }
    lh_ptr_deref(line_size) = size;
    lh_ptr_deref(pos) = next;
    return lh_bool_true;
}

lh_bool_t
lh_str_view_split_next_line(const lh_str_view_t *self, lh_usize_t *pos, lh_str_view_t *line)
{
    lh_str_cptr piece;
    lh_usize_t piece_size;

    lh_assert_runtime_ref(line);
    if (lh_str_view_is_empty(self))
    {
        return lh_bool_false;
    }

    if (!lh_str_ptr_split_next_line(lh_str_view_get_data(self), lh_str_view_get_size(self), pos,
                                    lh_addr_of(piece), lh_addr_of(piece_size)))
    {
        return lh_bool_false;
    }

    if (lh_math_is_zero(piece_size))
    {
        lh_str_view_init_empty(line);
    }
    else
    {
        lh_str_init_by_size(line, piece, piece_size);
    }
    return lh_bool_true;
}
