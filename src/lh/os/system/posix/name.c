#include <lh/os/system/name.h>
#include <lh/core/assert.h>

lh_void
lh_os_system_get_name(lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
#if LH_COMPILER_OS == LH_COMPILER_OS_LINUX
    lh_ptr_deref(out) = lh_str_view_lit("linux");
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
    lh_ptr_deref(out) = lh_str_view_lit("mac");
#else
    lh_ptr_deref(out) = lh_str_view_lit("posix");
#endif
}
