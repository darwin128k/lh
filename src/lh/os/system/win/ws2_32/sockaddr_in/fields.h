/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_sockaddr_in_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_IN_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_IN_FIELDS_H

/**
 * @def lh_os_system_win_sockaddr_in_fields(family_type, port_type, in_addr_type, char_type)
 * @brief `SOCKADDR_IN`. Member names are the BSD ones, same as POSIX's
 *        `struct sockaddr_in`, so net/socket/addr.h serves both backends.
 *
 * @param family_type  `ADDRESS_FAMILY` (::lh_ushort_t).
 * @param port_type    Port in network byte order (::lh_ushort_t).
 * @param in_addr_type ::lh_os_system_win_in_addr_t.
 * @param char_type    Narrow character type (::lh_char_t).
 */
#define lh_os_system_win_sockaddr_in_fields(family_type, port_type, in_addr_type, char_type)       \
    family_type sin_family;                                                                        \
    port_type sin_port;                                                                            \
    in_addr_type sin_addr;                                                                         \
    char_type sin_zero[8]

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_IN_FIELDS_H */
