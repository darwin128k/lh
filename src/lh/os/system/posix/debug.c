#include <lh/os/system/debug.h>
#include <lh/core/assert.h>

/* POSIX has no debugger output channel: a debugger reads stderr, which the host
   wires to the runtime logger itself. */
lh_bool_t
lh_os_system_debug_print(lh_str_cptr text)
{
    lh_assert_runtime_ref(text);
    return lh_bool_false;
}
