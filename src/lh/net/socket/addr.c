#include <lh/net/socket/addr.h>
#include <lh/assert.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_net_socket_addr_t
lh_net_socket_addr_make_ip4(const lh_net_ip4_socket_addr_t *addr)
{
    lh_net_socket_addr_t result;

    lh_assert_runtime_ref(addr);

    result.family = lh_net_socket_addr_family_ip4;
    result.as.ip4 = lh_ptr_deref(addr);
    return result;
}

lh_net_socket_addr_family_t
lh_net_socket_addr_get_family(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->family;
}

lh_net_ip4_socket_addr_t
lh_net_socket_addr_get_ip4(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->family != lh_net_socket_addr_family_ip4,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    return self->as.ip4;
}

lh_usize_t
lh_net_socket_addr_format(const lh_net_socket_addr_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    switch (self->family)
    {
    case lh_net_socket_addr_family_ip4:
        return lh_net_ip4_socket_addr_format(lh_addr_of(self->as.ip4), str, str_size);
    default:
        return 0; /* unknown family — not a validly-constructed lh_net_socket_addr_t */
    }
}

lh_bool_t
lh_net_socket_addr_equals(const lh_net_socket_addr_t *self, const lh_net_socket_addr_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    if (self->family != other->family)
    {
        return lh_bool_false;
    }

    switch (self->family)
    {
    case lh_net_socket_addr_family_ip4:
        return lh_net_ip4_socket_addr_equals(lh_addr_of(self->as.ip4), lh_addr_of(other->as.ip4));
    default:
        return lh_bool_false;
    }
}
