/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_in_addr_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_IN_ADDR_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_IN_ADDR_FIELDS_H

/**
 * @def lh_os_system_win_in_addr_fields(ulong_type)
 * @brief `IN_ADDR`: an IPv4 address in network byte order. Win32 wraps it in
 *        a union of byte/word views; only the 32-bit view is mirrored.
 *
 * @param ulong_type 32-bit `u_long` (::lh_ulong_t).
 */
#define lh_os_system_win_in_addr_fields(ulong_type) ulong_type s_addr

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_IN_ADDR_FIELDS_H */
