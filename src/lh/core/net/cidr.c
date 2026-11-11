/**
 * @file cidr.c
 * @brief IPv4 blocks: building, reading and matching.
 */

#include <lh/core/net/cidr.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/char/map.h>
#include <lh/core/math.h>
#include <lh/core/str/parse/uint.h>
#include <lh/core/str/split/once.h>

lh_u32_t
lh_net_cidr_get_net(const lh_net_cidr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->net;
}

lh_void
lh_net_cidr_set_net(lh_net_cidr_t *self, lh_u32_t net)
{
    lh_assert_runtime_ref(self);
    self->net = net;
}

lh_u32_t
lh_net_cidr_get_mask(const lh_net_cidr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mask;
}

lh_void
lh_net_cidr_set_mask(lh_net_cidr_t *self, lh_u32_t mask)
{
    lh_assert_runtime_ref(self);
    self->mask = mask;
}

lh_void
lh_net_cidr_init(lh_net_cidr_t *self, const lh_net_ip4_t *address, lh_uint_t bits)
{
    /* A shift by 32 is undefined: no bits is the empty mask, spelled out. */
    const lh_u32_t mask =
        lh_math_is_zero(bits)
            ? 0U
            : lh_cast_static(lh_u32_t, 0xFFFFFFFFU << (32U - lh_math_min(bits, 32U)));

    lh_net_cidr_set_mask(self, mask);
    lh_net_cidr_set_net(self, lh_net_ip4_to_u32(address) & mask);
}

lh_bool_t
lh_net_cidr_contains(const lh_net_cidr_t *self, const lh_net_ip4_t *address)
{
    return lh_math_eq(lh_net_ip4_to_u32(address) & lh_net_cidr_get_mask(self),
                      lh_net_cidr_get_net(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_net_cidr_try_parse(const lh_str_ptr text, lh_usize_t text_size, lh_net_cidr_t *out)
{
    lh_assert_runtime_ref(out);

    lh_str_view_t whole;

    lh_str_view_init_by_size(lh_addr_of(whole), text, text_size);

    lh_str_view_t address_text;
    lh_str_view_t bits_text;
    lh_uint_t bits = 32U;

    if (lh_str_view_split_once(lh_addr_of(whole), lh_char_map_slash, lh_addr_of(address_text),
                               lh_addr_of(bits_text)))
    {
        if (!lh_str_ptr_parse_uint(lh_str_view_get_data(lh_addr_of(bits_text)),
                                   lh_str_view_get_size(lh_addr_of(bits_text)), 32U,
                                   lh_addr_of(bits)))
        {
            return lh_bool_false;
        }
    }
    else
    {
        address_text = whole;
    }

    lh_net_ip4_t address;

    if (lh_math_is_zero(lh_str_view_get_size(lh_addr_of(address_text))) ||
        !lh_net_ip4_parse(lh_str_view_get_data(lh_addr_of(address_text)),
                          lh_str_view_get_size(lh_addr_of(address_text)), lh_addr_of(address)))
    {
        return lh_bool_false;
    }
    lh_net_cidr_init(out, lh_addr_of(address), bits);
    return lh_bool_true;
}

lh_bool_t
lh_net_cidr_list_contains(const lh_net_cidr_t *list, lh_usize_t count, const lh_net_ip4_t *address)
{
    for (lh_usize_t i = 0U; lh_math_lt(i, count); ++i)
    {
        if (lh_net_cidr_contains(list + i, address))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}
