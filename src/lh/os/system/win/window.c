/**
 * @file window.c
 * @brief Win32 backend for `lh/os/system/window.h` — XP-clean.
 *
 * One window class registered on first use and unregistered on
 * `lh_os_system_window_close`; multiple windows of the same class are
 * allowed. The class name is fixed ("lh_pa_window") because the OS needs a
 * stable, globally-unique identifier per process to dispatch `WndProc` —
 * generating one per window would require keeping an atom map, with no
 * payoff for the (currently single-class) setup.
 *
 * On rejection (`<= XP`): no `WM_INPUT`, no `WM_TOUCH`, no
 * `SetProcessDPIAware`. Mouse input via `WM_LBUTTONDOWN` /
 * `WM_MOUSEMOVE`; key input via `WM_KEYDOWN` / `WM_KEYUP`; size via
 * `WM_SIZE`. All work as documented since Win95 / NT4.
 */

#include <lh/cast/static.h>
#include <lh/color.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/window.h>
#include <lh/os/system/win/geom.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/wchar.h>
#include <lh/wstr.h>
#include <lh/wstr/ptr.h>

#include <stddef.h>

/* Project orange (#FF7A18) — the only color we paint with. `lh_color_t` is
   OS-portable; converted to Win32's `COLORREF` at the API boundary via
   `lh_os_system_win_color_to_lh`. Same orange across all three backends. */
static const lh_color_t lh_os_system_win_brush_color = { 0xFF, 0x7A, 0x18, 0xFF };

/* Window class name — wide string literal; stored as `LPCWSTR` in
   `WNDCLASSEXW::lpszClassName`. Stable identifier for the OS dispatch. */
static const wchar_t lh_os_system_win_window_class_name[] = L"lh_pa_window";

/* `WndProc` — must have external linkage for `WNDCLASSEXW::lpfnWndProc`. */
static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam);

/* Round-trip a stored `lh_ssize_t` window handle back to the Win32 HWND.
   Lives here so the public API never names `HWND`; reusable by any future
   Win32 backend code that holds an `lh_os_system_window_handle_t`. */
lh_os_system_win_hwnd_t
lh_os_system_win_window_native(lh_os_system_window_handle_t self)
{
    return (lh_os_system_win_hwnd_t)(lh_ptr)lh_cast_static(lh_ssize_t, self);
}

/* Track whether the class has been registered in this process; refcounted
   close so two windows don't accidentally unregister while one is still
   alive. */
static lh_int_t lh_os_system_win_window_class_refcount;

static lh_bool_t
lh_os_system_win_window_register_class(void)
{
    if (lh_os_system_win_window_class_refcount > 0)
    {
        lh_os_system_win_window_class_refcount += 1;
        return lh_bool_true;
    }

    lh_os_system_win_wndclassexw_t wc;
    wc.cbSize = lh_cast_static(lh_os_system_win_dword_t, sizeof wc);
    wc.style = LH_OS_SYSTEM_WIN_CS_HREDRAW | LH_OS_SYSTEM_WIN_CS_VREDRAW;
    wc.lpfnWndProc = lh_os_system_win_window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = lh_null;
    wc.hIcon = lh_null;
    wc.hCursor = LoadCursorW(LH_OS_SYSTEM_WIN_HINSTANCE_NULL,
                             LH_OS_SYSTEM_WIN_MAKEINTRESOURCE(LH_OS_SYSTEM_WIN_IDC_ARROW));
    wc.hbrBackground = lh_null;
    wc.lpszMenuName = lh_null;
    /* `lh_addr_of` keeps this a typed pointer to the wide string; the
       `lh_ptr_rcast` widens it to `lh_ptr` (same memory layout). */
    wc.lpszClassName = lh_ptr_rcast(lh_uchar_t, lh_addr_of(lh_os_system_win_window_class_name));
    wc.hIconSm = lh_null;

    if (RegisterClassExW(&wc) == 0)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }

    lh_os_system_win_window_class_refcount = 1;
    return lh_bool_true;
}

