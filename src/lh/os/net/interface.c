#include <lh/os/net/interface.h>
#include <lh/os/system/net/interface.h>
#include <lh/null.h>

lh_bool_t
lh_os_net_interface_each_ip4(lh_os_net_interface_ip4_fn *fn, lh_ptr user)
{
    if (lh_null_eq(fn))
    {
        return lh_bool_false;
    }
    return lh_os_system_net_interface_each_ip4(fn, user);
}
