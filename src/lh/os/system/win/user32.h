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

#include <lh/os/system/win/types.h>
#include <lh/os/system/win/window/msg/fields.h>
#include <lh/os/system/win/window/paintstruct/fields.h>
#include <lh/os/system/win/window/wndclassexw/fields.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

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
    lh_int_t x; /**< Horizontal coordinate. */
    lh_int_t y; /**< Vertical coordinate. */
};
/** @copybrief lh_os_system_win_point */
typedef struct lh_os_system_win_point lh_os_system_win_point_t;

/**
 * @struct lh_os_system_win_rect
 * @brief `RECT` (right/bottom exclusive for regions and `FillRect`).
 *        Present since Windows 95.
 */
struct lh_os_system_win_rect
{
    lh_int_t left;   /**< Left edge. */
    lh_int_t top;    /**< Top edge. */
    lh_int_t right;  /**< Right edge (exclusive). */
    lh_int_t bottom; /**< Bottom edge (exclusive). */
};
/** @copybrief lh_os_system_win_rect */
typedef struct lh_os_system_win_rect lh_os_system_win_rect_t;

/**
 * @struct lh_os_system_win_monitorinfo
 * @brief `MONITORINFO`: where a display is, and the part of it that is usable.
 *        Present since Windows 2000.
 */
struct lh_os_system_win_monitorinfo
{
    lh_os_system_win_dword_t cb_size; /**< `sizeof` the struct; set before the call */
    lh_os_system_win_rect_t monitor;  /**< the whole display, in virtual-screen coordinates */
    lh_os_system_win_rect_t work;     /**< the same minus taskbars and docked app bars */
    lh_os_system_win_dword_t flags;   /**< ::LH_OS_SYSTEM_WIN_MONITORINFOF_PRIMARY or 0 */
};
/** @copybrief lh_os_system_win_monitorinfo */
typedef struct lh_os_system_win_monitorinfo lh_os_system_win_monitorinfo_t;

/** `MONITORINFOF_PRIMARY`: this display is the one with the Start menu and taskbar.
 * A `MONITORINFO` asked for without the flag never has it set. */
#define LH_OS_SYSTEM_WIN_MONITORINFOF_PRIMARY 1

/** @{ */
/** `MonitorFromWindow` / `MonitorFromPoint`: which display a window or a point is
 * on, and what to answer when it is on none. The default-to flags are how a caller
 * says which screen "the one" means. */
#define LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTONULL 0
#define LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTOPRIMARY 1
#define LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTONEAREST 2
/** @} */

/** The display @p hwnd is on. Present since Windows 2000. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
MonitorFromWindow(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t flags);

/** The display @p point is on. Present since Windows 2000. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
MonitorFromPoint(lh_os_system_win_point_t point, lh_os_system_win_dword_t flags);

/** Where a display is and how much of it is usable, into @p info.
 *  Present since Windows 2000. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetMonitorInfoA(lh_os_system_win_handle_t monitor, lh_os_system_win_monitorinfo_t *info);

/**
 * @struct lh_os_system_win_msg
 * @brief `MSG`. Present since Windows 95.
 *
 * `pt` is flattened to two integers (same layout as Win32 `POINT`).
 */
struct lh_os_system_win_msg
{
    /** Members: ::lh_os_system_win_msg_fields. */
    lh_os_system_win_msg_fields(lh_os_system_win_hwnd_t, lh_os_system_win_dword_t,
                                lh_os_system_win_wparam_t, lh_os_system_win_lparam_t);
};
/** @copybrief lh_os_system_win_msg */
typedef struct lh_os_system_win_msg lh_os_system_win_msg_t;

/**
 * @struct lh_os_system_win_paintstruct
 * @brief `PAINTSTRUCT`. Present since Windows 95.
 */
struct lh_os_system_win_paintstruct
{
    /** Members: ::lh_os_system_win_paintstruct_fields. */
    lh_os_system_win_paintstruct_fields(lh_os_system_win_hdc_t, lh_os_system_win_bool_t,
                                        lh_os_system_win_rect_t, lh_byte_t);
};
/** @copybrief lh_os_system_win_paintstruct */
typedef struct lh_os_system_win_paintstruct lh_os_system_win_paintstruct_t;

