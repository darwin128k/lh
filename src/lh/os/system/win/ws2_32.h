/**
 * @file ws2_32.h
 * @brief Backend-private: the part of ws2_32.dll (Winsock 2) the Windows
 *        backend uses, declared by us instead of `<winsock2.h>`.
 *
 * Same rules as kernel32.h: only what is called, `lh`-prefixed types and
 * constants, Win32 function and member names. See types.h.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WS2_32_H
#define LH_SRC_OS_SYSTEM_WIN_WS2_32_H

#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/ws2_32/in_addr/fields.h>
#include <lh/os/system/win/ws2_32/sockaddr/fields.h>
#include <lh/os/system/win/ws2_32/sockaddr_in/fields.h>
#include <lh/os/system/win/ws2_32/wsadata/fields.h>
#include <lh/str/ptr.h>

/** @brief `SOCKET`: `UINT_PTR`, pointer-sized. */
typedef lh_usize_t lh_os_system_win_socket_t;

/* `INVALID_SOCKET` / `SOCKET_ERROR`. */
#define LH_OS_SYSTEM_WIN_INVALID_SOCKET (lh_cast_static(lh_os_system_win_socket_t, ~0ULL))
#define LH_OS_SYSTEM_WIN_SOCKET_ERROR (-1)

/* `MAKEWORD(2, 2)`: the Winsock version we ask WSAStartup for. */
#define LH_OS_SYSTEM_WIN_WINSOCK_VERSION_2_2 0x0202U

#define LH_OS_SYSTEM_WIN_AF_INET 2
#define LH_OS_SYSTEM_WIN_SOCK_STREAM 1
#define LH_OS_SYSTEM_WIN_SOCK_DGRAM 2
#define LH_OS_SYSTEM_WIN_IPPROTO_TCP 6
#define LH_OS_SYSTEM_WIN_IPPROTO_UDP 17

/* Winsock's SOL_SOCKET / SO_REUSEADDR are the BSD values (0xFFFF / 4), not
   Linux's (1 / 2). */
#define LH_OS_SYSTEM_WIN_SOL_SOCKET 0xFFFF
#define LH_OS_SYSTEM_WIN_SO_REUSEADDR 0x0004

/** @brief `WSADATA`. */
typedef struct lh_os_system_win_wsadata
{
    lh_os_system_win_wsadata_fields(lh_os_system_win_word_t, lh_ushort_t, lh_char_t);
} lh_os_system_win_wsadata_t;

/** @brief `IN_ADDR`. */
typedef struct lh_os_system_win_in_addr
{
    lh_os_system_win_in_addr_fields(lh_ulong_t);
} lh_os_system_win_in_addr_t;

/** @brief `SOCKADDR`. */
typedef struct lh_os_system_win_sockaddr
{
    lh_os_system_win_sockaddr_fields(lh_ushort_t, lh_char_t);
} lh_os_system_win_sockaddr_t;

/** @brief `SOCKADDR_IN`. */
typedef struct lh_os_system_win_sockaddr_in
{
    lh_os_system_win_sockaddr_in_fields(lh_ushort_t, lh_ushort_t, lh_os_system_win_in_addr_t, lh_char_t);
} lh_os_system_win_sockaddr_in_t;

/* Library setup. */

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
WSAStartup(lh_os_system_win_word_t wVersionRequested, lh_os_system_win_wsadata_t *lpWSAData);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
WSACleanup(void);

/* Sockets. Failure is INVALID_SOCKET or SOCKET_ERROR; the code is in
   WSAGetLastError(), which is GetLastError() (see kernel32.h). */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_socket_t LH_OS_SYSTEM_WIN_CALL
socket(lh_int_t af, lh_int_t type, lh_int_t protocol);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
closesocket(lh_os_system_win_socket_t s);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
connect(lh_os_system_win_socket_t s, const lh_os_system_win_sockaddr_t *name, lh_int_t namelen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
bind(lh_os_system_win_socket_t s, const lh_os_system_win_sockaddr_t *name, lh_int_t namelen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
listen(lh_os_system_win_socket_t s, lh_int_t backlog);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_socket_t LH_OS_SYSTEM_WIN_CALL
accept(lh_os_system_win_socket_t s, lh_os_system_win_sockaddr_t *addr, lh_int_t *addrlen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
getsockname(lh_os_system_win_socket_t s, lh_os_system_win_sockaddr_t *name, lh_int_t *namelen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
setsockopt(lh_os_system_win_socket_t s, lh_int_t level, lh_int_t optname, lh_str_cptr optval, lh_int_t optlen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
send(lh_os_system_win_socket_t s, lh_str_cptr buf, lh_int_t len, lh_int_t flags);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
recv(lh_os_system_win_socket_t s, lh_str_ptr buf, lh_int_t len, lh_int_t flags);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
sendto(lh_os_system_win_socket_t s, lh_str_cptr buf, lh_int_t len, lh_int_t flags,
       const lh_os_system_win_sockaddr_t *to, lh_int_t tolen);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
recvfrom(lh_os_system_win_socket_t s, lh_str_ptr buf, lh_int_t len, lh_int_t flags,
         lh_os_system_win_sockaddr_t *from, lh_int_t *fromlen);

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_H */
