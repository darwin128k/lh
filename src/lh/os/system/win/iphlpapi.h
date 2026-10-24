/**
 * @file iphlpapi.h
 * @brief Backend-private: the part of iphlpapi.dll the Windows backend uses,
 *        declared by us instead of `<iphlpapi.h>` (see kernel32.h).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_IPHLPAPI_H
#define LH_SRC_OS_SYSTEM_WIN_IPHLPAPI_H

#include <lh/os/system/win/types.h>

/** `ERROR_INSUFFICIENT_BUFFER`: the table did not fit; the size is updated. */
#define LH_OS_SYSTEM_WIN_ERROR_INSUFFICIENT_BUFFER 122UL

/** `MIB_IPADDRROW`: one IPv4 address of an interface. */
struct lh_os_system_win_mib_ipaddrrow
{
    lh_os_system_win_dword_t dwAddr; /**< Network byte order. */
    lh_os_system_win_dword_t dwIndex;
    lh_os_system_win_dword_t dwMask;
    lh_os_system_win_dword_t dwBCastAddr;
    lh_os_system_win_dword_t dwReasmSize;
    unsigned short unused1;
    unsigned short wType;
};
typedef struct lh_os_system_win_mib_ipaddrrow lh_os_system_win_mib_ipaddrrow_t;

/** `MIB_IPADDRTABLE`: @c dwNumEntries rows follow the count. */
struct lh_os_system_win_mib_ipaddrtable
{
    lh_os_system_win_dword_t dwNumEntries;
    lh_os_system_win_mib_ipaddrrow_t table[1];
};
typedef struct lh_os_system_win_mib_ipaddrtable lh_os_system_win_mib_ipaddrtable_t;

/** The interface-to-IPv4 address table. Present since Windows 98 / NT 4.0 SP4. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
GetIpAddrTable(lh_os_system_win_mib_ipaddrtable_t *pIpAddrTable, lh_os_system_win_dword_t *pdwSize,
               lh_os_system_win_bool_t bOrder);

#endif /* LH_SRC_OS_SYSTEM_WIN_IPHLPAPI_H */
