/**
 * @file user32.h
 * @brief Backend-private: the part of user32.dll the Windows window backend
 *        uses, declared by us instead of `<windows.h>`.
 *
 * Same rules as ws2_32.h / kernel32.h: only what is called, `lh`-prefixed
 * types and constants, Win32 function and member names. See types.h.
 *
 * XP-compat: nothing here is post-XP (no `WM_INPUT`, no `WM_TOUCH`,
 * `SetProcessDPIAware`, `RegisterClassExW` with Vista fields).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_USER32_H
#define LH_SRC_OS_SYSTEM_WIN_USER32_H

#include <lh/cast/static.h>
#include <lh/numeric/types.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/window/msg.h>
#include <lh/os/system/win/window/paintstruct.h>
#include <lh/os/system/win/window/rect.h>
#include <lh/os/system/win/window/wndclassexw.h>
#include <lh/ptr.h>
#include <lh/wstr/ptr.h>

/* `HWND` is `HANDLE` to a window. Same all-ones-sentinel trick as
   ::LH_OS_SYSTEM_WIN_INVALID_HANDLE works for "no window" once reinterpreted
   as `lh_ssize_t`. */
typedef lh_os_system_win_handle_t lh_os_system_win_hwnd_t;

/** @def LH_OS_SYSTEM_WIN_HWND_NULL
 *  @brief `NULL` HWND — i.e. `((HWND)0)`, the only null sentinel Win32
 *         actually uses for windows. (Compare to ::LH_OS_SYSTEM_WIN_INVALID_HANDLE,
 *         which is the `-1` cast used for general `HANDLE` returns.) */
#define LH_OS_SYSTEM_WIN_HWND_NULL                                                              \
    (lh_cast_static(lh_os_system_win_hwnd_t, lh_cast_static(lh_ssize_t, 0)))

/* `HINSTANCE` for `CreateWindowExW`'s first argument: pass NULL for a window
   owned by the current process. */
typedef lh_os_system_win_handle_t lh_os_system_win_hinstance_t;

/** @def LH_OS_SYSTEM_WIN_HINSTANCE_NULL
 *  @brief `NULL` HINSTANCE — same bit pattern as ::LH_OS_SYSTEM_WIN_HWND_NULL,
 *         kept as a separate macro for self-documentation at call sites. */
#define LH_OS_SYSTEM_WIN_HINSTANCE_NULL                                                          \
    (lh_cast_static(lh_os_system_win_hinstance_t, lh_cast_static(lh_ssize_t, 0)))

/* `HDC` — device context handle. */
typedef lh_ptr lh_os_system_win_hdc_t;

/* `HBRUSH` — brush handle (returned by `CreateSolidBrush`, used by
   `FillRect`, freed with `DeleteObject`). */
typedef lh_ptr lh_os_system_win_hbrush_t;

/* `HCURSOR` — cursor handle (returned by `LoadCursorW`). */
typedef lh_ptr lh_os_system_win_hcursor_t;

/* `COLORREF` — 32-bit RGB, `0x00bbggrr`. Passed to `CreateSolidBrush`. */
typedef lh_os_system_win_dword_t lh_os_system_win_colorref_t;

/* `RGB(r, g, b)` macro for `COLORREF`. `lh_cast_static` widens each
   `lh_uchar_t` to `dword` without sign warnings; `lh_bit_or` keeps the
   outer pack under the lh macro layer. */
#define LH_OS_SYSTEM_WIN_RGB(r, g, b)                                                            \
    (lh_cast_static(lh_os_system_win_colorref_t,                                                \
                    lh_bit_or(lh_bit_or(lh_bit_shl(lh_cast_static(lh_os_system_win_dword_t, (r)), 0),    \
                                        lh_bit_shl(lh_cast_static(lh_os_system_win_dword_t, (g)), 8)), \
                              lh_bit_shl(lh_cast_static(lh_os_system_win_dword_t, (b)), 16))))

