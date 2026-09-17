/**
 * @file ip.h
 * @brief IPv4 address value type: parse and format.
 *
 * Pure value type — no sockets, no system headers. Builds and runs anywhere
 * ::lh itself does, including without an OS (see @c lh/os/net for the actual
 * socket contract, which does depend on the platform).
 */

#ifndef LH_NET_IP_H
#define LH_NET_IP_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_NET_IP4_TEXT_MAX
 * @brief Longest dotted-decimal IPv4 text, excluding a NUL terminator.
 *
 * `255.255.255.255` is 15 characters.
 */
#define LH_NET_IP4_TEXT_MAX 15U

/**
 * @def LH_NET_IP4_OCTET_COUNT
 * @brief Number of octets in an ::lh_net_ip4_t.
 */
#define LH_NET_IP4_OCTET_COUNT 4U

/**
 * @def LH_NET_IP4_OCTET_INDEX_0
 * @brief Index of the first octet (`192` in `192.168.0.1`).
 */
#define LH_NET_IP4_OCTET_INDEX_0 0U

/**
 * @def LH_NET_IP4_OCTET_INDEX_1
 * @brief Index of the second octet.
 */
#define LH_NET_IP4_OCTET_INDEX_1 1U

/**
 * @def LH_NET_IP4_OCTET_INDEX_2
 * @brief Index of the third octet.
 */
#define LH_NET_IP4_OCTET_INDEX_2 2U

/**
 * @def LH_NET_IP4_OCTET_INDEX_3
 * @brief Index of the fourth octet.
 */
#define LH_NET_IP4_OCTET_INDEX_3 3U

/**
 * @struct lh_net_ip4
 * @typedef lh_net_ip4_t
 * @brief IPv4 address as ::LH_NET_IP4_OCTET_COUNT octets, in the order they
 *        appear in dotted-decimal text (`octets[0]` is `192` in `192.168.0.1`).
 */
struct lh_net_ip4
{
    lh_u8_t octets[LH_NET_IP4_OCTET_COUNT];
};
typedef struct lh_net_ip4 lh_net_ip4_t;

/**
 * @def LH_NET_IP4_OCTET_MAX
 * @brief Largest valid value for a single IPv4 octet.
 *
 * Expands to ::LH_U8_T_MAX (each octet is an ::lh_u8_t).
 */
#define LH_NET_IP4_OCTET_MAX LH_U8_T_MAX

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Write individual octets into @p self.
 *
 * Each output pointer is optional: pass ::lh_null to leave that octet unchanged.
 *
 * @param self   Address to modify.
 * @param octet0 New first octet, or ::lh_null to leave unchanged.
 * @param octet1 New second octet, or ::lh_null to leave unchanged.
 * @param octet2 New third octet, or ::lh_null to leave unchanged.
 * @param octet3 New fourth octet, or ::lh_null to leave unchanged.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_net_ip4_pack(lh_net_ip4_t *self, const lh_u8_t *octet0, const lh_u8_t *octet1,
                const lh_u8_t *octet2, const lh_u8_t *octet3);

/**
 * @brief Read individual octets out of @p self.
 *
 * Each output pointer is optional: pass ::lh_null to skip that octet.
 *
 * @param self   Address to read from.
 * @param octet0 Output for the first octet, or ::lh_null to skip.
 * @param octet1 Output for the second octet, or ::lh_null to skip.
 * @param octet2 Output for the third octet, or ::lh_null to skip.
 * @param octet3 Output for the fourth octet, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_net_ip4_unpack(const lh_net_ip4_t *self, lh_u8_t *octet0, lh_u8_t *octet1, lh_u8_t *octet2,
                  lh_u8_t *octet3);

/* ── construct / set ─────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_net_ip4_t from four octets.
 *
 * @param octet0 First octet (`192` in `192.168.0.1`).
 * @param octet1 Second octet.
 * @param octet2 Third octet.
 * @param octet3 Fourth octet.
 * @return Constructed ::lh_net_ip4_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_net_ip4_t
lh_net_ip4_make(lh_u8_t octet0, lh_u8_t octet1, lh_u8_t octet2, lh_u8_t octet3);

/**
 * @brief Replace all four octets of @p self.
 *
 * @param self   Address to modify.
 * @param octet0 First octet.
 * @param octet1 Second octet.
 * @param octet2 Third octet.
 * @param octet3 Fourth octet.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_net_ip4_set(lh_net_ip4_t *self, lh_u8_t octet0, lh_u8_t octet1, lh_u8_t octet2, lh_u8_t octet3);

/**
 * @brief Copy the address state from @p other into @p self.
 * @param self  Destination address.
 * @param other Source address.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_net_ip4_assign(lh_net_ip4_t *self, const lh_net_ip4_t *other);

/* ── octet access ────────────────────────────────────────────────────────── */

/**
 * @brief Return the octet at @p index.
 *
 * @param self  Address to read from.
 * @param index Octet position (0-3; `0` is `192` in `192.168.0.1`).
 * @return Octet value at @p index.
 */
LH_ATTRIBUTE_SYMBOL
lh_u8_t
lh_net_ip4_get_octet(const lh_net_ip4_t *self, lh_usize_t index);

/**
 * @brief Replace the octet at @p index.
 *
 * @param self  Address to modify.
 * @param index Octet position (0-3).
 * @param value New octet value.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_net_ip4_set_octet(lh_net_ip4_t *self, lh_usize_t index, lh_u8_t value);

/**
 * @brief Test whether @p self is in 127.0.0.0/8 (loopback).
 *
 * @param self Address to classify.
 * @return ::lh_bool_true if the first octet is 127.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_net_ip4_is_loopback(const lh_net_ip4_t *self);

/**
 * @brief Test whether @p self is in RFC 1918 private space.
 *
 * Matches `10.0.0.0/8`, `172.16.0.0/12`, and `192.168.0.0/16`. Loopback
 * is a separate predicate (::lh_net_ip4_is_loopback).
 *
 * @param self Address to classify.
 * @return ::lh_bool_true if the address is RFC 1918 private.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_net_ip4_is_private(const lh_net_ip4_t *self);

/* ── parse / format / compare ────────────────────────────────────────────── */

/**
 * @brief Parse a strict dotted-decimal IPv4 address.
 *
 * Requires exactly four decimal octets (0-255) separated by `.`, all of
 * @p str_size consumed. Rejects a leading zero on any octet longer than one
 * digit (`192.168.001.1`), whitespace, and a leading sign.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed address on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid
 *         address, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_net_ip4_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_ip4_t *out);

/**
 * @brief Format @p self as dotted-decimal text.
 *
 * No NUL terminator is appended; a buffer of ::LH_NET_IP4_TEXT_MAX + 1
 * always has room to add one after the returned length.
 *
 * @param self     Address to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (3-15), or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_net_ip4_format(const lh_net_ip4_t *self, lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Test whether @p self and @p other hold the same four octets.
 *
 * @param self  Address to compare.
 * @param other Address to compare against.
 * @return ::lh_bool_true if every octet matches, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_net_ip4_equals(const lh_net_ip4_t *self, const lh_net_ip4_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_IP_H */
