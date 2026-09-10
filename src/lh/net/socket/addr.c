#include <lh/net/socket/addr.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/str/format/text.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

lh_net_socket_addr_t
lh_net_socket_addr_make(const lh_net_ip4_t *ip, lh_net_port_t port)
{
    lh_net_socket_addr_t result;
    lh_net_socket_addr_set(lh_addr_of(result), ip, port);
    return result;
}

void
lh_net_socket_addr_set(lh_net_socket_addr_t *self, const lh_net_ip4_t *ip, lh_net_port_t port)
{
    lh_assert_runtime_ref(self);

    lh_net_socket_addr_set_ip(self, ip);
    lh_net_socket_addr_set_port(self, port);
}

void
lh_net_socket_addr_assign(lh_net_socket_addr_t *self, const lh_net_socket_addr_t *other)
{
    lh_net_ip4_t ip;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    ip = lh_net_socket_addr_get_ip(other);
    lh_net_socket_addr_set(self, lh_addr_of(ip), lh_net_socket_addr_get_port(other));
}

lh_net_ip4_t
lh_net_socket_addr_get_ip(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ip;
}

void
lh_net_socket_addr_set_ip(lh_net_socket_addr_t *self, const lh_net_ip4_t *ip)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(ip);
    self->ip = lh_ptr_deref(ip);
}

lh_net_port_t
lh_net_socket_addr_get_port(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->port;
}

void
lh_net_socket_addr_set_port(lh_net_socket_addr_t *self, lh_net_port_t port)
{
    lh_assert_runtime_ref(self);
    self->port = port;
}

lh_bool_t
lh_net_socket_addr_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_socket_addr_t *out)
{
    lh_net_ip4_t ip;
    lh_net_port_t port;
    lh_str_cptr colon;
    lh_usize_t ip_len;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    colon = lh_str_ptr_find_of_char(str, str_size, ':');
    if (colon == lh_null)
    {
        return lh_bool_false;
    }
    ip_len = (lh_usize_t)(colon - str);

    if (!lh_net_ip4_parse(str, ip_len, lh_addr_of(ip)))
    {
        return lh_bool_false;
    }
    if (!lh_net_port_parse(colon + 1, str_size - ip_len - 1, lh_addr_of(port)))
    {
        return lh_bool_false;
    }

    lh_net_socket_addr_set(out, lh_addr_of(ip), port);
    return lh_bool_true;
}

lh_usize_t
lh_net_socket_addr_format(const lh_net_socket_addr_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_net_ip4_t ip;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    ip = lh_net_socket_addr_get_ip(self);

    /* Octets/port are narrower than int; variadic default promotion takes them to plain int,
     * not lh_uint_t (the type %u reads via va_arg) — cast each one explicitly. */
    return lh_str_ptr_format_text(
        str, str_size, "%u.%u.%u.%u:%u",
        (lh_uint_t)lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_0),
        (lh_uint_t)lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_1),
        (lh_uint_t)lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_2),
        (lh_uint_t)lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_3),
        (lh_uint_t)lh_net_socket_addr_get_port(self));
}

lh_bool_t
lh_net_socket_addr_equals(const lh_net_socket_addr_t *self, const lh_net_socket_addr_t *other)
{
    lh_net_ip4_t self_ip;
    lh_net_ip4_t other_ip;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    self_ip = lh_net_socket_addr_get_ip(self);
    other_ip = lh_net_socket_addr_get_ip(other);
    if (!lh_net_ip4_equals(lh_addr_of(self_ip), lh_addr_of(other_ip)))
    {
        return lh_bool_false;
    }
    return lh_net_socket_addr_get_port(self) == lh_net_socket_addr_get_port(other) ? lh_bool_true
                                                                                    : lh_bool_false;
}
