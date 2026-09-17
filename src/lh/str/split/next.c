#include <lh/str/split/next.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
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
