#include <lh/os/system/name.h>
#include <lh/core/assert.h>

lh_void
lh_os_system_get_name(lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_str_view_lit("win32");
}
