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

#include <lh/os/system/win/types.h>
#include <lh/os/system/win/ws2_32/fd_set/fields.h>
#include <lh/os/system/win/ws2_32/in_addr/fields.h>
#include <lh/os/system/win/ws2_32/sockaddr/fields.h>
#include <lh/os/system/win/ws2_32/sockaddr_in/fields.h>
#include <lh/os/system/win/ws2_32/timeval/fields.h>
#include <lh/os/system/win/ws2_32/wsadata/fields.h>
#include <lh/str/ptr.h>
#include <lh/compiler/extern/c.h>

/** @brief `SOCKET`: `UINT_PTR`, pointer-sized. */
typedef lh_usize_t lh_os_system_win_socket_t;

/** @{ */
/** `INVALID_SOCKET` / `SOCKET_ERROR`. */
#define LH_OS_SYSTEM_WIN_INVALID_SOCKET (lh_cast_static(lh_os_system_win_socket_t, ~0ULL))
#define LH_OS_SYSTEM_WIN_SOCKET_ERROR (-1)
/** @} */

/** `MAKEWORD(2, 2)`: the Winsock version we ask WSAStartup for. */
#define LH_OS_SYSTEM_WIN_WINSOCK_VERSION_2_2 0x0202U

/** @{ */
/** Address family, socket types and protocols for ::socket. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_AF_INET 2
#define LH_OS_SYSTEM_WIN_SOCK_STREAM 1
#define LH_OS_SYSTEM_WIN_SOCK_DGRAM 2
#define LH_OS_SYSTEM_WIN_IPPROTO_TCP 6
#define LH_OS_SYSTEM_WIN_IPPROTO_UDP 17
/** @} */

/** @{ */
/** Winsock's SOL_SOCKET / SO_REUSEADDR are the BSD values (0xFFFF / 4), not
   Linux's (1 / 2). */
#define LH_OS_SYSTEM_WIN_SOL_SOCKET 0xFFFF
#define LH_OS_SYSTEM_WIN_SO_REUSEADDR 0x0004
#define LH_OS_SYSTEM_WIN_SO_ERROR 0x1007
/** @} */

/** `TCP_NODELAY` at level IPPROTO_TCP. Present since Winsock 1.1. */
#define LH_OS_SYSTEM_WIN_TCP_NODELAY 0x0001

/** `FIONBIO`: _IOW('f', 126, u_long), the ioctlsocket command that switches a
   socket between blocking and non-blocking. Negative as a `long`. */
#define LH_OS_SYSTEM_WIN_FIONBIO (lh_cast_static(lh_long_t, 0x8004667EUL))

/** `FIONREAD`: _IOR('f', 127, u_long), the ioctlsocket command that answers how many
   bytes a `recv` could return now. */
#define LH_OS_SYSTEM_WIN_FIONREAD (lh_cast_static(lh_long_t, 0x4004667FUL))

/** Interrupted Winsock call. */
#define LH_OS_SYSTEM_WIN_WSAEINTR 10004

/** Invalid socket argument. */
#define LH_OS_SYSTEM_WIN_WSAEINVAL 10022

/** A bounded connection phase timed out. */
#define LH_OS_SYSTEM_WIN_WSAETIMEDOUT 10060

/** `WSAEWOULDBLOCK`: a non-blocking call that cannot finish yet. A connect that is
   under way says it too, where POSIX says EINPROGRESS. */
#define LH_OS_SYSTEM_WIN_WSAEWOULDBLOCK 10035

/** `FD_SETSIZE`: the default, which is all a single-socket wait needs. */
#define LH_OS_SYSTEM_WIN_FD_SETSIZE 64

/** @brief `WSADATA`. */
struct lh_os_system_win_wsadata
{
    /** Members: ::lh_os_system_win_wsadata_fields. */
    lh_os_system_win_wsadata_fields(lh_os_system_win_word_t, lh_ushort_t, lh_char_t);
};
/** @copybrief lh_os_system_win_wsadata */
typedef struct lh_os_system_win_wsadata lh_os_system_win_wsadata_t;

/** @brief `IN_ADDR`. */
struct lh_os_system_win_in_addr
{
    /** Members: ::lh_os_system_win_in_addr_fields. */
    lh_os_system_win_in_addr_fields(lh_ulong_t);
};
/** @copybrief lh_os_system_win_in_addr */
typedef struct lh_os_system_win_in_addr lh_os_system_win_in_addr_t;

/** @brief `SOCKADDR`. */
struct lh_os_system_win_sockaddr
{
    /** Members: ::lh_os_system_win_sockaddr_fields. */
    lh_os_system_win_sockaddr_fields(lh_ushort_t, lh_char_t);
};
/** @copybrief lh_os_system_win_sockaddr */
typedef struct lh_os_system_win_sockaddr lh_os_system_win_sockaddr_t;

