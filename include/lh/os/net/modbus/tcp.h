/**
 * @file tcp.h
 * @brief Modbus TCP: a ::lh_mb_transport_t over a non-blocking socket.
 *
 * The link (`lh/net/modbus/link.h`) never waits, and it can only keep that
 * promise over a transport that does not either. This one never blocks in
 * `send` or `recv`: nothing arrived yet is `0`, a full send buffer is `0`, and
 * only a connection that is really gone is an error.
 *
 * Opening is the one call that can take time, because a connect to an address
 * nobody answers is not refused, it is simply never answered — and left to the
 * operating system that is twenty seconds and more per dead device. Here it is
 * at most ::lh_os_net_modbus_tcp_t's connect timeout.
 *
 * A connection that breaks — the peer closed it, reset it, or half a request
 * went out — is closed by the transport there and then, and every call after
 * that is an error until the link is opened again (::lh_mb_link_open). A
 * stream that has lost its place cannot be read on; reconnecting is the only
 * way back, and when to do it is the app's decision.
 *
 * IPv4 addresses only (`"192.168.1.10"`): there is no name lookup.
 *
 * Requires ::LH_LIBRARY_OPTION_OS; ::lh_os_net_init must have succeeded.
 */

#ifndef LH_OS_NET_MODBUS_TCP_H
#define LH_OS_NET_MODBUS_TCP_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/net/modbus/link.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/net/modbus/tcp/fields.h>
#include <lh/os/net/socket.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/net/modbus/tcp.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/** @brief The port Modbus TCP listens on. */
#define LH_OS_NET_MODBUS_TCP_PORT ((lh_u16_t)502)

/**
 * @struct lh_os_net_modbus_tcp
 * @typedef lh_os_net_modbus_tcp_t
 * @brief One Modbus TCP connection, as a link's transport.
 */
struct lh_os_net_modbus_tcp
{
    lh_os_net_modbus_tcp_fields(lh_os_net_socket_t, lh_u32_t);
};
typedef struct lh_os_net_modbus_tcp lh_os_net_modbus_tcp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start closed.
 *
 * @param self               Transport to set up.
 * @param connect_timeout_ms How long opening may wait for the device to accept;
 *                           `0` means it must accept at once, which only a
 *                           device on the same machine does.
 */
lh_void
lh_os_net_modbus_tcp_init(lh_os_net_modbus_tcp_t *self, lh_u32_t connect_timeout_ms);

/**
 * @brief The four calls a link is given (::lh_mb_link_init), closing over @p self.
 *
 * @param self Transport; must outlive every link it is given to.
 */
lh_mb_transport_t
lh_os_net_modbus_tcp_get_transport(lh_os_net_modbus_tcp_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_MODBUS_TCP_H */