/**
 * @struct lh_os_system_win_wndclassexa
 * @brief `WNDCLASSEXA`. Present since Windows 95 (`WNDCLASSEX`).
 */
struct lh_os_system_win_wndclassexa
{
    /** Members: ::lh_os_system_win_wndclassexw_fields. */
    lh_os_system_win_wndclassexw_fields(lh_os_system_win_dword_t, lh_int_t,
                                        lh_os_system_win_wndproc_t);
};
/** @copybrief lh_os_system_win_wndclassexa */
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
    lh_ptr lpCreateParams;                  /**< `lpParam` of ::CreateWindowExA. */
    lh_os_system_win_hinstance_t hInstance; /**< Module that owns the window. */
    lh_os_system_win_hgdi_t hMenu;          /**< Menu or child id. */
    lh_os_system_win_hwnd_t hwndParent;     /**< Parent or owner window. */
    lh_int_t cy;                            /**< Height. */
    lh_int_t cx;                            /**< Width. */
    lh_int_t y;                             /**< Top. */
    lh_int_t x;                             /**< Left. */
    lh_long_t style;                        /**< `WS_*` style. */
    lh_str_cptr lpszName;                   /**< Window title. */
    lh_str_cptr lpszClass;                  /**< Class name. */
    lh_os_system_win_dword_t dwExStyle;     /**< `WS_EX_*` style. */
};
/** @copybrief lh_os_system_win_createstructa */
typedef struct lh_os_system_win_createstructa lh_os_system_win_createstructa_t;

/** @{ */
/** BOOL literals. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_FALSE 0
#define LH_OS_SYSTEM_WIN_TRUE 1
/** @} */

/** @{ */
/** Class styles. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_CS_VREDRAW 0x0001U
#define LH_OS_SYSTEM_WIN_CS_HREDRAW 0x0002U
/** @} */

/** Extended window styles. Present since Windows 95. `WS_EX_APPWINDOW` puts a
   frameless window in the taskbar in its own right; a `WS_POPUP` window without it
   is invisible there. */
#define LH_OS_SYSTEM_WIN_WS_EX_APPWINDOW 0x00040000UL

/** @{ */
/** Window styles. Present since Windows 95. `WS_POPUP` alone is the frame of last
   resort: the OS draws nothing, so the window is only what the app paints. */
#define LH_OS_SYSTEM_WIN_WS_POPUP 0x80000000UL
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPED 0x00000000UL
#define LH_OS_SYSTEM_WIN_WS_CAPTION 0x00C00000UL
#define LH_OS_SYSTEM_WIN_WS_SYSMENU 0x00080000UL
#define LH_OS_SYSTEM_WIN_WS_THICKFRAME 0x00040000UL
#define LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX 0x00020000UL
#define LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX 0x00010000UL
#define LH_OS_SYSTEM_WIN_WS_OVERLAPPEDWINDOW                                                       \
    (LH_OS_SYSTEM_WIN_WS_OVERLAPPED | LH_OS_SYSTEM_WIN_WS_CAPTION | LH_OS_SYSTEM_WIN_WS_SYSMENU |  \
     LH_OS_SYSTEM_WIN_WS_THICKFRAME | LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX |                            \
     LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX)
/** @} */

/** @{ */
/** ShowWindow commands. Present since Windows 95. `SW_MAXIMIZE` fills the screen with
   the window and `SW_RESTORE` puts it back the way it was; both work on a frameless
   window, which has no maximize box of its own to press. */
#define LH_OS_SYSTEM_WIN_SW_SHOW 5
#define LH_OS_SYSTEM_WIN_SW_MINIMIZE 6
#define LH_OS_SYSTEM_WIN_SW_MAXIMIZE 3
#define LH_OS_SYSTEM_WIN_SW_RESTORE 9
/** @} */

/** @{ */
/** `WM_SIZE`: the window changed size, and this is why. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_WM_SIZE 0x0005U
#define LH_OS_SYSTEM_WIN_SIZE_RESTORED 0
#define LH_OS_SYSTEM_WIN_SIZE_MINIMIZED 1
#define LH_OS_SYSTEM_WIN_SIZE_MAXIMIZED 2
/** @} */

