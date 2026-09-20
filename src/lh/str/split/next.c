#include <lh/str/split/next.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>

lh_bool_t
lh_str_ptr_split_next_of(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr delims,
                         lh_usize_t delim_count, lh_usize_t *pos, lh_str_cptr *field,
                         lh_usize_t *field_size, lh_bool_t *had_delim)
{
    lh_usize_t start;
    lh_usize_t rest;
    lh_usize_t i;
    lh_str_cptr delim_at;
    lh_str_cptr found;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(delims);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(field);
    lh_assert_runtime_ref(field_size);
    lh_assert_runtime_ref(had_delim);

    start = *pos;
    if (start >= str_size)
    {
        return lh_bool_false;
    }

    rest = str_size - start;
    delim_at = lh_null;
    for (i = 0U; i < delim_count; ++i)
    {
        found = lh_memory_find(str + start, rest, delims + i, LH_CHAR_T_SIZE);
        if (found != lh_null && (delim_at == lh_null || found < delim_at))
        {
            delim_at = found;
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
        *field_size = rest;
        *had_delim = lh_bool_false;
        *pos = str_size;
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
