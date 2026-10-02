#include <lh/os/system/str.h>
#include <lh/assert.h>

/* POSIX OS text already is UTF-8 bytes: both directions are a copy. */

lh_bool_t
lh_os_system_str_from_utf8(lh_os_str_t *out, lh_str_cptr text)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(text);
    lh_str_assign_view(out, lh_str_view_make(text));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_str_to_utf8(lh_os_str_cptr text, lh_usize_t count, lh_str_t *out)
{
    lh_assert_runtime_ref(text);
    lh_assert_runtime_ref(out);
    lh_str_clear(out);
    lh_str_append(out, text, count);
    return lh_bool_true;
}