/** CreateWindow position. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_CW_USEDEFAULT (lh_cast_static(lh_int_t, 0x80000000))

/** Stock cursor id as `MakeIntResourceA(32512)`. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_IDC_ARROW (lh_cast_static(lh_str_cptr, lh_cast_static(lh_usize_t, 32512)))

/** Window long offsets. Present since Windows 95 (`GWL_USERDATA`). */
#define LH_OS_SYSTEM_WIN_GWLP_USERDATA (-21)

/** @{ */
/** Messages. Present since Windows 95 unless noted. */
#define LH_OS_SYSTEM_WIN_WM_DESTROY 0x0002U
#define LH_OS_SYSTEM_WIN_WM_PAINT 0x000FU
#define LH_OS_SYSTEM_WIN_WM_TIMER 0x0113U
#define LH_OS_SYSTEM_WIN_WM_ERASEBKGND 0x0014U
#define LH_OS_SYSTEM_WIN_WM_NCCREATE 0x0081U
#define LH_OS_SYSTEM_WIN_WM_NCHITTEST 0x0084U
#define LH_OS_SYSTEM_WIN_WM_QUIT 0x0012U
#define LH_OS_SYSTEM_WIN_WM_MOUSEMOVE 0x0200U
#define LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN 0x0201U
#define LH_OS_SYSTEM_WIN_WM_LBUTTONUP 0x0202U
#define LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN 0x0204U
#define LH_OS_SYSTEM_WIN_WM_RBUTTONUP 0x0205U
#define LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN 0x0207U
#define LH_OS_SYSTEM_WIN_WM_MBUTTONUP 0x0208U
/** @} */
/** @{ */
/** `WM_XBUTTONDOWN` / `WM_XBUTTONUP`: the side buttons, which one in the high word of
   `wParam` (::LH_OS_SYSTEM_WIN_XBUTTON1, ::LH_OS_SYSTEM_WIN_XBUTTON2). Present since
   Windows 2000. */
#define LH_OS_SYSTEM_WIN_WM_XBUTTONDOWN 0x020BU
#define LH_OS_SYSTEM_WIN_WM_XBUTTONUP 0x020CU
#define LH_OS_SYSTEM_WIN_XBUTTON1 0x0001U
#define LH_OS_SYSTEM_WIN_XBUTTON2 0x0002U
/** @} */
/** `WM_MOUSEHWHEEL`: the sideways wheel, positive to the right. Present since Windows
   Vista; XP never sends it. */
#define LH_OS_SYSTEM_WIN_WM_MOUSEHWHEEL 0x020EU
/** `WM_CAPTURECHANGED`: this window lost the mouse capture; `lParam` is the window
   that took it. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_WM_CAPTURECHANGED 0x0215U
/** @{ */
/** `WM_SETFOCUS` / `WM_KILLFOCUS`: the keyboard focus came or went. Present since
   Windows 95. */
#define LH_OS_SYSTEM_WIN_WM_SETFOCUS 0x0007U
#define LH_OS_SYSTEM_WIN_WM_KILLFOCUS 0x0008U
/** @} */
/** `WM_MOUSEWHEEL`. Present since Windows 98 / Windows NT 4.0 SP3. */
#define LH_OS_SYSTEM_WIN_WM_MOUSEWHEEL 0x020AU

/** `WHEEL_DELTA`: one notch of `WM_MOUSEWHEEL`. Present since Windows 98 / NT 4.0. */
#define LH_OS_SYSTEM_WIN_WHEEL_DELTA 120

/** @{ */
/** Keyboard messages. Present since Windows 95. With an ANSI window class,
   `WM_CHAR` carries one byte of the ANSI code page. */
#define LH_OS_SYSTEM_WIN_WM_KEYDOWN 0x0100U
#define LH_OS_SYSTEM_WIN_WM_KEYUP 0x0101U
#define LH_OS_SYSTEM_WIN_WM_CHAR 0x0102U
/** @} */
/** @{ */
/** `WM_SYSKEYDOWN` / `WM_SYSKEYUP`: a key with Alt held, and F10. Present since Windows
   95. `DefWindowProc` must still see them, or Alt+F4 and the system menu stop working. */
