/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_fd_set_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_FD_SET_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_FD_SET_FIELDS_H

/**
 * @def lh_os_system_win_fd_set_fields(uint_type, socket_type, size)
 * @brief `fd_set`. Winsock's is a counted array of handles, not the bit set POSIX
 *        has, so a handle's value puts no limit on whether it fits.
 *
 * @param uint_type   `u_int` (::lh_uint_t).
 * @param socket_type `SOCKET` (::lh_os_system_win_socket_t).
 * @param size        `FD_SETSIZE`: how many handles the array holds.
 */
#define lh_os_system_win_fd_set_fields(uint_type, socket_type, size)                               \
    uint_type fd_count;                                                                            \
    socket_type fd_array[size]

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_FD_SET_FIELDS_H */
