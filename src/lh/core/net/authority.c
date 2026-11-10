/**
 * @file authority.c
 * @brief Splitting `host[:port]` and `[ipv6][:port]`.
 */

#include <lh/core/net/authority.h>
#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_net_authority_try_split(lh_str_cptr text, lh_usize_t text_size, lh_str_view_t *host,
                           lh_net_port_t *port, lh_bool_t *has_port)
{
    /* The characters a host never holds. */
    const lh_char_t refused[2] = {lh_char_map_lbracket, lh_char_map_rbracket};
    lh_usize_t begin = 0U;
    lh_usize_t end = text_size;
    lh_usize_t port_at = text_size;
    lh_bool_t with_port = lh_bool_false;
    lh_net_port_t number = 0U;

    lh_assert_runtime_ref(host);
    lh_assert_runtime_ref(port);
    lh_return_if(lh_null_eq(text) || lh_math_is_zero(text_size), lh_bool_false);
    if (lh_math_eq(text[0], lh_char_map_lbracket))
    {
        lh_str_cptr close = lh_str_ptr_find_of_char(text + 1, text_size - 1U, lh_char_map_rbracket);

        lh_return_if(lh_null_eq(close), lh_bool_false);
        begin = 1U;
        end = lh_cast_static(lh_usize_t, close - text);
        if (lh_math_lt(end + 1U, text_size))
        {
            lh_return_if(lh_math_ne(text[end + 1U], lh_char_map_colon), lh_bool_false);
            with_port = lh_bool_true;
            port_at = end + 2U;
        }
    }
    else
    {
        lh_str_cptr colon = lh_str_ptr_find_of_char(text, text_size, lh_char_map_colon);

        /* One colon is a port; more mean a bare IPv6 address. */
        if (lh_null_ne(colon))
        {
            const lh_usize_t at = lh_cast_static(lh_usize_t, colon - text);

            if (lh_null_eq(
                    lh_str_ptr_find_of_char(colon + 1, text_size - at - 1U, lh_char_map_colon)))
            {
                end = at;
                with_port = lh_bool_true;
                port_at = at + 1U;
            }
        }
    }
    lh_return_if(lh_math_eq(begin, end), lh_bool_false);
    for (lh_usize_t i = begin; lh_math_lt(i, end); ++i)
    {
        lh_return_if(lh_math_le(lh_cast_static(lh_byte_t, text[i]), lh_char_map_space) ||
                         lh_str_ptr_contains_char(refused, sizeof(refused), text[i]),
                     lh_bool_false);
    }
    if (with_port)
    {
        lh_return_ifn(lh_net_port_parse(text + port_at, text_size - port_at, lh_addr_of(number)),
                      lh_bool_false);
    }
    lh_str_view_init_by_size(host, text + begin, end - begin);
    lh_ptr_deref(port) = number;
    if (lh_null_ne(has_port))
    {
        lh_ptr_deref(has_port) = with_port;
    }
    return lh_bool_true;
}