#define LH_OS_SYSTEM_WIN_WM_SYSKEYDOWN 0x0104U
#define LH_OS_SYSTEM_WIN_WM_SYSKEYUP 0x0105U
/** @} */

/** @{ */
/** Virtual-key codes. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_VK_BACK 0x08U
#define LH_OS_SYSTEM_WIN_VK_TAB 0x09U
#define LH_OS_SYSTEM_WIN_VK_RETURN 0x0DU
#define LH_OS_SYSTEM_WIN_VK_SHIFT 0x10U
#define LH_OS_SYSTEM_WIN_VK_CONTROL 0x11U
#define LH_OS_SYSTEM_WIN_VK_MENU 0x12U
#define LH_OS_SYSTEM_WIN_VK_ESCAPE 0x1BU
#define LH_OS_SYSTEM_WIN_VK_SPACE 0x20U
#define LH_OS_SYSTEM_WIN_VK_PRIOR 0x21U
#define LH_OS_SYSTEM_WIN_VK_NEXT 0x22U
#define LH_OS_SYSTEM_WIN_VK_END 0x23U
#define LH_OS_SYSTEM_WIN_VK_HOME 0x24U
#define LH_OS_SYSTEM_WIN_VK_LEFT 0x25U
#define LH_OS_SYSTEM_WIN_VK_UP 0x26U
#define LH_OS_SYSTEM_WIN_VK_RIGHT 0x27U
#define LH_OS_SYSTEM_WIN_VK_DOWN 0x28U
#define LH_OS_SYSTEM_WIN_VK_DELETE 0x2EU
#define LH_OS_SYSTEM_WIN_VK_CAPITAL 0x14U
#define LH_OS_SYSTEM_WIN_VK_LWIN 0x5BU
#define LH_OS_SYSTEM_WIN_VK_RWIN 0x5CU
#define LH_OS_SYSTEM_WIN_VK_NUMLOCK 0x90U
/** @} */

/** @{ */
/** Mouse-message key state (low word of `wParam`): the buttons and keys held. Present
   since Windows 95; the X buttons since Windows 2000. */
#define LH_OS_SYSTEM_WIN_MK_LBUTTON 0x0001U
#define LH_OS_SYSTEM_WIN_MK_RBUTTON 0x0002U
#define LH_OS_SYSTEM_WIN_MK_SHIFT 0x0004U
#define LH_OS_SYSTEM_WIN_MK_CONTROL 0x0008U
#define LH_OS_SYSTEM_WIN_MK_MBUTTON 0x0010U
#define LH_OS_SYSTEM_WIN_MK_XBUTTON1 0x0020U
#define LH_OS_SYSTEM_WIN_MK_XBUTTON2 0x0040U
/** @} */

/** PeekMessage. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_PM_REMOVE 0x0001U

/** MsgWaitForMultipleObjects wake mask. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_QS_ALLINPUT 0x04FFU

/** @{ */
/** Wait result. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_INFINITE 0xFFFFFFFFUL
#define LH_OS_SYSTEM_WIN_WAIT_TIMEOUT 258UL
/** @} */

/** `LOWORD`: the low 16 bits of @p v. */
#define LH_OS_SYSTEM_WIN_LOWORD(v)                                                                 \
    (lh_cast_static(lh_os_system_win_word_t, (lh_cast_static(lh_usize_t, (v)) & 0xffffU)))
/** `HIWORD`: bits 16..31 of @p v. */
#define LH_OS_SYSTEM_WIN_HIWORD(v)                                                                 \
    (lh_cast_static(lh_os_system_win_word_t, ((lh_cast_static(lh_usize_t, (v)) >> 16) & 0xffffU)))
/** `GET_WHEEL_DELTA_WPARAM`: the signed wheel distance in @p w, in ::LH_OS_SYSTEM_WIN_WHEEL_DELTA
 * units. */
#define LH_OS_SYSTEM_WIN_GET_WHEEL_DELTA_WPARAM(w)                                                 \
    (lh_cast_static(lh_sshort_t, LH_OS_SYSTEM_WIN_HIWORD(w)))

