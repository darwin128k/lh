/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_wsadata_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_WSADATA_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_WSADATA_FIELDS_H

#include <lh/compiler/arch.h>

/**
 * @def lh_os_system_win_wsadata_fields(word_type, ushort_type, char_type)
 * @brief `WSADATA`. The member order differs between 64-bit and 32-bit
 *        Windows (the vendor-info block moves ahead of the strings on Win64).
 *
 * @param word_type   ::lh_os_system_win_word_t.
 * @param ushort_type ::lh_ushort_t.
 * @param char_type   Narrow character type (::lh_char_t).
 */
#if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64
#    define lh_os_system_win_wsadata_fields(word_type, ushort_type, char_type)                     \
        word_type wVersion;                                                                        \
        word_type wHighVersion;                                                                    \
        ushort_type iMaxSockets;                                                                   \
        ushort_type iMaxUdpDg;                                                                     \
        char_type *lpVendorInfo;                                                                   \
        char_type szDescription[257];                                                              \
        char_type szSystemStatus[129]
#else
#    define lh_os_system_win_wsadata_fields(word_type, ushort_type, char_type)                     \
        word_type wVersion;                                                                        \
        word_type wHighVersion;                                                                    \
        char_type szDescription[257];                                                              \
        char_type szSystemStatus[129];                                                             \
        ushort_type iMaxSockets;                                                                   \
        ushort_type iMaxUdpDg;                                                                     \
        char_type *lpVendorInfo
#endif

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_WSADATA_FIELDS_H */
