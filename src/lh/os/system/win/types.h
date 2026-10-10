/**
 * @file types.h
 * @brief Backend-private: Win32 base types and the import/calling-convention
 *        markers for our own declarations of the Win32 API.
 *
 * The Windows backend describes the kernel itself (kernel32.h, ws2_32.h)
 * instead of including `<windows.h>` / `<winsock2.h>`. The Win32 ABI is a
 * documented, frozen contract, so mirroring the few types and functions the
 * backend uses is safe. Not installed, not part of the API, and must never
 * share a translation unit with `<windows.h>` (the function names clash).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_TYPES_H
#define LH_SRC_OS_SYSTEM_WIN_TYPES_H

#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/size.h>

/* Win32 is LLP64 on every Windows target: long is 32-bit, so DWORD is
   `unsigned long`, exactly as <windows.h> spells it. */

/** @brief `DWORD`. */
typedef lh_ulong_t lh_os_system_win_dword_t;

/** @brief `UINT`. */
typedef lh_uint_t lh_os_system_win_uint_t;

/**
 * @brief `UINT_PTR`: an unsigned the size of a pointer.
 *
 * On the 32-bit targets this build is pinned to, `LPARAM` -- and so `wParam` -- is a
 * 32-bit integer even though it carries a pointer's worth of bits, which is what makes
 * one type that fits both on every target this library builds for. Present since
 * Windows 95.
 */
typedef lh_ulong_t lh_os_system_win_uintptr_t;

/** @brief `WORD`. */
typedef lh_ushort_t lh_os_system_win_word_t;

/** @brief `BOOL`: nonzero on success. */
typedef lh_int_t lh_os_system_win_bool_t;

/** @brief `HANDLE`. */
typedef lh_ptr lh_os_system_win_handle_t;

/* `MAX_PATH`: length of a classic (non-`\\?\`) path buffer, NUL included. */
#define LH_OS_SYSTEM_WIN_MAX_PATH 260

/* Placed before the return type: the function lives in a system DLL. */
#define LH_OS_SYSTEM_WIN_IMPORT __declspec(dllimport)

/* Placed before the function name: `WINAPI` / `WSAAPI`. Only matters on
   32-bit x86, where it is not the default convention. */
#define LH_OS_SYSTEM_WIN_CALL __stdcall

#endif /* LH_SRC_OS_SYSTEM_WIN_TYPES_H */