/** Register a window class; 0 on failure. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_atom_t LH_OS_SYSTEM_WIN_CALL
RegisterClassExA(const lh_os_system_win_wndclassexa_t *wc);

/** A stock or resource cursor (::LH_OS_SYSTEM_WIN_IDC_ARROW). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hgdi_t LH_OS_SYSTEM_WIN_CALL
LoadCursorA(lh_os_system_win_hinstance_t instance, lh_str_cptr name);

/** Create a window of a registered class. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
CreateWindowExA(lh_os_system_win_dword_t dwExStyle, lh_str_cptr lpClassName,
                lh_str_cptr lpWindowName, lh_os_system_win_dword_t dwStyle, lh_int_t x, lh_int_t y,
                lh_int_t nWidth, lh_int_t nHeight, lh_os_system_win_hwnd_t hWndParent,
                lh_os_system_win_hgdi_t hMenu, lh_os_system_win_hinstance_t hInstance,
                lh_ptr lpParam);

/** Show, hide, minimize, maximize or restore a window (`SW_*`). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ShowWindow(lh_os_system_win_hwnd_t hWnd, lh_int_t nCmdShow);

/** Paint a window now if it has an update region. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
UpdateWindow(lh_os_system_win_hwnd_t hWnd);

/** Destroy a window (`WM_DESTROY` follows). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DestroyWindow(lh_os_system_win_hwnd_t hWnd);

/** Enable or disable mouse and keyboard input to a window. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EnableWindow(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_bool_t bEnable);

/** Grow a client rectangle to the window rectangle a style needs. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
AdjustWindowRect(lh_os_system_win_rect_t *lpRect, lh_os_system_win_dword_t dwStyle,
                 lh_os_system_win_bool_t bMenu);

/** The client area, with its top-left at (0, 0). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetClientRect(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_rect_t *lpRect);

/** Add a rectangle (or the whole client when null) to the update region. Present since Windows 95.
 */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
InvalidateRect(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_rect_t *lpRect,
               lh_os_system_win_bool_t bErase);

/** Start painting the update region; the DC is valid until ::EndPaint. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
BeginPaint(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_paintstruct_t *lpPaint);

/** End painting started by ::BeginPaint. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
EndPaint(lh_os_system_win_hwnd_t hWnd, const lh_os_system_win_paintstruct_t *lpPaint);

/** Send every mouse message to this window until released. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL
SetCapture(lh_os_system_win_hwnd_t hWnd);

/** Timers. Present since Windows 95. `SetTimer` returns the new timer id, and returns 0
   when the timer could not be made -- which is why the caller has to check it rather
   than trust a window that has simply gone quiet. `lpTimerFunc` is NULL here because
   the callback is a message, not a function: a window that wants to be called back is
   sent WM_TIMER and the WndProc dispatches it, which is the only arrangement that keeps
   every callback on one thread. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_uintptr_t LH_OS_SYSTEM_WIN_CALL
SetTimer(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_uintptr_t nIDEvent,
         lh_os_system_win_uint_t uElapse, lh_ptr lpTimerFunc);

/** Destroy a timer made by ::SetTimer. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
KillTimer(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_uintptr_t nIDEvent);

/** End the mouse capture of ::SetCapture. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL ReleaseCapture(lh_void);

/** Where a window's own corners are, in screen coordinates; only its size matters
   here. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetWindowRect(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_rect_t *lpRect);

/** @{ */
/** Hit-test answers. A window that draws its own frame answers `WM_NCHITTEST` itself,
   and `DefWindowProc` turns a press on one of these into the matching system command
   — `SC_MOVE` or `SC_SIZE` — which is the whole of moving and resizing a frameless
   window, cursors included. All present since Windows 95. */
