/**
 * @file addr.h
 * @brief Socket address — tagged union over the concrete per-IP-version
 *        address types (::lh_net_ip4_socket_addr_t today).
 *
 * The C answer to "the address can be either kind, decided at runtime":
 * not templates (there is nothing to decide at compile time — which kind a
 * given address is isn't known until you parse or receive one), but a
 * discriminated union, the same shape BSD sockets use themselves
 * (`struct sockaddr` + `sa_family`) and the same shape Rust's `SocketAddr`
 * enum compiles down to. `os/net`'s connect/send will take this type and
 * switch on ::lh_net_socket_addr_get_family internally to build whatever
 * the OS call actually needs.
 */

#ifndef LH_NET_SOCKET_ADDR_H
#define LH_NET_SOCKET_ADDR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @typedef lh_net_socket_addr_family_t
 * @brief Discriminator for which concrete address ::lh_net_socket_addr_t holds.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_net_socket_addr_family_*`
 * constants below.
 */
typedef lh_u8_t lh_net_socket_addr_family_t;

/**
 * @def lh_net_socket_addr_family_ip4
 * @brief ::lh_net_socket_addr_t holds an ::lh_net_ip4_socket_addr_t (`.as.ip4`).
 */
#define lh_net_socket_addr_family_ip4 0U

/**
 * @struct lh_net_socket_addr
 * @typedef lh_net_socket_addr_t
 * @brief A `family` tag plus a union of the address it names.
 *
 * Only ::lh_net_socket_addr_family_ip4 / `.as.ip4` exist today; a future
 * IPv6 counterpart adds its own tag and union member here, not a new type.
 */
struct lh_net_socket_addr
{
    lh_net_socket_addr_family_t family;
    union
    {
        lh_net_ip4_socket_addr_t ip4;
    } as;
};
typedef struct lh_net_socket_addr lh_net_socket_addr_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Wrap an ::lh_net_ip4_socket_addr_t as an ::lh_net_socket_addr_t.
 *
 * @param addr Address to wrap.
 * @return Constructed ::lh_net_socket_addr_t with
 *         ::lh_net_socket_addr_family_ip4.
 */
LH_ATTRIBUTE_SYMBOL
lh_net_socket_addr_t
lh_net_socket_addr_make_ip4(const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Return which concrete address @p self holds.
 * @param self Address to read from.
 * @return One of the `lh_net_socket_addr_family_*` constants.
 */
LH_ATTRIBUTE_SYMBOL
lh_net_socket_addr_family_t
lh_net_socket_addr_get_family(const lh_net_socket_addr_t *self);

/**
 * @brief Return the ::lh_net_ip4_socket_addr_t held by @p self.
 *
 * @param self Address to read from; must have
 *             ::lh_net_socket_addr_family_ip4 (checked at runtime).
 * @return The wrapped IPv4 socket address.
 */
LH_ATTRIBUTE_SYMBOL
lh_net_ip4_socket_addr_t
lh_net_socket_addr_get_ip4(const lh_net_socket_addr_t *self);

/**
 * @brief Format @p self as text, dispatching on its family.
 *
 * @param self     Address to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written, or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_net_socket_addr_format(const lh_net_socket_addr_t *self, lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Test whether @p self and @p other hold the same family and address.
 *
 * @param self  Address to compare.
 * @param other Address to compare against.
 * @return ::lh_bool_true if both have the same family and equal address,
 *         ::lh_bool_false otherwise (including a family mismatch).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_net_socket_addr_equals(const lh_net_socket_addr_t *self, const lh_net_socket_addr_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_SOCKET_ADDR_H */
