#include <lh/net/socket/addr.h>
#include <lh/assert.h>

lh_net_socket_addr_family_t
lh_net_socket_addr_get_family(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->family;
}

lh_void
lh_net_socket_addr_set_family(lh_net_socket_addr_t *self, lh_net_socket_addr_family_t family)
{
    lh_assert_runtime_ref(self);
    self->family = family;
}

const lh_net_ip4_socket_addr_t *
lh_net_socket_addr_get_as_ip4(const lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->as.ip4);
}

lh_net_ip4_socket_addr_t *
lh_net_socket_addr_get_as_ip4_mut(lh_net_socket_addr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->as.ip4);
}

lh_void
lh_net_socket_addr_init_ip4(lh_net_socket_addr_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(addr);
    lh_net_socket_addr_set_family(self, lh_net_socket_addr_family_ip4);
    lh_ptr_deref(lh_net_socket_addr_get_as_ip4_mut(self)) = lh_ptr_deref(addr);
}

lh_void
lh_net_socket_addr_get_ip4(const lh_net_socket_addr_t *self, lh_net_ip4_socket_addr_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(lh_net_socket_addr_get_family(self) != lh_net_socket_addr_family_ip4,
                         lh_runtime_error_code_invalid_argument);
    lh_ptr_deref(out) = lh_ptr_deref(lh_net_socket_addr_get_as_ip4(self));
}

lh_usize_t
lh_net_socket_addr_format(const lh_net_socket_addr_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    switch (lh_net_socket_addr_get_family(self))
    {
    case lh_net_socket_addr_family_ip4:
        return lh_net_ip4_socket_addr_format(lh_net_socket_addr_get_as_ip4(self), str, str_size);
    default:
        return 0; /* unknown family — not a validly-constructed lh_net_socket_addr_t */
    }
}

lh_bool_t
lh_net_socket_addr_equals(const lh_net_socket_addr_t *self, const lh_net_socket_addr_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    if (lh_net_socket_addr_get_family(self) != lh_net_socket_addr_get_family(other))
    {
        return lh_bool_false;
    }

    switch (lh_net_socket_addr_get_family(self))
    {
    case lh_net_socket_addr_family_ip4:
        return lh_net_ip4_socket_addr_equals(lh_net_socket_addr_get_as_ip4(self), lh_net_socket_addr_get_as_ip4(other));
    default:
        return lh_bool_false;
    }
}
