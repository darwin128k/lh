#include <lh/os/system/net/interface.h>
#include <lh/assert.h>
#include <lh/os/system/error/capture.h>
#include <lh/math.h>

#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>

lh_bool_t
lh_os_system_net_interface_each_ip4(lh_os_net_interface_ip4_fn *fn, lh_ptr user)
{
    struct ifaddrs *list = lh_null;
    const struct ifaddrs *it;

    lh_assert_runtime_ref(fn);
    if (lh_math_ne(getifaddrs(lh_addr_of(list)), 0))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    for (it = list; lh_null_ne(it); it = it->ifa_next)
    {
        lh_net_ip4_t ip;

        if (lh_null_eq(it->ifa_addr) || it->ifa_addr->sa_family != AF_INET)
        {
            continue;
        }
        /* s_addr is in network order: its bytes are the octets as written. */
        const lh_u8_t *const octets =
            (const lh_u8_t *)lh_addr_of(((const struct sockaddr_in *)it->ifa_addr)->sin_addr);
        lh_net_ip4_init(lh_addr_of(ip), octets[0], octets[1], octets[2], octets[3]);
        fn(user, lh_addr_of(ip));
    }
    freeifaddrs(list);
    return lh_bool_true;
}
