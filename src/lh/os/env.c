#include <lh/os/env.h>
#include <lh/os/system/env.h>

lh_str_cptr
lh_os_env_get(lh_str_cptr name)
{
    if (lh_null_eq(name))
    {
        return lh_null;
    }
    return lh_os_system_env_get(name);
}