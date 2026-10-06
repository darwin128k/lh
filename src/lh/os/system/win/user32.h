/**
 * @file user32.h
 * @brief Backend-private: the part of user32.dll the Windows window backend
 *        uses, declared by us instead of `<windows.h>`.
 *
 * Must not share a translation unit with `<windows.h>` (names clash). Each
 * constant / type carries the Windows version it has been present since —
 * handwritten decls are the XP compatibility fence, not style.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_USER32_H
#define LH_SRC_OS_SYSTEM_WIN_USER32_H

#include <lh/byte.h>
#include <lh/char.h>
#include <lh/compiler/arch.h>
#include <lh/numeric/types.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/window/msg/fields.h>
#include <lh/os/system/win/window/paintstruct/fields.h>
#include <lh/os/system/win/window/wndclassexw/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/void.h>

/** @brief `HDC`. Present since Windows 95. */
typedef lh_ptr lh_os_system_win_hdc_t;

/** @brief `HWND`. Present since Windows 95. */
typedef lh_ptr lh_os_system_win_hwnd_t;

/** @brief `HINSTANCE` / `HMODULE`. Present since Windows 95. */
typedef lh_ptr lh_os_system_win_hinstance_t;

/** @brief `HCURSOR` / `HICON` / `HBRUSH` / `HMENU` as opaque handles. */
typedef lh_ptr lh_os_system_win_hgdi_t;

/** @brief `WPARAM`. Present since Windows 95. */
typedef lh_usize_t lh_os_system_win_wparam_t;

/** @brief `LPARAM` / `LRESULT` / `LONG_PTR`. Present since Windows 95. */
typedef lh_ssize_t lh_os_system_win_lparam_t;

/** @brief `LRESULT`. */
typedef lh_os_system_win_lparam_t lh_os_system_win_lresult_t;

/** @brief `ATOM`. Present since Windows 95. */
typedef lh_os_system_win_word_t lh_os_system_win_atom_t;

/**
 * @brief `WNDPROC`. Present since Windows 95.
 */
typedef lh_os_system_win_lresult_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_wndproc_t)(
    lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
    lh_os_system_win_lparam_t lparam);

/**
 * @struct lh_os_system_win_point
 * @brief `POINT`. Present since Windows 95.
 */
struct lh_os_system_win_point
{
    lh_int_t x;
    lh_int_t y;
};
typedef struct lh_os_system_win_point lh_os_system_win_point_t;

/**
 * @struct lh_os_system_win_rect
 * @brief `RECT` (right/bottom exclusive for regions and `FillRect`).
 *        Present since Windows 95.
 */
struct lh_os_system_win_rect
{
    lh_int_t left;
    lh_int_t top;
    lh_int_t right;
    lh_int_t bottom;
};
typedef struct lh_os_system_win_rect lh_os_system_win_rect_t;

/**
 * @struct lh_os_system_win_msg
 * @brief `MSG`. Present since Windows 95.
 *
 * `pt` is flattened to two integers (same layout as Win32 `POINT`).
 */
struct lh_os_system_win_msg
{
    lh_os_system_win_msg_fields(lh_os_system_win_hwnd_t, lh_os_system_win_dword_t,
                                lh_os_system_win_wparam_t, lh_os_system_win_lparam_t);
};
typedef struct lh_os_system_win_msg lh_os_system_win_msg_t;

/**
 * @struct lh_os_system_win_paintstruct
 * @brief `PAINTSTRUCT`. Present since Windows 95.
 */
struct lh_os_system_win_paintstruct
{
    lh_os_system_win_paintstruct_fields(lh_os_system_win_hdc_t, lh_os_system_win_bool_t,
                                        lh_os_system_win_rect_t, lh_byte_t);
};
typedef struct lh_os_system_win_paintstruct lh_os_system_win_paintstruct_t;

/**
 * @struct lh_os_system_win_wndclassexa
 * @brief `WNDCLASSEXA`. Present since Windows 95 (`WNDCLASSEX`).
 */
