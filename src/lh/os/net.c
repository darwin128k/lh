#include <lh/os/net.h>
#include <lh/compiler/os.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <winsock2.h>
#endif

lh_bool_t
lh_os_net_init(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), lh_addr_of(wsa_data)) == 0 ? lh_bool_true : lh_bool_false;
#else
    return lh_bool_true; /* POSIX sockets need no process-wide setup. */
#endif
}

void
lh_os_net_deinit(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    WSACleanup();
#endif
}
