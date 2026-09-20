#include <lh/str/split/next.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/str/ptr.h>

lh_bool_t
lh_str_ptr_split_next(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim, lh_usize_t *pos,
                      lh_str_cptr *field, lh_usize_t *field_size, lh_bool_t *had_delim)
{
    lh_usize_t start;
    lh_str_cptr delim_at;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(field);
    lh_assert_runtime_ref(field_size);
    lh_assert_runtime_ref(had_delim);

    start = *pos;
    if (start >= str_size)
    {
        return lh_bool_false;
    }

    delim_at = lh_str_ptr_find_of_char(str + start, str_size - start, delim);
    *field = str + start;
    if (delim_at != lh_null)
    {
        *field_size = lh_cast_static(lh_usize_t, delim_at - (str + start));
        *had_delim = lh_bool_true;
        *pos = start + *field_size + 1U;
    }
    else
    {
        *field_size = str_size - start;
        *had_delim = lh_bool_false;
        *pos = str_size;
    }
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_split_next_if(lh_str_cptr str, lh_usize_t str_size, lh_str_ptr_split_is_delim_t is_delim,
                         lh_usize_t *pos, lh_str_cptr *field, lh_usize_t *field_size,
                         lh_bool_t *had_delim)
{
    lh_usize_t start;
    lh_usize_t i;
    lh_str_cptr delim_at;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(is_delim);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(field);
    lh_assert_runtime_ref(field_size);
    lh_assert_runtime_ref(had_delim);

    start = *pos;
    if (start >= str_size)
    {
        return lh_bool_false;
    }

    delim_at = lh_null;
    for (i = start; i < str_size; ++i)
    {
        if (is_delim(str[i]))
        {
            delim_at = str + i;
            break;
        }
    }

    *field = str + start;
    if (delim_at != lh_null)
    {
        *field_size = lh_cast_static(lh_usize_t, delim_at - (str + start));
        *had_delim = lh_bool_true;
        *pos = start + *field_size + 1U;
    }
    else
    {
        *field_size = str_size - start;
        *had_delim = lh_bool_false;
        *pos = str_size;
    }
    return lh_bool_true;
}

lh_bool_t
lh_str_view_split_next_if(const lh_str_view_t *self, lh_str_ptr_split_is_delim_t is_delim,
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
    if (!lh_str_ptr_split_next_if(data, size, is_delim, pos, lh_addr_of(piece),
                                  lh_addr_of(piece_size), had_delim))
    {
        return lh_bool_false;
    }

    if (piece_size == 0U)
    {
        lh_str_view_init_empty(field);
    }
    else
    {
        lh_str_init_by_size(field, piece, piece_size);
    }
    return lh_bool_true;
}
