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

/* `HCURSOR` — cursor handle (returned by `LoadCursorW`). */
typedef lh_ptr lh_os_system_win_hcursor_t;

/* `MAKEINTRESOURCE(id)` from Win32: convert a `#IDWORD` into a void* by
   rounding-tripping through the pointer-sized integer. The standard Win32
   header writes it as `(LPWSTR)(ULONG_PTR)(WORD)(id)`; we mirror that. */
#define LH_OS_SYSTEM_WIN_MAKEINTRESOURCE(id)                                                     \
    (lh_cast_reinterpret(lh_ptr, lh_cast_static(lh_usize_t, lh_cast_static(lh_os_system_win_word_t, (id)))))

/* `WPARAM` / `LPARAM` / `LRESULT`: pointer-sized (`UINT_PTR` / `LONG_PTR`),
   so 64-bit on Win64. Not `lh_ulong_t`: `long` stays 32-bit on LLP64, which
   would shrink `MSG` below the 48 bytes `PeekMessageW` writes on x64. */
typedef lh_usize_t lh_os_system_win_wparam_t;
typedef lh_ssize_t lh_os_system_win_lparam_t;
typedef lh_ssize_t lh_os_system_win_lresult_t;

/* `WNDPROC` — `LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)`.
   `CALLBACK` is `__stdcall` (::LH_OS_SYSTEM_WIN_CALL): user32.dll calls the
   procedure that way. It only matters on 32-bit x86, where the callee pops
   its arguments - a pointer type without it mismatches the window procedure
   and, cast through, would unbalance the stack on every message. */
typedef lh_os_system_win_lresult_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_wndproc_t)(
    lh_os_system_win_hwnd_t, lh_os_system_win_dword_t, lh_os_system_win_wparam_t,
    lh_os_system_win_lparam_t);

/* `RECT` (left, top, right, bottom). Defined before `PAINTSTRUCT` because
   `PAINTSTRUCT`'s fields X-macro takes `lh_os_system_win_rect_t` as a type
   parameter — forward-reference would leave it as implicit `int` and break
   the layout. */
struct lh_os_system_win_rect
{
    lh_os_system_win_rect_fields(lh_int_t);
};
typedef struct lh_os_system_win_rect lh_os_system_win_rect_t;

/* `POINT`. */
struct lh_os_system_win_point
{
    lh_int_t x;
    lh_int_t y;
};
typedef struct lh_os_system_win_point lh_os_system_win_point_t;

/* `MINMAXINFO`. The maximized position and size are what a client-frame
   window uses so maximize stops at the work area. */
struct lh_os_system_win_minmaxinfo
{
    lh_os_system_win_point_t reserved;
    lh_os_system_win_point_t max_size;
    lh_os_system_win_point_t max_position;
    lh_os_system_win_point_t min_track;
    lh_os_system_win_point_t max_track;
};
typedef struct lh_os_system_win_minmaxinfo lh_os_system_win_minmaxinfo_t;

/* `MONITORINFO`. `size` is set to the struct size before the call. */
struct lh_os_system_win_monitorinfo
{
    lh_os_system_win_dword_t size;
    lh_os_system_win_rect_t monitor;
    lh_os_system_win_rect_t work;
    lh_os_system_win_dword_t flags;
};
typedef struct lh_os_system_win_monitorinfo lh_os_system_win_monitorinfo_t;

/* `MSG` payload, brought in via the X-macro in msg/fields.h. */
struct lh_os_system_win_msg
{
    lh_os_system_win_msg_fields(lh_os_system_win_hwnd_t, lh_os_system_win_dword_t,
                                lh_os_system_win_wparam_t, lh_os_system_win_lparam_t);
};
typedef struct lh_os_system_win_msg lh_os_system_win_msg_t;

