/**
 * @file authority.h
 * @brief The host and the port of a network address written as text.
 *
 * Reads the forms a configuration or a URL carries: `host:port`,
 * `[ipv6]:port`, a bare host and a bare IPv6 address (two or more colons and
 * no brackets mean the whole text is the host). No DNS: the host is only cut
 * out, never looked up or checked for existence.
 */

#ifndef LH_NET_AUTHORITY_H
#define LH_NET_AUTHORITY_H

#include <lh/core/net/port.h>
#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Split @p text into its host and its port.
 *
 * The host is not empty and holds no space, control character or bracket;
 * a port, when there is one, is what ::lh_net_port_parse accepts.
 *
 * @param text      The address text (not required to be NUL-terminated).
 * @param text_size Characters in @p text.
 * @param host      Receives a view of the host, inside @p text (brackets dropped).
 * @param port      Receives the port; 0 when the text has none.
 * @param has_port  Receives whether the text has a port; may be ::lh_null.
 * @return ::lh_bool_false for malformed text; the outputs are untouched then.
 */
lh_bool_t
lh_net_authority_try_split(const lh_str_ptr text, lh_usize_t text_size, lh_str_view_t *host,
                           lh_net_port_t *port, lh_bool_t *has_port);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_AUTHORITY_H */
