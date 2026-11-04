#include <lh/core/str/split/once.h>
#include <lh/core/assert.h>

lh_bool_t
lh_str_view_split_once(const lh_str_view_t *self, lh_char_t delim, lh_str_view_t *head,
                       lh_str_view_t *tail)
{
    lh_usize_t at;

    lh_assert_runtime_ref(head);
    lh_assert_runtime_ref(tail);
    if (lh_str_view_is_empty(self))
    {
        return lh_bool_false;
    }

    at = lh_str_view_find_char(self, delim);

    if (lh_math_eq(at, LH_STR_VIEW_INVALID))
    {
        return lh_bool_false;
    }
    lh_str_view_from_offset(self, 0U, at, head);
    lh_str_view_tail(self, lh_math_add_one(at), tail);
    return lh_bool_true;
}