/* `PAINTSTRUCT`. */
struct lh_os_system_win_paintstruct
{
    lh_os_system_win_paintstruct_fields(lh_os_system_win_hdc_t, lh_os_system_win_bool_t,
                                        lh_os_system_win_rect_t, lh_byte_t);
};
typedef struct lh_os_system_win_paintstruct lh_os_system_win_paintstruct_t;

/* `WNDCLASSEXW`. */
struct lh_os_system_win_wndclassexw
{
    lh_os_system_win_wndclassexw_fields(lh_os_system_win_dword_t, lh_int_t,
                                        lh_os_system_win_wndproc_t);
};
typedef struct lh_os_system_win_wndclassexw lh_os_system_win_wndclassexw_t;

/* Window class styles we set. */
#define LH_OS_SYSTEM_WIN_CS_HREDRAW 0x0002
#define LH_OS_SYSTEM_WIN_CS_VREDRAW 0x0001

/* Window styles: `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
   WS_MINIMIZEBOX | WS_MAXIMIZEBOX`. Available since Win95/NT3 — XP-clean. */
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPED 0x00000000
#define LH_OS_SYSTEM_WIN_WS_POPUP 0x80000000UL
#define LH_OS_SYSTEM_WIN_WS_CAPTION 0x00C00000
#define LH_OS_SYSTEM_WIN_WS_SYSMENU 0x00080000
#define LH_OS_SYSTEM_WIN_WS_THICKFRAME 0x00040000
#define LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX 0x00020000
#define LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX 0x00010000

/* Stock overlapped window, and the borderless one whose caption lh paints.
   Both sets of bits exist since Windows 95. */
#define LH_OS_SYSTEM_WIN_WS_FRAME_SYSTEM                                                         \
    (LH_OS_SYSTEM_WIN_WS_OVERLAPPED | LH_OS_SYSTEM_WIN_WS_CAPTION |                               \
     LH_OS_SYSTEM_WIN_WS_SYSMENU | LH_OS_SYSTEM_WIN_WS_THICKFRAME |                               \
     LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX | LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX)
#define LH_OS_SYSTEM_WIN_WS_FRAME_CLIENT                                                         \
    (LH_OS_SYSTEM_WIN_WS_POPUP | LH_OS_SYSTEM_WIN_WS_THICKFRAME |                                 \
     LH_OS_SYSTEM_WIN_WS_SYSMENU | LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX |                              \
     LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX)

/* `GetWindowLongW` / `SetWindowLongW` indices. Style is a 32-bit value, so
   `SetWindowLongW` is the right call on 32-bit and 64-bit. */
#define LH_OS_SYSTEM_WIN_GWL_STYLE -16
#define LH_OS_SYSTEM_WIN_GWL_EXSTYLE -20

/* ShowWindow / UpdateWindow commands. */
#define LH_OS_SYSTEM_WIN_SW_SHOW 5
#define LH_OS_SYSTEM_WIN_SW_SHOWNORMAL 1
#define LH_OS_SYSTEM_WIN_SW_MINIMIZE 6
#define LH_OS_SYSTEM_WIN_SW_MAXIMIZE 3
#define LH_OS_SYSTEM_WIN_SW_RESTORE 9

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
#define LH_OS_SYSTEM_WIN_WM_GETMINMAXINFO 0x0024
#define LH_OS_SYSTEM_WIN_WM_ERASEBKGND 0x0014
#define LH_OS_SYSTEM_WIN_WM_NCCALCSIZE 0x0083
#define LH_OS_SYSTEM_WIN_WM_NCHITTEST 0x0084
#define LH_OS_SYSTEM_WIN_WM_NCPAINT 0x0085
#define LH_OS_SYSTEM_WIN_WM_NCACTIVATE 0x0086
#define LH_OS_SYSTEM_WIN_WM_NCLBUTTONDOWN 0x00A1
#define LH_OS_SYSTEM_WIN_WM_NCLBUTTONUP 0x00A2
#define LH_OS_SYSTEM_WIN_WM_KEYDOWN 0x0100
#define LH_OS_SYSTEM_WIN_WM_KEYUP 0x0101
#define LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN 0x0201
#define LH_OS_SYSTEM_WIN_WM_LBUTTONUP 0x0202
#define LH_OS_SYSTEM_WIN_WM_RBUTTONUP 0x0205
#define LH_OS_SYSTEM_WIN_WM_MBUTTONUP 0x0208
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

