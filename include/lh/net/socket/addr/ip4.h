/**
 * @file ip4.h
 * @brief IPv4 socket address (IP + port) value type: parse and format.
 *
 * Protocol-agnostic — TCP vs UDP is a property of the socket you make with
 * this address, not of the address itself, so it is not stored here.
 *
 * Named with the IP version (`ip4`), not just `lh_net_socket_addr_t`,
 * because IPv6 needs extra fields (flow info, scope id) this shape doesn't
 * have — it will be its own ::lh_net_ip4_socket_addr-shaped-but-not-identical
 * type sharing ::lh_net_socket_addr_fields for the `ip`/`port` part, not a
 * drop-in extension of this one. The unversioned `lh_net_socket_addr` name
 * stays free for a future tagged union of the two (mirrors Rust's
 * `Ipv4Addr`/`SocketAddrV4`/`SocketAddrV6` vs the version-agnostic `SocketAddr`).
 */

#ifndef LH_NET_SOCKET_ADDR_IP4_H
#define LH_NET_SOCKET_ADDR_IP4_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/net/ip.h>
#include <lh/net/port.h>
#include <lh/net/socket/addr/fields.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_NET_IP4_SOCKET_ADDR_TEXT_MAX
 * @brief Longest `ip:port` text, excluding a NUL terminator.
 *
 * ::LH_NET_IP4_TEXT_MAX (15) + `:` (1) + longest port `65535` (5) = 21.
 */
#define LH_NET_IP4_SOCKET_ADDR_TEXT_MAX (LH_NET_IP4_TEXT_MAX + 1U + 5U)

/**
 * @struct lh_net_ip4_socket_addr
 * @typedef lh_net_ip4_socket_addr_t
 * @brief An ::lh_net_ip4_t paired with an ::lh_net_port_t.
 *
 * Fields injected via ::lh_net_socket_addr_fields.
 */
struct lh_net_ip4_socket_addr
{
    lh_net_socket_addr_fields(lh_net_ip4_t);
};
typedef struct lh_net_ip4_socket_addr lh_net_ip4_socket_addr_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── construct / set ─────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_net_ip4_socket_addr_t from an IP and a port.
 *
 * @param ip   IP address.
 * @param port Port number.
 * @return Constructed ::lh_net_ip4_socket_addr_t value.
 */
lh_net_ip4_socket_addr_t
lh_net_ip4_socket_addr_make(const lh_net_ip4_t *ip, lh_net_port_t port);

/**
 * @brief Replace both the IP and the port of @p self.
 *
 * @param self Address to modify.
 * @param ip   New IP address.
 * @param port New port number.
 */
void
lh_net_ip4_socket_addr_set(lh_net_ip4_socket_addr_t *self, const lh_net_ip4_t *ip,
                           lh_net_port_t port);

/**
 * @brief Copy the address state from @p other into @p self.
 * @param self  Destination address.
 * @param other Source address.
 */
void
lh_net_ip4_socket_addr_assign(lh_net_ip4_socket_addr_t *self,
                              const lh_net_ip4_socket_addr_t *other);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the IP address stored in @p self.
 * @param self Address to read from.
 * @return Current IP address.
 */
lh_net_ip4_t
lh_net_ip4_socket_addr_get_ip(const lh_net_ip4_socket_addr_t *self);

/**
 * @brief Replace the IP address stored in @p self.
 * @param self Address to modify.
 * @param ip   New IP address.
 */
void
lh_net_ip4_socket_addr_set_ip(lh_net_ip4_socket_addr_t *self, const lh_net_ip4_t *ip);

/**
 * @brief Return the port stored in @p self.
 * @param self Address to read from.
 * @return Current port number.
 */
lh_net_port_t
lh_net_ip4_socket_addr_get_port(const lh_net_ip4_socket_addr_t *self);

/**
 * @brief Replace the port stored in @p self.
 * @param self Address to modify.
 * @param port New port number.
 */
void
lh_net_ip4_socket_addr_set_port(lh_net_ip4_socket_addr_t *self, lh_net_port_t port);

/* ── parse / format / compare ────────────────────────────────────────────── */

/**
 * @brief Parse a strict `ip:port` address (e.g. `192.168.0.1:27015`).
 *
 * Splits on the first `:`, then delegates to ::lh_net_ip4_parse and
 * ::lh_net_port_parse on each side. All of @p str_size must be consumed.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed address on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid
 *         `ip:port` pair, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_net_ip4_socket_addr_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_ip4_socket_addr_t *out);

/**
 * @brief Format @p self as `ip:port` text.
 *
 * @param self     Address to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written, or 0 if @p str_size was too small.
 */
lh_usize_t
lh_net_ip4_socket_addr_format(const lh_net_ip4_socket_addr_t *self, lh_str_ptr str,
                              lh_usize_t str_size);

/**
 * @brief Test whether @p self and @p other hold the same IP and port.
 *
 * @param self  Address to compare.
 * @param other Address to compare against.
 * @return ::lh_bool_true if both the IP and the port match, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_net_ip4_socket_addr_equals(const lh_net_ip4_socket_addr_t *self,
                              const lh_net_ip4_socket_addr_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_SOCKET_ADDR_IP4_H */
