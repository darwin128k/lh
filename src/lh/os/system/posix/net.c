#include <lh/os/system/net.h>

lh_bool_t
lh_os_system_net_init(void)
{
    return lh_bool_true; /* POSIX sockets need no process-wide setup. */
}

void
lh_os_system_net_deinit(void)
{
}