/* `wParam` of `WM_SIZE`. */
#define LH_OS_SYSTEM_WIN_SIZE_RESTORED 0
#define LH_OS_SYSTEM_WIN_SIZE_MINIMIZED 1
#define LH_OS_SYSTEM_WIN_SIZE_MAXIMIZED 2

/* `WM_NCHITTEST` results. */
#define LH_OS_SYSTEM_WIN_HTCLIENT 1
#define LH_OS_SYSTEM_WIN_HTCAPTION 2
#define LH_OS_SYSTEM_WIN_HTMINBUTTON 8
#define LH_OS_SYSTEM_WIN_HTMAXBUTTON 9
#define LH_OS_SYSTEM_WIN_HTLEFT 10
#define LH_OS_SYSTEM_WIN_HTRIGHT 11
#define LH_OS_SYSTEM_WIN_HTTOP 12
#define LH_OS_SYSTEM_WIN_HTTOPLEFT 13
#define LH_OS_SYSTEM_WIN_HTTOPRIGHT 14
#define LH_OS_SYSTEM_WIN_HTBOTTOM 15
#define LH_OS_SYSTEM_WIN_HTBOTTOMLEFT 16
#define LH_OS_SYSTEM_WIN_HTBOTTOMRIGHT 17
#define LH_OS_SYSTEM_WIN_HTCLOSE 20

/* `SetWindowPos` flags. `FRAMECHANGED` applies a new client area. */
#define LH_OS_SYSTEM_WIN_SWP_NOSIZE 0x0001
#define LH_OS_SYSTEM_WIN_SWP_NOMOVE 0x0002
#define LH_OS_SYSTEM_WIN_SWP_NOZORDER 0x0004
#define LH_OS_SYSTEM_WIN_SWP_NOACTIVATE 0x0010
#define LH_OS_SYSTEM_WIN_SWP_FRAMECHANGED 0x0020

/* Window messages the default proc consumes silently: returning
   `DefWindowProcW` is the right answer for them, so the switch in our
   window proc lists nothing here yet. */

/* Class registration. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_word_t LH_OS_SYSTEM_WIN_CALL
RegisterClassExW(const lh_os_system_win_wndclassexw_t *lpwcx);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
UnregisterClassW(lh_wstr_cptr lpClassName, lh_os_system_win_hinstance_t hInstance);

/* Window lifetime. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
CreateWindowExW(lh_os_system_win_dword_t dwExStyle,
                lh_wstr_cptr lpClassName, lh_wstr_cptr lpWindowName,
                lh_os_system_win_dword_t dwStyle, lh_int_t X, lh_int_t Y,
                lh_int_t nWidth, lh_int_t nHeight,
                lh_os_system_win_hwnd_t hWndParent, lh_ptr hMenu,
                lh_os_system_win_hinstance_t hInstance, lh_ptr lpParam);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DestroyWindow(lh_os_system_win_hwnd_t hWnd);

/* Show / update. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ShowWindow(lh_os_system_win_hwnd_t hWnd, lh_int_t nCmdShow);

/* Default proc. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DefWindowProcW(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_dword_t Msg,
               lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

/* Message loop. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetMessageW(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
            lh_os_system_win_dword_t wMsgFilterMin, lh_os_system_win_dword_t wMsgFilterMax);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
PeekMessageW(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
             lh_os_system_win_dword_t wMsgFilterMin, lh_os_system_win_dword_t wMsgFilterMax,
             lh_os_system_win_dword_t wRemoveMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
TranslateMessage(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DispatchMessageW(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT void LH_OS_SYSTEM_WIN_CALL
PostQuitMessage(lh_int_t nExitCode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
PostMessageW(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_dword_t Msg,
             lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
SendMessageW(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_dword_t Msg,
             lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ReleaseCapture(void);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
WaitMessage(void);

/* Painting. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
BeginPaint(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_paintstruct_t *lpPaint);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EndPaint(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_paintstruct_t *lpPaint);

/* Device context of the client area, for drawing outside `WM_PAINT`. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
GetDC(lh_os_system_win_hwnd_t hWnd);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
ReleaseDC(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_hdc_t hDC);

/* `LoadCursorW` (for the arrow cursor on the class). */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hcursor_t LH_OS_SYSTEM_WIN_CALL
LoadCursorW(lh_os_system_win_hinstance_t hInstance, lh_ptr lpCursorName);

