/**
 * @file port.h
 * @brief TCP/UDP port number value type: parse.
 *
 * Pure value type — no sockets, no system headers.
 */

#ifndef LH_NET_PORT_H
#define LH_NET_PORT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @typedef lh_net_port_t
 * @brief TCP/UDP port number.
 *
 * Alias for ::lh_u16_t (0-65535). Port 0 is a valid bit pattern (commonly
 * used to mean "let the OS pick"); ::lh_net_port_parse accepts it like any
 * other in-range value — callers that must reject it check separately.
 */
typedef lh_u16_t lh_net_port_t;

/**
 * @def LH_NET_PORT_MAX
 * @brief Largest valid ::lh_net_port_t value.
 *
 * Expands to ::LH_U16_T_MAX (::lh_net_port_t is an alias for ::lh_u16_t).
 */
#define LH_NET_PORT_MAX LH_U16_T_MAX

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse a decimal port number.
 *
 * Accepts only ASCII digits, all of @p str_size consumed. Rejects empty
 * input, a leading zero on input longer than one digit (`0080`), and values
 * above 65535.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed port on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid port
 *         number, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_net_port_parse(lh_str_cptr str, lh_usize_t str_size, lh_net_port_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_PORT_H */
