/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_sockaddr_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_FIELDS_H

/**
 * @def lh_os_system_win_sockaddr_fields(family_type, char_type)
 * @brief `SOCKADDR`: the generic address every socket call takes.
 *
 * @param family_type `ADDRESS_FAMILY` (::lh_ushort_t).
 * @param char_type   Narrow character type (::lh_char_t).
 */
#define lh_os_system_win_sockaddr_fields(family_type, char_type)                                   \
    family_type sa_family;                                                                         \
    char_type sa_data[14]

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_SOCKADDR_FIELDS_H */