struct lh_os_system_win_wndclassexa
{
    lh_os_system_win_wndclassexw_fields(lh_os_system_win_dword_t, lh_int_t,
                                        lh_os_system_win_wndproc_t);
};
typedef struct lh_os_system_win_wndclassexa lh_os_system_win_wndclassexa_t;

/**
 * @struct lh_os_system_win_createstructa
 * @brief `CREATESTRUCTA`. Present since Windows 95.
 *
 * Only `lpCreateParams` is read (WM_NCCREATE); the rest must still match the
 * Win32 layout for the cast from `LPARAM` to be valid.
 */
struct lh_os_system_win_createstructa
{
    lh_ptr lpCreateParams;
    lh_os_system_win_hinstance_t hInstance;
    lh_os_system_win_hgdi_t hMenu;
    lh_os_system_win_hwnd_t hwndParent;
    lh_int_t cy;
    lh_int_t cx;
    lh_int_t y;
    lh_int_t x;
    lh_long_t style;
    lh_str_cptr lpszName;
    lh_str_cptr lpszClass;
    lh_os_system_win_dword_t dwExStyle;
};
typedef struct lh_os_system_win_createstructa lh_os_system_win_createstructa_t;

/* BOOL literals. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_FALSE 0
#define LH_OS_SYSTEM_WIN_TRUE 1

/* Class styles. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_CS_VREDRAW 0x0001U
#define LH_OS_SYSTEM_WIN_CS_HREDRAW 0x0002U

/* Window styles. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPED 0x00000000UL
#define LH_OS_SYSTEM_WIN_WS_CAPTION 0x00C00000UL
#define LH_OS_SYSTEM_WIN_WS_SYSMENU 0x00080000UL
#define LH_OS_SYSTEM_WIN_WS_THICKFRAME 0x00040000UL
#define LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX 0x00020000UL
#define LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX 0x00010000UL
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPEDWINDOW                                                        \
    (LH_OS_SYSTEM_WIN_WS_OVERLAPPED | LH_OS_SYSTEM_WIN_WS_CAPTION | LH_OS_SYSTEM_WIN_WS_SYSMENU |    \
     LH_OS_SYSTEM_WIN_WS_THICKFRAME | LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX |                             \
     LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX)

/* ShowWindow. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_SW_SHOW 5

/* CreateWindow position. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_CW_USEDEFAULT ((lh_int_t)0x80000000)

/* Stock cursor id as `MakeIntResourceA(32512)`. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_IDC_ARROW ((lh_str_cptr)(lh_usize_t)32512)

/* Window long offsets. Present since Windows 95 (`GWL_USERDATA`). */
#define LH_OS_SYSTEM_WIN_GWLP_USERDATA (-21)

/* Messages. Present since Windows 95 unless noted. */
#define LH_OS_SYSTEM_WIN_WM_DESTROY 0x0002U
#define LH_OS_SYSTEM_WIN_WM_PAINT 0x000FU
#define LH_OS_SYSTEM_WIN_WM_ERASEBKGND 0x0014U
#define LH_OS_SYSTEM_WIN_WM_NCCREATE 0x0081U
#define LH_OS_SYSTEM_WIN_WM_QUIT 0x0012U
#define LH_OS_SYSTEM_WIN_WM_MOUSEMOVE 0x0200U
#define LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN 0x0201U
#define LH_OS_SYSTEM_WIN_WM_LBUTTONUP 0x0202U
/* `WM_MOUSEWHEEL`. Present since Windows 98 / Windows NT 4.0 SP3. */
#define LH_OS_SYSTEM_WIN_WM_MOUSEWHEEL 0x020AU

#define LH_OS_SYSTEM_WIN_WHEEL_DELTA 120

