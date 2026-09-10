#include <lh/net/port.h>
#include <lh/assert.h>
#include <lh/str/parse/uint.h>
#include <lh/util/addr.h>

lh_bool_t
lh_net_port_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_port_t *out)
{
    lh_uint_t value;

    lh_assert_runtime_ref(out);

    if (!lh_str_ptr_parse_uint(str, str_size, LH_NET_PORT_MAX, lh_addr_of(value)))
    {
        return lh_bool_false;
    }

    *out = (lh_net_port_t)value;
    return lh_bool_true;
}
