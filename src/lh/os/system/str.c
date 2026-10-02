#include <lh/os/system/str.h>
#include <lh/assert.h>

/* Built on the backend's from_utf8 / to_utf8: the same on every OS. */

lh_bool_t
lh_os_system_str_init_by_utf8(lh_os_str_t *self, lh_str_cptr text)
{
    lh_os_str_init(self);
    return lh_os_system_str_from_utf8(self, text);
}

lh_bool_t
lh_os_system_str_ptr_to_utf8(lh_os_str_cptr text, lh_str_t *out)
{
    lh_assert_runtime_ref(text);
    return lh_os_system_str_to_utf8(text, lh_os_str_ptr_len(text), out);
}