/* `MAKEINTRESOURCE(id)` from Win32: convert a `#IDWORD` into a void* by
   rounding-tripping through the pointer-sized integer. The standard Win32
   header writes it as `(LPWSTR)(ULONG_PTR)(WORD)(id)`; we mirror that. */
#define LH_OS_SYSTEM_WIN_MAKEINTRESOURCE(id)                                                     \
    (lh_cast_reinterpret(lh_ptr, lh_cast_static(lh_usize_t, lh_cast_static(lh_os_system_win_word_t, (id)))))

/* `WPARAM` / `LPARAM`: pointer-sized (LLP64). */
typedef lh_ulong_t lh_os_system_win_wparam_t;
typedef lh_ssize_t lh_os_system_win_lparam_t;
typedef lh_ulong_t lh_os_system_win_lresult_t;

/* `WNDPROC` — `LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)`.
   `LH_OS_SYSTEM_WIN_CALL` is `__stdcall` (the calling convention
   user32.dll dispatches with on every Windows target, not just x86). */
typedef lh_os_system_win_lresult_t (*lh_os_system_win_wndproc_t)(                               \
    lh_os_system_win_hwnd_t, lh_os_system_win_dword_t,                                          \
    lh_os_system_win_wparam_t, lh_os_system_win_lparam_t);

/* `RECT` (left, top, right, bottom). Defined before `PAINTSTRUCT` because
   `PAINTSTRUCT`'s fields X-macro takes `lh_os_system_win_rect_t` as a type
   parameter — forward-reference would leave it as implicit `int` and break
   the layout. */
typedef struct lh_os_system_win_rect
{
    lh_os_system_win_rect_fields(lh_int_t);
} lh_os_system_win_rect_t;

/* `MSG` payload, brought in via the X-macro in msg/fields.h. */
typedef struct lh_os_system_win_msg
{
    lh_os_system_win_msg_fields(lh_os_system_win_hwnd_t, lh_os_system_win_dword_t,
                                lh_os_system_win_wparam_t, lh_os_system_win_lparam_t);
} lh_os_system_win_msg_t;

/* `PAINTSTRUCT`. */
typedef struct lh_os_system_win_paintstruct
{
    lh_os_system_win_paintstruct_fields(lh_os_system_win_hdc_t, lh_bool_t, lh_os_system_win_rect_t,
                                        lh_os_system_win_dword_t);
} lh_os_system_win_paintstruct_t;

/* `WNDCLASSEXW`. */
typedef struct lh_os_system_win_wndclassexw
{
    lh_os_system_win_wndclassexw_fields(lh_os_system_win_dword_t, lh_int_t,
                                        lh_os_system_win_wndproc_t);
} lh_os_system_win_wndclassexw_t;

/* Window class styles we set. */
#define LH_OS_SYSTEM_WIN_CS_HREDRAW 0x0002
#define LH_OS_SYSTEM_WIN_CS_VREDRAW 0x0001

/* Window styles: `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
   WS_MINIMIZEBOX | WS_MAXIMIZEBOX`. Available since Win95/NT3 — XP-clean. */
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPED 0x00000000
#define LH_OS_SYSTEM_WIN_WS_CAPTION 0x00C00000
#define LH_OS_SYSTEM_WIN_WS_SYSMENU 0x00080000
#define LH_OS_SYSTEM_WIN_WS_THICKFRAME 0x00040000
#define LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX 0x00020000
#define LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX 0x00010000

/* ShowWindow / UpdateWindow commands. */
#define LH_OS_SYSTEM_WIN_SW_SHOW 5
#define LH_OS_SYSTEM_WIN_SW_SHOWNORMAL 1

/* GetMessage / PeekMessage filter flags. */
#define LH_OS_SYSTEM_WIN_PM_NOREMOVE 0x0000
#define LH_OS_SYSTEM_WIN_PM_REMOVE 0x0001
#define LH_OS_SYSTEM_WIN_PM_NOYIELD 0x0002

