/**
 * @file forwarded.c
 * @brief Walking `X-Forwarded-For` right to left past trusted proxies.
 */

#include <lh/core/http/forwarded.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/char/map.h>
#include <lh/core/math.h>

lh_void
lh_http_forwarded_copy_client(const lh_str_view_t *const *values, lh_usize_t count,
                              const lh_net_ip4_t *proxy, const lh_net_cidr_t *trusted,
                              lh_usize_t trusted_count, lh_net_ip4_t *client)
{
    lh_assert_runtime_ref(proxy);
    lh_assert_runtime_ref(client);
    lh_net_ip4_assign(client, proxy);
    for (lh_usize_t field = count; lh_math_gt(field, 0U); --field)
    {
        lh_str_view_t rest;

        lh_str_view_init_by_other(lh_addr_of(rest), values[lh_math_sub_one(field)]);
        for (;;)
        {
            const lh_usize_t comma = lh_str_view_rfind_char(lh_addr_of(rest), lh_char_map_comma);
            const lh_usize_t start =
                lh_math_eq(comma, LH_STR_VIEW_INVALID) ? 0U : lh_math_add_one(comma);
            lh_str_view_t entry;
            lh_net_ip4_t hop;

            lh_str_view_tail(lh_addr_of(rest), start, lh_addr_of(entry));
            lh_str_view_trim(lh_addr_of(entry));
            if (lh_str_view_is_empty(lh_addr_of(entry)) ||
                !lh_net_ip4_parse(lh_str_view_get_data(lh_addr_of(entry)),
                                  lh_str_view_get_size(lh_addr_of(entry)), lh_addr_of(hop)))
            {
                return;
            }
            lh_net_ip4_assign(client, lh_addr_of(hop));
            if (!lh_net_cidr_list_contains(trusted, trusted_count, lh_addr_of(hop)))
            {
                return;
            }
            if (lh_math_eq(comma, LH_STR_VIEW_INVALID))
            {
                break;
            }

            lh_str_view_t left;

            lh_str_view_from_offset(lh_addr_of(rest), 0U, comma, lh_addr_of(left));
            lh_str_view_assign(lh_addr_of(rest), lh_addr_of(left));
        }
    }
}
