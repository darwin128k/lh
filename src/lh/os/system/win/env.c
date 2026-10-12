#include <lh/os/system/env.h>
#include <lh/assert.h>

#include <stdlib.h>

lh_str_cptr
lh_os_system_env_get(lh_str_cptr name)
{
    lh_assert_runtime_ref(name);
    return getenv(name);
}