static void
lh_os_system_win_window_unregister_class(void)
{
    if (lh_os_system_win_window_class_refcount <= 0)
    {
        return;
    }
    lh_os_system_win_window_class_refcount -= 1;
    if (lh_os_system_win_window_class_refcount == 0)
    {
        (void)UnregisterClassW(lh_os_system_win_window_class_name, LH_OS_SYSTEM_WIN_HINSTANCE_NULL);
    }
}

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_ptr title, lh_int_t width, lh_int_t height)
{
    if (width <= 0 || height <= 0)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    if (!lh_os_system_win_window_register_class())
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* `CreateWindowExW` returns `NULL` (== `0` as int) on failure. */
    lh_os_system_win_hwnd_t hwnd = CreateWindowExW(
        lh_cast_static(lh_os_system_win_dword_t, 0),
        lh_os_system_win_window_class_name,
        (lh_wstr_cptr)title,
        LH_OS_SYSTEM_WIN_WS_OVERLAPPED | LH_OS_SYSTEM_WIN_WS_CAPTION | LH_OS_SYSTEM_WIN_WS_SYSMENU
            | LH_OS_SYSTEM_WIN_WS_THICKFRAME | LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX
            | LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX,
        LH_OS_SYSTEM_WIN_CW_USEDEFAULT, LH_OS_SYSTEM_WIN_CW_USEDEFAULT,
        width, height,
        LH_OS_SYSTEM_WIN_HWND_NULL, lh_null, LH_OS_SYSTEM_WIN_HINSTANCE_NULL, lh_null);

    if (lh_null_eq(hwnd))
    {
        lh_os_system_error_capture();
        lh_os_system_win_window_unregister_class();
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* Bit-pattern round-trip: store the Win32 pointer as `lh_ssize_t`. */
    return lh_cast_static(lh_os_system_window_handle_t,
                           lh_cast_static(lh_ssize_t, (lh_ptr)hwnd));
}

void
lh_os_system_window_close(lh_os_system_window_handle_t self)
{
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        return;
    }
    (void)DestroyWindow(lh_os_system_win_window_native(self));
    lh_os_system_win_window_unregister_class();
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t self)
{
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    /* `NULL` (== `0` as HWND-as-int) is also "no window". */
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_NULL))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

void
lh_os_system_window_show(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    (void)ShowWindow(lh_os_system_win_window_native(self), LH_OS_SYSTEM_WIN_SW_SHOW);
}

lh_bool_t
lh_os_system_window_pump_messages(void)
{
    lh_os_system_win_msg_t msg;
    /* `PM_REMOVE` peek-and-pull: dispatch paint / key / mouse / close to
       WndProc; observe `WM_QUIT` (posted by `WM_CLOSE`) and return true so
       the caller's run loop exits. */
    while (PeekMessageW(&msg, LH_OS_SYSTEM_WIN_HWND_NULL,
                        lh_cast_static(lh_os_system_win_dword_t, 0),
                        lh_cast_static(lh_os_system_win_dword_t, 0),
                        LH_OS_SYSTEM_WIN_PM_REMOVE) != lh_bool_false)
    {
        if (msg.message == LH_OS_SYSTEM_WIN_WM_QUIT)
        {
            return lh_bool_true;
        }
        (void)TranslateMessage(&msg);
        (void)DispatchMessageW(&msg);
    }
    return lh_bool_false;
}

static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam)
{
    (void)wparam;
    (void)lparam;

    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_PAINT: {
        lh_os_system_win_paintstruct_t ps;
        lh_os_system_win_hdc_t dc = BeginPaint(hwnd, lh_addr_of(ps));
        lh_os_system_win_hbrush_t brush = CreateSolidBrush(
            lh_os_system_win_color_to_lh(lh_os_system_win_brush_color));
        (void)FillRect(dc, lh_addr_of(ps.rcPaint), brush);
        (void)DeleteObject((lh_ptr)brush);
        (void)EndPaint(hwnd, lh_addr_of(ps));
        return lh_cast_static(lh_os_system_win_lresult_t, 0);
    }
    case LH_OS_SYSTEM_WIN_WM_CLOSE:
        PostQuitMessage(0);
        return lh_cast_static(lh_os_system_win_lresult_t, 0);
    case LH_OS_SYSTEM_WIN_WM_DESTROY:
        /* Closing one window should not exit the process; only `WM_CLOSE`
           followed by `DestroyWindow` calls `PostQuitMessage`. */
        return lh_cast_static(lh_os_system_win_lresult_t, 0);
    default:
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}