/* Outer frame of a window, in screen coordinates. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetWindowRect(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_rect_t *lpRect);

/* Grows a client rectangle to the outer window size for @p dwStyle.
   `bMenu` is false when the window has no menu. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
AdjustWindowRect(lh_os_system_win_rect_t *lpRect, lh_os_system_win_dword_t dwStyle,
                 lh_os_system_win_bool_t bMenu);

/* `MONITOR_DEFAULTTONEAREST`. Present since Windows 2000, so XP has it. */
#define LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTONEAREST 2

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
MonitorFromWindow(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t dwFlags);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetMonitorInfoW(lh_os_system_win_handle_t hMonitor, lh_os_system_win_monitorinfo_t *lpmi);

/* `MONITORENUMPROC`. Return true to keep walking monitors. */
typedef lh_int_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_monitor_enum_proc_t)(
    lh_os_system_win_handle_t hMonitor, lh_os_system_win_hdc_t hdc,
    lh_os_system_win_rect_t *lprcMonitor, lh_os_system_win_lparam_t dwData);

/* Every monitor when @p hdc and @p lprcClip are null. Present since Windows 2000. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EnumDisplayMonitors(lh_os_system_win_hdc_t hdc, const lh_os_system_win_rect_t *lprcClip,
                    lh_os_system_win_monitor_enum_proc_t lpfnEnum, lh_os_system_win_lparam_t dwData);

/* Replaces the window's clipping region. A null region restores the
   rectangle. The system takes ownership of a non-null region. */

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
SetWindowRgn(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_handle_t hRgn,
             lh_os_system_win_bool_t bRedraw);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
IsZoomed(lh_os_system_win_hwnd_t hWnd);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
IsIconic(lh_os_system_win_hwnd_t hWnd);

/* Window properties: a pointer-sized value kept by the window itself.
   The name is a literal; the value is not freed by the system. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
SetPropW(lh_os_system_win_hwnd_t hWnd, lh_wstr_cptr lpString, lh_os_system_win_handle_t hData);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
GetPropW(lh_os_system_win_hwnd_t hWnd, lh_wstr_cptr lpString);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
RemovePropW(lh_os_system_win_hwnd_t hWnd, lh_wstr_cptr lpString);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetClientRect(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_rect_t *lpRect);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ScreenToClient(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_point_t *lpPoint);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
GetWindowTextW(lh_os_system_win_hwnd_t hWnd, lh_wstr_ptr lpString, lh_int_t nMaxCount);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
SetWindowPos(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_hwnd_t hWndInsertAfter, lh_int_t X,
             lh_int_t Y, lh_int_t cx, lh_int_t cy, lh_os_system_win_uint_t uFlags);

LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
GetWindowLongW(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex);

LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
SetWindowLongW(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex, lh_long_t dwNewLong);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
FillRect(lh_os_system_win_hdc_t hDC, const lh_os_system_win_rect_t *lprc,
         lh_os_system_win_handle_t hbr);

#endif /* LH_SRC_OS_SYSTEM_WIN_USER32_H */