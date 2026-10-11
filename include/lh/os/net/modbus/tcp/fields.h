/**
 * @file fields.h
 * @brief Member fields of ::lh_os_net_modbus_tcp_t.
 */

#ifndef LH_OS_NET_MODBUS_TCP_FIELDS_H
#define LH_OS_NET_MODBUS_TCP_FIELDS_H

/**
 * @def lh_os_net_modbus_tcp_fields(socket_type, timeout_type)
 * @brief The connection, and how long opening one may take.
 *
 * @param socket_type  Type of `socket` (::lh_os_net_socket_t); empty while closed.
 * @param timeout_type Type of `connect_timeout_ms` (::lh_u32_t).
 */
#define lh_os_net_modbus_tcp_fields(socket_type, timeout_type)                                     \
    socket_type socket;                                                                            \
    timeout_type connect_timeout_ms

#endif /* LH_OS_NET_MODBUS_TCP_FIELDS_H */
