/**
 * @file forwarded.h
 * @brief The client behind trusted proxies, from `X-Forwarded-For`.
 *
 * Each proxy appends the address it got the request from, so the hops are read
 * right to left (the last field first, the last entry of a field first): every
 * hop that is a trusted proxy is skipped and the first untrusted one is the
 * client. A malformed entry ends the walk, because nothing to its left can be
 * believed; the last trusted hop seen stands then.
 */

#ifndef LH_HTTP_FORWARDED_H
#define LH_HTTP_FORWARDED_H

#include <lh/core/net/cidr.h>
#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write the client of a request that came through @p proxy into @p client.
 *
 * @param values        The `X-Forwarded-For` field values, in the order received;
 *                      may be ::lh_null when @p count is 0.
 * @param count         Fields in @p values.
 * @param proxy         The address the request came from; checked by
 *                      ::lh_assert_runtime_ref.
 * @param trusted       The trusted proxies; may be ::lh_null when @p trusted_count is 0.
 * @param trusted_count Blocks in @p trusted.
 * @param client        Receives the client: @p proxy when no hop can be believed;
 *                      checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_http_forwarded_copy_client(const lh_str_view_t *const *values, lh_usize_t count,
                              const lh_net_ip4_t *proxy, const lh_net_cidr_t *trusted,
                              lh_usize_t trusted_count, lh_net_ip4_t *client);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_FORWARDED_H */
