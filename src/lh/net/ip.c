#include <lh/net/ip.h>
#include <lh/assert.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/runtime/error.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

void
lh_net_ip4_pack(lh_net_ip4_t *self, const lh_u8_t *octet0, const lh_u8_t *octet1,
                const lh_u8_t *octet2, const lh_u8_t *octet3)
{
    lh_optional_ref(octet0)
    {
        self->octets[LH_NET_IP4_OCTET_INDEX_0] = lh_ptr_deref(octet0);
    }
    lh_optional_ref(octet1)
    {
        self->octets[LH_NET_IP4_OCTET_INDEX_1] = lh_ptr_deref(octet1);
    }
    lh_optional_ref(octet2)
    {
        self->octets[LH_NET_IP4_OCTET_INDEX_2] = lh_ptr_deref(octet2);
    }
    lh_optional_ref(octet3)
    {
        self->octets[LH_NET_IP4_OCTET_INDEX_3] = lh_ptr_deref(octet3);
    }
}

void
lh_net_ip4_unpack(const lh_net_ip4_t *self, lh_u8_t *octet0, lh_u8_t *octet1, lh_u8_t *octet2,
                  lh_u8_t *octet3)
{
    lh_optional_ref(octet0)
    {
        lh_ptr_deref(octet0) = self->octets[LH_NET_IP4_OCTET_INDEX_0];
    }
    lh_optional_ref(octet1)
    {
        lh_ptr_deref(octet1) = self->octets[LH_NET_IP4_OCTET_INDEX_1];
    }
    lh_optional_ref(octet2)
    {
        lh_ptr_deref(octet2) = self->octets[LH_NET_IP4_OCTET_INDEX_2];
    }
    lh_optional_ref(octet3)
    {
        lh_ptr_deref(octet3) = self->octets[LH_NET_IP4_OCTET_INDEX_3];
    }
}

lh_net_ip4_t
lh_net_ip4_make(lh_u8_t octet0, lh_u8_t octet1, lh_u8_t octet2, lh_u8_t octet3)
{
    lh_net_ip4_t result;
    lh_net_ip4_set(lh_addr_of(result), octet0, octet1, octet2, octet3);
    return result;
}

void
lh_net_ip4_set(lh_net_ip4_t *self, lh_u8_t octet0, lh_u8_t octet1, lh_u8_t octet2, lh_u8_t octet3)
{
    lh_net_ip4_pack(self, lh_addr_of(octet0), lh_addr_of(octet1), lh_addr_of(octet2),
                    lh_addr_of(octet3));
}

void
lh_net_ip4_assign(lh_net_ip4_t *self, const lh_net_ip4_t *other)
{
    lh_u8_t octet0;
    lh_u8_t octet1;
    lh_u8_t octet2;
    lh_u8_t octet3;

    lh_net_ip4_unpack(other, lh_addr_of(octet0), lh_addr_of(octet1), lh_addr_of(octet2),
                      lh_addr_of(octet3));
    lh_net_ip4_set(self, octet0, octet1, octet2, octet3);
}

lh_u8_t
lh_net_ip4_get_octet(const lh_net_ip4_t *self, lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(index >= LH_NET_IP4_OCTET_COUNT,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    return self->octets[index];
}

void
lh_net_ip4_set_octet(lh_net_ip4_t *self, lh_usize_t index, lh_u8_t value)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(index >= LH_NET_IP4_OCTET_COUNT,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    self->octets[index] = value;
}

lh_bool_t
lh_net_ip4_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_ip4_t *out)
{
    lh_net_ip4_t result;
    lh_usize_t pos = 0;
    lh_usize_t octet_index;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    for (octet_index = 0; octet_index < LH_NET_IP4_OCTET_COUNT; octet_index++)
    {
        lh_bool_t is_last = octet_index == LH_NET_IP4_OCTET_COUNT - 1U;
        lh_str_cptr dot = lh_str_ptr_find_of_char(str + pos, str_size - pos, '.');
        lh_usize_t octet_len = dot != lh_null ? (lh_usize_t)(dot - (str + pos)) : str_size - pos;
        lh_uint_t octet_value;

        if (is_last != (dot == lh_null))
        {
            return lh_bool_false; /* missing '.' before the last octet, or an extra one after it */
        }

        if (!lh_str_ptr_parse_uint(str + pos, octet_len, LH_NET_IP4_OCTET_MAX, lh_addr_of(octet_value)))
        {
            return lh_bool_false;
        }
        lh_net_ip4_set_octet(lh_addr_of(result), octet_index, (lh_u8_t)octet_value);

        pos += octet_len + (is_last ? 0U : 1U); /* also skip the '.' except after the last octet */
    }

    *out = result;
    return lh_bool_true;
}

lh_usize_t
lh_net_ip4_format(const lh_net_ip4_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    /* Octets are lh_u8_t; variadic default promotion takes them to plain int, not lh_uint_t
     * (the type %u reads via va_arg) — cast each one explicitly. */
    return lh_str_ptr_format_text(
        str, str_size, "%u.%u.%u.%u",
        (lh_uint_t)lh_net_ip4_get_octet(self, LH_NET_IP4_OCTET_INDEX_0),
        (lh_uint_t)lh_net_ip4_get_octet(self, LH_NET_IP4_OCTET_INDEX_1),
        (lh_uint_t)lh_net_ip4_get_octet(self, LH_NET_IP4_OCTET_INDEX_2),
        (lh_uint_t)lh_net_ip4_get_octet(self, LH_NET_IP4_OCTET_INDEX_3));
}

lh_bool_t
lh_net_ip4_equals(const lh_net_ip4_t *self, const lh_net_ip4_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    return lh_memory_compare(self->octets, LH_NET_IP4_OCTET_COUNT, other->octets,
                             LH_NET_IP4_OCTET_COUNT) == lh_null
               ? lh_bool_true
               : lh_bool_false;
}