/* Window messages we handle. */
#define LH_OS_SYSTEM_WIN_WM_PAINT 0x000F
#define LH_OS_SYSTEM_WIN_WM_CLOSE 0x0010
#define LH_OS_SYSTEM_WIN_WM_QUIT 0x0012
#define LH_OS_SYSTEM_WIN_WM_DESTROY 0x0002
#define LH_OS_SYSTEM_WIN_WM_SIZE 0x0005
#define LH_OS_SYSTEM_WIN_WM_KEYDOWN 0x0100
#define LH_OS_SYSTEM_WIN_WM_KEYUP 0x0101
#define LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN 0x0201
#define LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN 0x0204
#define LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN 0x0207
#define LH_OS_SYSTEM_WIN_WM_MOUSEMOVE 0x0200
#define LH_OS_SYSTEM_WIN_WM_MOUSEWHEEL 0x020A

/* `CreateWindowExW` extended styles. */
#define LH_OS_SYSTEM_WIN_WS_EX_CLIENTEDGE 0x00000200
#define LH_OS_SYSTEM_WIN_WS_EX_APPWINDOW 0x00040000

/* `CW_USEDEFAULT` lets the OS pick the position/size. */
#define LH_OS_SYSTEM_WIN_CW_USEDEFAULT (lh_cast_static(lh_int_t, 0x80000000))

/* `LoadCursorW`'s predefined cursors (top word of the `lpCursorName` arg). */
#define LH_OS_SYSTEM_WIN_IDC_ARROW 32512

/* Window messages the default proc consumes silently: returning
   `DefWindowProcW` is the right answer for them, so the switch in our
   window proc lists nothing here yet. */

/* Class registration. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_word_t LH_OS_SYSTEM_WIN_CALL
RegisterClassExW(const lh_os_system_win_wndclassexw_t *lpwcx);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
UnregisterClassW(lh_wstr_cptr lpClassName, lh_os_system_win_hinstance_t hInstance);

/* Window lifetime. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
CreateWindowExW(lh_os_system_win_dword_t dwExStyle,
                lh_wstr_cptr lpClassName, lh_wstr_cptr lpWindowName,
                lh_os_system_win_dword_t dwStyle, lh_int_t X, lh_int_t Y,
                lh_int_t nWidth, lh_int_t nHeight,
                lh_os_system_win_hwnd_t hWndParent, lh_ptr hMenu,
                lh_os_system_win_hinstance_t hInstance, lh_ptr lpParam);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
DestroyWindow(lh_os_system_win_hwnd_t hWnd);

/* Show / update. */

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
ShowWindow(lh_os_system_win_hwnd_t hWnd, lh_int_t nCmdShow);

/* Default proc. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DefWindowProcW(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_dword_t Msg,
               lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

/* Message loop. */

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
GetMessageW(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
            lh_os_system_win_dword_t wMsgFilterMin, lh_os_system_win_dword_t wMsgFilterMax);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
PeekMessageW(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
             lh_os_system_win_dword_t wMsgFilterMin, lh_os_system_win_dword_t wMsgFilterMax,
             lh_os_system_win_dword_t wRemoveMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
TranslateMessage(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DispatchMessageW(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT void LH_OS_SYSTEM_WIN_CALL
PostQuitMessage(lh_int_t nExitCode);

/* Painting. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
BeginPaint(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_paintstruct_t *lpPaint);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
EndPaint(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_paintstruct_t *lpPaint);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
FillRect(lh_os_system_win_hdc_t hDC, const lh_os_system_win_rect_t *lprc, lh_os_system_win_hbrush_t hbr);

/* Custom brush. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hbrush_t LH_OS_SYSTEM_WIN_CALL
CreateSolidBrush(lh_os_system_win_colorref_t color);

LH_OS_SYSTEM_WIN_IMPORT lh_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteObject(lh_ptr hObject);

/* `LoadCursorW` (for the arrow cursor on the class). */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hcursor_t LH_OS_SYSTEM_WIN_CALL
LoadCursorW(lh_os_system_win_hinstance_t hInstance, lh_ptr lpCursorName);

#endif /* LH_SRC_OS_SYSTEM_WIN_USER32_H */