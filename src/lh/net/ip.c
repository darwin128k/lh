#include <lh/net/ip.h>
#include <lh/assert.h>
#include <lh/memory.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/str/format/uint.h>
#include <lh/str/parse/uint.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

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

/* lh_net_ip4_get_octet / lh_net_ip4_set_octet are LH_ATTRIBUTE_FORCE_INLINE,
 * defined in the header — nothing to put here.
 *
 * lh_net_ip4_parse's octet loop used to go through lh_str_ptr_split_next +
 * lh_str_ptr_parse_uint + lh_net_ip4_set_octet as ordinary exported calls;
 * measured ~4-5x slower (Release, LTO, both static and DLL) than a fully
 * inlined equivalent. lh_str_ptr_split_next, lh_str_ptr_parse_uint, and
 * lh_net_ip4_get_octet/set_octet are now all LH_ATTRIBUTE_FORCE_INLINE in
 * their own headers, so this loop keeps the same decomposed shape while
 * compiling down to the fast version — lh_str_ptr_find_of_char (called once
 * per octet, from inside the now-inlined split_next) is the one real call
 * left, since it is shared, general-purpose, and used broadly elsewhere. */
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
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;
        lh_uint_t octet_value;

        if (!lh_str_ptr_split_next(str, str_size, '.', lh_addr_of(pos), lh_addr_of(field),
                                   lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            return lh_bool_false; /* ran out of string before all octets were read */
        }
        if (had_delim == is_last)
        {
            return lh_bool_false; /* missing '.' before the last octet, or an extra one after it */
        }

        if (!lh_str_ptr_parse_uint(field, field_size, LH_NET_IP4_OCTET_MAX,
                                   lh_addr_of(octet_value)))
        {
            return lh_bool_false;
        }
        lh_net_ip4_set_octet(lh_addr_of(result), octet_index, (lh_u8_t)octet_value);
    }

    *out = result;
    return lh_bool_true;
}

lh_usize_t
lh_net_ip4_format(const lh_net_ip4_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    /* Same shape as lh_str_ptr_format_text(str, str_size, "%u.%u.%u.%u", ...) — but that
     * reparses the four-conversion format string from scratch on every call (walk it
     * character by character, dispatch on '%', pull each argument via va_arg) for a format
     * that never changes. Since the shape is fixed, call lh_str_ptr_format_uint directly and
     * splice in the dots by hand instead; ~2.4x faster measured (Release, LTO). Builds into a
     * fixed-size scratch buffer first and only copies out once the full result is known to
     * fit — all-or-nothing on failure, same as every other lh_str_ptr_format_* function (see
     * lh_str_ptr_format_uint itself), so a too-small str_size leaves str untouched. */
    lh_char_t scratch[LH_NET_IP4_TEXT_MAX];
    lh_usize_t pos = 0;
    lh_usize_t octet_index;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    for (octet_index = 0; octet_index < LH_NET_IP4_OCTET_COUNT; octet_index++)
    {
        lh_usize_t written;

        if (octet_index > 0)
        {
            scratch[pos] = '.';
            pos++;
        }

        written = lh_str_ptr_format_uint((lh_uint_t)lh_net_ip4_get_octet(self, octet_index),
                                         scratch + pos, LH_NET_IP4_TEXT_MAX - pos);
        if (written == 0)
        {
            return 0; /* unreachable: each octet is <= 3 digits and scratch always has room,
                         but format_uint's own contract requires checking the return value */
        }
        pos += written;
    }

    if (pos > str_size)
    {
        return 0;
    }

    lh_memory_std_copy(str, scratch, pos);
    return pos;
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
