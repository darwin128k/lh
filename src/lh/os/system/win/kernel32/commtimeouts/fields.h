/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_commtimeouts_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_COMMTIMEOUTS_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_COMMTIMEOUTS_FIELDS_H

/**
 * @def lh_os_system_win_commtimeouts_fields(dword_type)
 * @brief `COMMTIMEOUTS`: how long a read or a write on a port may take, in ms.
 *
 * @param dword_type `DWORD` (::lh_os_system_win_dword_t).
 */
#define lh_os_system_win_commtimeouts_fields(dword_type)                                           \
    dword_type ReadIntervalTimeout;                                                                \
    dword_type ReadTotalTimeoutMultiplier;                                                         \
    dword_type ReadTotalTimeoutConstant;                                                           \
    dword_type WriteTotalTimeoutMultiplier;                                                        \
    dword_type WriteTotalTimeoutConstant

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_COMMTIMEOUTS_FIELDS_H */
