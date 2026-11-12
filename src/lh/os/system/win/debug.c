#include <lh/os/system/debug.h>
#include <lh/core/assert.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/kernel32.h>

lh_bool_t
lh_os_system_debug_print(lh_str_cptr text)
{
    lh_assert_runtime_ref(text);
    lh_os_str_t wide;
    if (!lh_os_system_str_init_by_utf8(lh_addr_of(wide), text))
    {
        return lh_bool_false;
    }
    OutputDebugStringW(lh_os_str_get_data(lh_addr_of(wide)));
    lh_os_str_deinit(lh_addr_of(wide));
    return lh_bool_true;
}