#define LH_OS_SYSTEM_WIN_HTCLIENT 1
#define LH_OS_SYSTEM_WIN_HTCAPTION 2
#define LH_OS_SYSTEM_WIN_HTTOP 12
#define LH_OS_SYSTEM_WIN_HTBOTTOM 15
#define LH_OS_SYSTEM_WIN_HTRIGHT 11
#define LH_OS_SYSTEM_WIN_HTLEFT 10
#define LH_OS_SYSTEM_WIN_HTTOPLEFT 13
#define LH_OS_SYSTEM_WIN_HTTOPRIGHT 14
#define LH_OS_SYSTEM_WIN_HTBOTTOMLEFT 16
#define LH_OS_SYSTEM_WIN_HTBOTTOMRIGHT 17
/** @} */

/** Cut the window to a region, so the corners the caller does not fill are really
   not there — the only way to round a window's corners on Windows XP, where there
   is no composition to blur them with. Present since Windows 95. The window takes
   ownership of @p region and deletes it, so handing it lh_null is how a window goes
   back to square. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
SetWindowRgn(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_handle_t hRgn,
             lh_os_system_win_bool_t bRedraw);

/** The state of a key when the current message was posted: the high bit is down, the
   low bit is toggled on (Caps Lock, Num Lock). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_sshort_t LH_OS_SYSTEM_WIN_CALL
GetKeyState(lh_int_t nVirtKey);

/** The window that has the mouse capture, or null. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hwnd_t LH_OS_SYSTEM_WIN_CALL GetCapture(lh_void);

/** Turn a screen point into a window's client coordinates. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ScreenToClient(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_point_t *lpPoint);

/** Take (`PM_REMOVE`) or look at the next message without waiting. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
PeekMessageA(lh_os_system_win_msg_t *lpMsg, lh_os_system_win_hwnd_t hWnd,
             lh_os_system_win_uint_t wMsgFilterMin, lh_os_system_win_uint_t wMsgFilterMax,
             lh_os_system_win_uint_t wRemoveMsg);

/** Turn key messages into `WM_CHAR`. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
TranslateMessage(const lh_os_system_win_msg_t *lpMsg);

/** Send a message to its window procedure. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DispatchMessageA(const lh_os_system_win_msg_t *lpMsg);

/** Post `WM_QUIT` to this thread's queue. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_void LH_OS_SYSTEM_WIN_CALL
PostQuitMessage(lh_int_t nExitCode);

/** Wait for handles or for input in the queue (`QS_*`). Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
MsgWaitForMultipleObjects(lh_os_system_win_dword_t nCount,
                          const lh_os_system_win_handle_t *pHandles,
                          lh_os_system_win_bool_t bWaitAll, lh_os_system_win_dword_t dwMilliseconds,
                          lh_os_system_win_dword_t dwWakeMask);

/** Default handling of a message the window procedure does not handle. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
DefWindowProcA(lh_os_system_win_hwnd_t hWnd, lh_os_system_win_uint_t Msg,
               lh_os_system_win_wparam_t wParam, lh_os_system_win_lparam_t lParam);

#if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

/** Set a pointer-sized window value (`GWLP_*`). Present since Windows 2000 (64-bit). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lparam_t LH_OS_SYSTEM_WIN_CALL
SetWindowLongPtrA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex,
                  lh_os_system_win_lparam_t dwNewLong);

/** Read a pointer-sized window value (`GWLP_*`). Present since Windows 2000 (64-bit). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_lparam_t LH_OS_SYSTEM_WIN_CALL
GetWindowLongPtrA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex);

#else

/* On x86 `SetWindowLongPtrA` is a macro for `SetWindowLongA` — XP has no Ptr export. */
LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
SetWindowLongA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex, lh_long_t dwNewLong);

LH_OS_SYSTEM_WIN_IMPORT lh_long_t LH_OS_SYSTEM_WIN_CALL
GetWindowLongA(lh_os_system_win_hwnd_t hWnd, lh_int_t nIndex);

#    define SetWindowLongPtrA(hwnd, index, value)                                                  \
        SetWindowLongA((hwnd), (index),                                                            \
                       lh_cast_static(lh_long_t, lh_cast_static(lh_ssize_t, (value))))
#    define GetWindowLongPtrA(hwnd, index)                                                         \
        (lh_cast_static(lh_os_system_win_lparam_t, GetWindowLongA((hwnd), (index))))

#endif

#endif /* LH_SRC_OS_SYSTEM_WIN_USER32_H */
