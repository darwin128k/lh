#include <lh/os/net.h>
#include <lh/os/system/net.h>

lh_bool_t
lh_os_net_init(void)
{
    return lh_os_system_net_init();
}

void
lh_os_net_deinit(void)
{
    lh_os_system_net_deinit();
}
