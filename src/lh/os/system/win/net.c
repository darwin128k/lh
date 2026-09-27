#include <lh/os/system/net.h>
#include <lh/cast/static.h>
#include <lh/os/system/win/ws2_32.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

lh_bool_t
lh_os_system_net_init(void)
{
    lh_os_system_win_wsadata_t wsa_data;

    return lh_cast_static(
        lh_bool_t,
        lh_math_is_zero(WSAStartup(LH_OS_SYSTEM_WIN_WINSOCK_VERSION_2_2, lh_addr_of(wsa_data))));
}

void
lh_os_system_net_deinit(void)
{
    (void)WSACleanup();
}
