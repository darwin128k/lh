#include <lh/os/system/net/interface.h>
#include <lh/assert.h>
#include <lh/os/alloc.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/iphlpapi.h>
#include <lh/runtime/allocator.h>
#include <lh/math.h>

/* The table can grow between the size query and the call: a few tries. */
#define LH_OS_SYSTEM_NET_INTERFACE_TRIES 3U

lh_bool_t
lh_os_system_net_interface_each_ip4(lh_os_net_interface_ip4_fn *fn, lh_ptr user)
{
    lh_os_system_win_mib_ipaddrtable_t *table = lh_null;
    lh_os_system_win_dword_t size = 0UL;
    lh_os_system_win_dword_t result = LH_OS_SYSTEM_WIN_ERROR_INSUFFICIENT_BUFFER;
    lh_usize_t i;

    lh_assert_runtime_ref(fn);
    for (i = 0U; i < LH_OS_SYSTEM_NET_INTERFACE_TRIES &&
                 lh_math_eq(result, LH_OS_SYSTEM_WIN_ERROR_INSUFFICIENT_BUFFER);
         ++i)
    {
        if (lh_null_ne(table))
        {
            lh_runtime_allocator_free(table);
            table = lh_null;
        }
        if (lh_math_ne(size, 0UL))
        {
            table = lh_ptr_cast(lh_os_system_win_mib_ipaddrtable_t, lh_os_alloc((lh_usize_t)size));
            if (lh_null_eq(table))
            {
                return lh_bool_false;
            }
        }
        result = GetIpAddrTable(table, lh_addr_of(size), 0);
    }
    if (lh_math_ne(result, 0UL) || lh_null_eq(table))
    {
        if (lh_null_ne(table))
        {
            lh_runtime_allocator_free(table);
        }
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    for (i = 0U; i < (lh_usize_t)table->dwNumEntries; ++i)
    {
        lh_net_ip4_t ip;
        /* dwAddr is in network order: its bytes are the octets as written. */
        const lh_u8_t *const octets = (const lh_u8_t *)lh_addr_of(table->table[i].dwAddr);
        lh_net_ip4_init(lh_addr_of(ip), octets[0], octets[1], octets[2], octets[3]);
        fn(user, lh_addr_of(ip));
    }
    lh_runtime_allocator_free(table);
    return lh_bool_true;
}
