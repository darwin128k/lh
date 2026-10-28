#include <lh/str/split/next.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_bool_t
lh_str_ptr_split_next_of(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr delims,
                         lh_usize_t delim_count, lh_usize_t *pos, lh_str_cptr *field,
                         lh_usize_t *field_size, lh_bool_t *had_delim)
{
    lh_usize_t start;
    lh_usize_t rest;
    lh_str_cptr from;
    lh_str_cptr delim_at;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(field);
    lh_assert_runtime_ref(field_size);
    lh_assert_runtime_ref(had_delim);

    start = lh_ptr_deref(pos);
    if (lh_math_ge(start, str_size))
    {
        return lh_bool_false;
    }
    rest = lh_math_sub(str_size, start);
    from = lh_ptr_add_by_offset_unsafe(const lh_char_t, str, start);
    delim_at = lh_str_ptr_find_of_chars(from, rest, delims, delim_count);

    lh_ptr_deref(field) = from;
    if (lh_ptr_is_set(delim_at))
    {
        lh_ptr_deref(field_size) = lh_ptr_udiff(delim_at, from);
        lh_ptr_deref(had_delim) = lh_bool_true;
        lh_ptr_deref(pos) = lh_math_add_one(lh_math_add(start, lh_ptr_deref(field_size)));
    }
    else
    {
        lh_ptr_deref(field_size) = rest;
        lh_ptr_deref(had_delim) = lh_bool_false;
        lh_ptr_deref(pos) = str_size;
    }
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_split_next(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim, lh_usize_t *pos,
                      lh_str_cptr *field, lh_usize_t *field_size, lh_bool_t *had_delim)
{
    return lh_str_ptr_split_next_of(str, str_size, lh_addr_of(delim), 1U, pos, field, field_size,
                                    had_delim);
}

lh_bool_t
lh_str_view_split_next_of(const lh_str_view_t *self, lh_str_cptr delims, lh_usize_t delim_count,
                          lh_usize_t *pos, lh_str_view_t *field, lh_bool_t *had_delim)
{
    lh_str_cptr data;
    lh_usize_t size;
    lh_str_cptr piece;
    lh_usize_t piece_size;

    lh_assert_runtime_ref(field);
    if (lh_str_view_is_empty(self))
    {
        return lh_bool_false;
    }

    data = lh_str_view_get_data(self);
    size = lh_str_view_get_size(self);
    if (!lh_str_ptr_split_next_of(data, size, delims, delim_count, pos, lh_addr_of(piece),
                                  lh_addr_of(piece_size), had_delim))
    {
        return lh_bool_false;
    }

    if (lh_math_is_zero(piece_size))
    {
        lh_str_view_init_empty(field);
    }
    else
    {
        lh_str_init_by_size(field, piece, piece_size);
    }
    return lh_bool_true;
}
