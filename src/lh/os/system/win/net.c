#include <lh/os/system/net.h>
#include <lh/cast/static.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

lh_bool_t
lh_os_system_net_init(void)
{
    WSADATA wsa_data;

    return lh_cast_static(lh_bool_t, lh_math_is_zero(WSAStartup(MAKEWORD(2, 2), lh_addr_of(wsa_data))));
}

void
lh_os_system_net_deinit(void)
{
    (void)WSACleanup();
}