/** @brief `fd_set`. */
struct lh_os_system_win_fd_set
{
    /** Members: ::lh_os_system_win_fd_set_fields. */
    lh_os_system_win_fd_set_fields(lh_uint_t, lh_os_system_win_socket_t,
                                   LH_OS_SYSTEM_WIN_FD_SETSIZE);
};
/** @copybrief lh_os_system_win_fd_set */
typedef struct lh_os_system_win_fd_set lh_os_system_win_fd_set_t;

/** @brief `TIMEVAL`. */
struct lh_os_system_win_timeval
{
    /** Members: ::lh_os_system_win_timeval_fields. */
    lh_os_system_win_timeval_fields(lh_long_t);
};
/** @copybrief lh_os_system_win_timeval */
typedef struct lh_os_system_win_timeval lh_os_system_win_timeval_t;

/** @brief `SOCKADDR_IN`. */
struct lh_os_system_win_sockaddr_in
{
    /** Members: ::lh_os_system_win_sockaddr_in_fields. */
    lh_os_system_win_sockaddr_in_fields(lh_ushort_t, lh_ushort_t, lh_os_system_win_in_addr_t,
                                        lh_char_t);
};
/** @copybrief lh_os_system_win_sockaddr_in */
typedef struct lh_os_system_win_sockaddr_in lh_os_system_win_sockaddr_in_t;

/* Library setup. */

LH_COMPILER_EXTERN_C_BEGIN

/** Start Winsock for this process; pairs with ::WSACleanup.
 * Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
WSAStartup(lh_os_system_win_word_t wVersionRequested, lh_os_system_win_wsadata_t *lpWSAData);

/** Release what ::WSAStartup took. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
WSACleanup(void);

/* Sockets. Failure is INVALID_SOCKET or SOCKET_ERROR; the code is in
   WSAGetLastError(), which is GetLastError() (see kernel32.h). */

/** A new socket. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_socket_t LH_OS_SYSTEM_WIN_CALL
socket(lh_int_t af, lh_int_t type, lh_int_t protocol);

/** Close a socket. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
closesocket(lh_os_system_win_socket_t s);

/** Connect a socket to a peer. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
connect(lh_os_system_win_socket_t s, const lh_os_system_win_sockaddr_t *name, lh_int_t namelen);

/** Give a socket a local address. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
bind(lh_os_system_win_socket_t s, const lh_os_system_win_sockaddr_t *name, lh_int_t namelen);

/** Mark a bound socket as accepting connections. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
listen(lh_os_system_win_socket_t s, lh_int_t backlog);

/** Take the next pending connection. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_socket_t LH_OS_SYSTEM_WIN_CALL
accept(lh_os_system_win_socket_t s, lh_os_system_win_sockaddr_t *addr, lh_int_t *addrlen);

/** The local address of a socket. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
getsockname(lh_os_system_win_socket_t s, lh_os_system_win_sockaddr_t *name, lh_int_t *namelen);

/** Set a socket option. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
setsockopt(lh_os_system_win_socket_t s, lh_int_t level, lh_int_t optname, lh_str_cptr optval,
           lh_int_t optlen);

/** Read a socket option. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
getsockopt(lh_os_system_win_socket_t s, lh_int_t level, lh_int_t optname, lh_str_ptr optval,
           lh_int_t *optlen);

/** Socket control, e.g. `FIONBIO` for non-blocking mode.
 * Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
ioctlsocket(lh_os_system_win_socket_t s, lh_long_t cmd, lh_ulong_t *argp);

/** Wait until sockets are readable or writable. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
select(lh_int_t nfds, lh_os_system_win_fd_set_t *readfds, lh_os_system_win_fd_set_t *writefds,
       lh_os_system_win_fd_set_t *exceptfds, const lh_os_system_win_timeval_t *timeout);

/** Set the error `WSAGetLastError` returns. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT void LH_OS_SYSTEM_WIN_CALL
WSASetLastError(lh_int_t iError);

/** Send on a connected socket. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
send(lh_os_system_win_socket_t s, lh_str_cptr buf, lh_int_t len, lh_int_t flags);

/** Receive from a connected socket. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
recv(lh_os_system_win_socket_t s, lh_str_ptr buf, lh_int_t len, lh_int_t flags);

/** Send a datagram to an address. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
sendto(lh_os_system_win_socket_t s, lh_str_cptr buf, lh_int_t len, lh_int_t flags,
       const lh_os_system_win_sockaddr_t *to, lh_int_t tolen);

/** Receive a datagram and its sender's address. Present since Windows 95 / NT 4.0 (Winsock 2). */
LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
recvfrom(lh_os_system_win_socket_t s, lh_str_ptr buf, lh_int_t len, lh_int_t flags,
         lh_os_system_win_sockaddr_t *from, lh_int_t *fromlen);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_WS2_32_H */