/* PeekMessage. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_PM_REMOVE 0x0001U

/* MsgWaitForMultipleObjects wake mask. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_QS_ALLINPUT 0x04FFU

/* Wait result. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_INFINITE 0xFFFFFFFFUL
#define LH_OS_SYSTEM_WIN_WAIT_TIMEOUT 258UL

#define LH_OS_SYSTEM_WIN_LOWORD(v) ((lh_os_system_win_word_t)((lh_usize_t)(v) & 0xffffU))
#define LH_OS_SYSTEM_WIN_HIWORD(v) ((lh_os_system_win_word_t)(((lh_usize_t)(v) >> 16) & 0xffffU))
#define LH_OS_SYSTEM_WIN_GET_WHEEL_DELTA_WPARAM(w)                                                 \
    ((lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(w))

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_atom_t LH_OS_SYSTEM_WIN_CALL
RegisterClassExA(const lh_os_system_win_wndclassexa_t *wc);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hgdi_t LH_OS_SYSTEM_WIN_CALL
LoadCursorA(lh_os_system_win_hinstance_t instance, lh_str_cptr name);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
CreateWindowExA(lh_os_system_win_dword_t dwExStyle, lh_str_cptr lpClassName, lh_str_cptr lpWindowName,
                lh_os_system_win_dword_t dwStyle, lh_int_t x, lh_int_t y, lh_int_t nWidth,
                lh_int_t nHeight, lh_os_system_win_hwnd_t hWndParent, lh_os_system_win_hgdi_t hMenu,
                lh_os_system_win_hinstance_t hInstance, lh_ptr lpParam);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ShowWindow(lh_os_system_win_hwnd_t hWnd, lh_int_t nCmdShow);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
UpdateWindow(lh_os_system_win_hwnd_t hWnd);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DestroyWindow(lh_os_system_win_hwnd_t hWnd);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EnableWindow(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_bool_t bEnable);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
AdjustWindowRect(lh_os_system_win_rect_t *lpRect, lh_os_system_win_dword_t dwStyle,
                  lh_os_system_win_bool_t bMenu);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetClientRect(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_rect_t *lpRect);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
InvalidateRect(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_rect_t *lpRect,
               lh_os_system_win_bool_t bErase);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
BeginPaint(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_paintstruct_t *lpPaint);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EndPaint(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_paintstruct_t *lpPaint);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
SetCapture(lh_os_system_win_hwnd_t hWnd);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ReleaseCapture(lh_void);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
GetCapture(lh_void);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ScreenToClient(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_point_t *lpPoint);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
PeekMessageA(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
             lh_os_system_win_uint_t wMsgFilterMin, lh_os_system_win_uint_t wMsgFilterMax,
             lh_os_system_win_uint_t wRemoveMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
TranslateMessage(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DispatchMessageA(const lh_os_system_win_msg_t *lpMsg);

LH_OS_SYSTEM_WIN_IMPORT lh_void LH_OS_SYSTEM_WIN_CALL
PostQuitMessage(lh_int_t nExitCode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
MsgWaitForMultipleObjects(lh_os_system_win_dword_t nCount, const lh_os_system_win_handle_t *pHandles,
                          lh_os_system_win_bool_t bWaitAll, lh_os_system_win_dword_t dwMilliseconds,
                          lh_os_system_win_dword_t dwWakeMask);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DefWindowProcA(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_uint_t Msg,
               lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

#if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lparam_t LH_OS_SYSTEM_WIN_CALL
SetWindowLongPtrA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex, lh_os_system_win_lparam_t dwNewLong);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lparam_t LH_OS_SYSTEM_WIN_CALL
GetWindowLongPtrA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex);

#else

/* On x86 `SetWindowLongPtrA` is a macro for `SetWindowLongA` — XP has no Ptr export. */
LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
SetWindowLongA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex, lh_long_t dwNewLong);

LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
GetWindowLongA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex);

#define SetWindowLongPtrA(hwnd, index, value)                                                      \
    SetWindowLongA((hwnd), (index), (lh_long_t)(lh_ssize_t)(value))
#define GetWindowLongPtrA(hwnd, index)                                                             \
    ((lh_os_system_win_lparam_t)GetWindowLongA((hwnd), (index)))

#endif

#endif /* LH_SRC_OS_SYSTEM_WIN_USER32_H */
