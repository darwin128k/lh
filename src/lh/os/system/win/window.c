/**
 * @file window.c
 * @brief Win32 backend for `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/os/system/win/key.h>
#include <lh/os/system/win/user32.h>
#include <lh/os/system/window.h>
#include <lh/os/window.h>
#include <lh/timer/tick.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#define LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME "lh_os_window"

static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam);

static lh_os_system_win_atom_t
lh_os_system_win_window_register_class(lh_os_system_win_hinstance_t instance)
{
    lh_os_system_win_wndclassexa_t wc;
    static lh_os_system_win_atom_t atom;

    if (atom != 0)
    {
        return atom;
    }

    lh_memory_set(lh_addr_of(wc), sizeof(wc), 0);
    wc.cbSize = sizeof(wc);
    wc.style = LH_OS_SYSTEM_WIN_CS_HREDRAW | LH_OS_SYSTEM_WIN_CS_VREDRAW;
    wc.lpfnWndProc = lh_os_system_win_window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorA(lh_null, LH_OS_SYSTEM_WIN_IDC_ARROW);
    /* No class brush: the app paints the whole client; erase would flash. */
    wc.hbrBackground = lh_null;
    wc.lpszClassName = LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME;
    atom = RegisterClassExA(lh_addr_of(wc));
    return atom;
}

/* What ::lh_os_window_zone_t means to Windows. The one table, so a zone added to the
   portable enum cannot be forgotten here. */
static lh_s32_t
lh_os_system_win_window_hit(lh_os_window_zone_t zone)
{
    switch (zone)
    {
    case lh_os_window_zone_caption:
        return LH_OS_SYSTEM_WIN_HTCAPTION;
    case lh_os_window_zone_left:
        return LH_OS_SYSTEM_WIN_HTLEFT;
    case lh_os_window_zone_right:
        return LH_OS_SYSTEM_WIN_HTRIGHT;
    case lh_os_window_zone_top:
        return LH_OS_SYSTEM_WIN_HTTOP;
    case lh_os_window_zone_bottom:
        return LH_OS_SYSTEM_WIN_HTBOTTOM;
    case lh_os_window_zone_top_left:
        return LH_OS_SYSTEM_WIN_HTTOPLEFT;
    case lh_os_window_zone_top_right:
        return LH_OS_SYSTEM_WIN_HTTOPRIGHT;
    case lh_os_window_zone_bottom_left:
        return LH_OS_SYSTEM_WIN_HTBOTTOMLEFT;
    case lh_os_window_zone_bottom_right:
        return LH_OS_SYSTEM_WIN_HTBOTTOMRIGHT;
    case lh_os_window_zone_client:
    default:
        return LH_OS_SYSTEM_WIN_HTCLIENT;
    }
}

static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam)
{
    lh_os_window_t *window;

    if (msg == LH_OS_SYSTEM_WIN_WM_NCCREATE)
    {
        const lh_os_system_win_createstructa_t *cs =
            lh_ptr_rcast(const lh_os_system_win_createstructa_t, (lh_ptr)lparam);
        window = lh_ptr_rcast(lh_os_window_t, cs->lpCreateParams);
        SetWindowLongPtrA(hwnd, LH_OS_SYSTEM_WIN_GWLP_USERDATA, (lh_os_system_win_lparam_t)window);
        if (lh_null_ne(window))
        {
            window->handle = lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
        }
    }

    window = lh_cast_reinterpret(lh_os_window_t *,
                                 (lh_ptr)GetWindowLongPtrA(hwnd, LH_OS_SYSTEM_WIN_GWLP_USERDATA));

    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_NCHITTEST:
        if (lh_null_ne(window))
        {
            /* The point arrives in screen coordinates; the app thinks in client ones,
               and it is the app that knows what its own chrome is. */
            lh_os_system_win_point_t at;

            at.x = (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_LOWORD(lparam);
            at.y = (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(lparam);
            ScreenToClient(hwnd, lh_addr_of(at));
            return lh_os_system_win_window_hit(lh_os_window_zone_at(window, at.x, at.y));
        }
        break;
    case LH_OS_SYSTEM_WIN_WM_SIZE:
        if (lh_null_ne(window))
        {
            /* The low word of wParam says why the window changed size; the size it
               carries is the *window*, not the client, so with a frame of the system
               on it the two differ by the frame. The app draws into and hit-tests
               the client, so that is what a resize has to report, and GetClientRect
               is already the truth by the time this message arrives.

               It also fixes the first message of a window's life: creation sends its
               size before the one the window was created with is applied, and that
               message carries 0x0 for a window that is a moment later 800x600. A
               window with a frame of its own gets no WM_NCPAINT and no frame to hear
               about, so this is also the only way it learns that it is now a
               different size or that it is maximized. */
            lh_os_system_win_rect_t client;
            const int reason = (int)LH_OS_SYSTEM_WIN_LOWORD(wparam);

            if (GetClientRect(hwnd, lh_addr_of(client)))
            {
                lh_os_window_on_native_resize(
                    window, (int)client.right, (int)client.bottom,
                    lh_cast_static(lh_bool_t, reason == LH_OS_SYSTEM_WIN_SIZE_MAXIMIZED));
                /* The cut follows the size, and is gone while the window is maximized:
                   both are things about this window's shape, not about the app that
                   drew it, so they belong here rather than in every app that resizes. */
                lh_os_system_window_set_corner_radius(
                    lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd),
                    reason == LH_OS_SYSTEM_WIN_SIZE_MAXIMIZED ? 0 : window->corner);
            }
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_ERASEBKGND:
        /* Client is fully redrawn in WM_PAINT; skip the system fill. */
        return 1;
    case LH_OS_SYSTEM_WIN_WM_PAINT:
    {
        lh_os_system_win_paintstruct_t ps;
        lh_os_system_win_hdc_t hdc;

        hdc = BeginPaint(hwnd, lh_addr_of(ps));
        if (lh_null_ne(window))
        {
            lh_os_window_on_native_paint(window, lh_cast_reinterpret(lh_ptr, hdc), ps.rcPaint.left,
                                         ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
        }
        EndPaint(hwnd, lh_addr_of(ps));
        return 0;
    }
    case LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN:
        if (lh_null_ne(window))
        {
            SetCapture(hwnd);
            lh_os_window_on_native_press(window, (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_LOWORD(lparam),
                                         (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(lparam));
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_MOUSEMOVE:
        if (lh_null_ne(window))
        {
            lh_os_window_on_native_move(window, (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_LOWORD(lparam),
                                        (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(lparam));
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_LBUTTONUP:
        if (lh_null_ne(window))
        {
            if (GetCapture() == hwnd)
            {
                ReleaseCapture();
            }
            lh_os_window_on_native_release(window, (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_LOWORD(lparam),
                                           (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(lparam));
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_MOUSEWHEEL:
        if (lh_null_ne(window))
        {
            lh_os_system_win_point_t cursor;
            int delta;

            cursor.x = (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_LOWORD(lparam);
            cursor.y = (int)(lh_sshort_t)LH_OS_SYSTEM_WIN_HIWORD(lparam);
            ScreenToClient(hwnd, lh_addr_of(cursor));
            delta = (int)LH_OS_SYSTEM_WIN_GET_WHEEL_DELTA_WPARAM(wparam) / LH_OS_SYSTEM_WIN_WHEEL_DELTA;
            if ((LH_OS_SYSTEM_WIN_LOWORD(wparam) & LH_OS_SYSTEM_WIN_MK_SHIFT) != 0U)
            {
                lh_os_window_on_native_wheel(window, cursor.x, cursor.y, delta, 0);
                return 0;
            }
            lh_os_window_on_native_wheel(window, cursor.x, cursor.y, 0, delta);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_KEYDOWN:
    case LH_OS_SYSTEM_WIN_WM_KEYUP:
        if (lh_null_ne(window))
        {
            lh_os_window_on_native_key(window, lh_os_system_win_key_from_vk((lh_u32_t)wparam),
                                       msg == LH_OS_SYSTEM_WIN_WM_KEYDOWN ? lh_bool_true : lh_bool_false);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_CHAR:
        if (lh_null_ne(window))
        {
            const lh_u32_t code = lh_os_system_win_text_from_char((lh_u32_t)wparam);

            if (lh_os_system_win_is_text(code))
            {
                lh_os_window_on_native_text(window, code);
            }
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_DESTROY:
        if (lh_null_ne(window))
        {
            lh_os_window_on_native_destroy(window);
        }
        return 0;
    default:
        break;
    }

    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/* Where a window opens. Win32 has exactly one answer for this — `CW_USEDEFAULT`,
   a cascade from the top-left corner — and no call that means "in the middle of
   what the user is looking at", so that half is ours: the work area of the monitor
   the window opens on, which is the area the system itself calls usable.

   A modal child centres on the monitor its owner is on, because a dialog that
   lands on the other screen is not near the thing it belongs to. A window larger
   than that work area keeps its top-left corner inside it: the size the caller
   asked for is the size it gets, but a title bar off the edge is a window nobody
   can move. */
static lh_void
lh_os_system_win_window_centered_origin(const lh_os_system_win_rect_t *window_rect,
                                        lh_os_system_win_hwnd_t owner_hwnd, int *x, int *y)
{
    lh_os_system_win_monitorinfo_t monitor_info;
    lh_os_system_win_handle_t monitor;
    int centered_x;
    int centered_y;

    lh_memory_set(lh_addr_of(monitor_info), sizeof(monitor_info), 0);
    monitor_info.cb_size = lh_cast_static(lh_os_system_win_dword_t, sizeof(monitor_info));

    if (lh_null_ne(owner_hwnd))
    {
        monitor = MonitorFromWindow(owner_hwnd, LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTONEAREST);
    }
    else
    {
        lh_os_system_win_point_t point;

        point.x = 0;
        point.y = 0;
        /* Only the primary display starts at the top-left of the virtual screen,
           so asking which display holds that point with DEFAULTTOPRIMARY asks for
           the one with the taskbar and the Start menu on it. */
        monitor = MonitorFromPoint(point, LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTOPRIMARY);
    }

    if (lh_null_eq(monitor) ||
        GetMonitorInfoA(monitor, lh_addr_of(monitor_info)) == LH_OS_SYSTEM_WIN_FALSE)
    {
        /* Nothing said where to put it, so it goes where the system would have put
           it. A missing monitor is not a reason to open nothing. */
        *x = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;
        *y = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;
        return;
    }

    centered_x = monitor_info.work.left +
                 (monitor_info.work.right - monitor_info.work.left -
                  (window_rect->right - window_rect->left)) / 2;
    centered_y = monitor_info.work.top +
                 (monitor_info.work.bottom - monitor_info.work.top -
                  (window_rect->bottom - window_rect->top)) / 2;
    *x = centered_x > monitor_info.work.left ? centered_x : monitor_info.work.left;
    *y = centered_y > monitor_info.work.top ? centered_y : monitor_info.work.top;
}

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner, lh_os_window_frame_t frame, int corner,
                         lh_os_window_placement_t placement)
{
    lh_os_system_win_hinstance_t instance;
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_hwnd_t owner_hwnd;
    lh_os_system_win_rect_t rect;
    lh_os_system_win_dword_t style;
    lh_os_system_win_dword_t ex_style;
    lh_bool_t own_frame;
    int x = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;
    int y = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;

    lh_assert_runtime_ref(title);
    if (width <= 0 || height <= 0)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    instance = GetModuleHandleA(lh_null);
    if (lh_os_system_win_window_register_class(instance) == 0)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    owner_hwnd = lh_null_eq(owner) ? lh_null : lh_cast_reinterpret(lh_os_system_win_hwnd_t, owner);
    /* A frame of the caller's own means no OS frame at all. WS_POPUP draws nothing,
       so the client is the whole window and width x height stays the client size with
       no AdjustWindowRect to undo; WS_EX_APPWINDOW keeps it in the taskbar, which
       WS_POPUP alone would drop from the taskbar entirely. */
    own_frame = frame == lh_os_window_frame_own ? lh_bool_true : lh_bool_false;
    style = own_frame ? LH_OS_SYSTEM_WIN_WS_POPUP : LH_OS_SYSTEM_WIN_WS_OVERLAPPEDWINDOW;
    ex_style = own_frame ? LH_OS_SYSTEM_WIN_WS_EX_APPWINDOW : 0;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(lh_addr_of(rect), style, LH_OS_SYSTEM_WIN_FALSE);

    /* The outer rectangle is known by now, and that is what has to be centred: the
       work area is in screen coordinates, the same ones this call passes. */
    if (placement == lh_os_window_placement_center)
    {
        lh_os_system_win_window_centered_origin(lh_addr_of(rect), owner_hwnd, lh_addr_of(x), lh_addr_of(y));
    }

    hwnd = CreateWindowExA(ex_style, LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME, title, style, x, y,
                           rect.right - rect.left, rect.bottom - rect.top, owner_hwnd, lh_null,
                           instance, user);
    if (hwnd == lh_null)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    lh_os_system_window_set_corner_radius(lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd), corner);

    ShowWindow(hwnd, LH_OS_SYSTEM_WIN_SW_SHOW);
    UpdateWindow(hwnd);
    return lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
}

lh_void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t handle, int radius)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t window;
    lh_os_system_win_handle_t region;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    if (radius <= 0)
    {
        /* A region is the window's shape, and the shape of the size it was cut for: a
           window that keeps its corners round has to be cut again every time it
           changes size, or it stays the size it was cut at — a maximized window with
           the 800x600 region of its normal state is an 800x600 window in the corner of
           the screen. Radius 0 is therefore not "leave it alone" but "no cut": it is
           what a maximized window is, on every desktop. */
        SetWindowRgn(hwnd, lh_null, LH_OS_SYSTEM_WIN_TRUE);
        return;
    }
    if (GetWindowRect(hwnd, lh_addr_of(window)) == 0)
    {
        return;
    }
    /* The region is in window coordinates, whose origin is the window itself — so
       this cuts the corners of the window whatever its frame is, and with a frame of
       the caller's own, where window and client are the same rectangle, the cut lands
       exactly on the corner the app did not paint. right and bottom are exclusive,
       hence +1, and the corner ellipses are asked for by their full width, hence
       radius * 2. The window keeps the region; nothing here has to delete it. */
    region = CreateRoundRectRgn(0, 0, window.right - window.left + 1, window.bottom - window.top + 1,
                                radius * 2, radius * 2);
    if (lh_null_eq(region))
    {
        return;
    }
    SetWindowRgn(hwnd, region, LH_OS_SYSTEM_WIN_TRUE);
}

lh_void
lh_os_system_window_minimize(lh_os_system_window_handle_t handle)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    ShowWindow(hwnd, LH_OS_SYSTEM_WIN_SW_MINIMIZE);
}

lh_void
lh_os_system_window_set_maximized(lh_os_system_window_handle_t handle, lh_bool_t maximized)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    ShowWindow(hwnd, maximized ? LH_OS_SYSTEM_WIN_SW_MAXIMIZE : LH_OS_SYSTEM_WIN_SW_RESTORE);
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t handle)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(handle));
}

lh_void
lh_os_system_window_set_enabled(lh_os_system_window_handle_t handle, lh_bool_t enabled)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    EnableWindow(hwnd, enabled ? LH_OS_SYSTEM_WIN_TRUE : LH_OS_SYSTEM_WIN_FALSE);
}

lh_void
lh_os_system_window_close(lh_os_system_window_handle_t handle)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    DestroyWindow(hwnd);
}

lh_os_system_window_pump_result_t
lh_os_system_window_pump_wait(lh_tick_t timeout_ms)
{
    lh_os_system_win_msg_t msg;
    lh_os_system_win_dword_t wait;
    lh_os_system_win_dword_t result;
    lh_bool_t saw_message;

    wait = (timeout_ms == LH_TICK_T_MAX) ? LH_OS_SYSTEM_WIN_INFINITE
                                         : lh_cast_static(lh_os_system_win_dword_t, timeout_ms);
    result = MsgWaitForMultipleObjects(0, lh_null, LH_OS_SYSTEM_WIN_FALSE, wait,
                                       LH_OS_SYSTEM_WIN_QS_ALLINPUT);
    if (result == LH_OS_SYSTEM_WIN_WAIT_TIMEOUT)
    {
        return lh_os_system_window_pump_timeout;
    }

    saw_message = lh_bool_false;
    while (PeekMessageA(lh_addr_of(msg), lh_null, 0, 0, LH_OS_SYSTEM_WIN_PM_REMOVE))
    {
        if (msg.message == LH_OS_SYSTEM_WIN_WM_QUIT)
        {
            return lh_os_system_window_pump_quit;
        }
        TranslateMessage(lh_addr_of(msg));
        DispatchMessageA(lh_addr_of(msg));
        saw_message = lh_bool_true;
    }
    return saw_message ? lh_os_system_window_pump_message : lh_os_system_window_pump_timeout;
}

lh_void
lh_os_system_window_post_quit(void)
{
    PostQuitMessage(0);
}

lh_void
lh_os_system_window_invalidate(lh_os_system_window_handle_t handle)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    InvalidateRect(hwnd, lh_null, LH_OS_SYSTEM_WIN_FALSE);
}

lh_void
lh_os_system_window_invalidate_rect(lh_os_system_window_handle_t handle, int left, int top, int right,
                                    int bottom)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t area;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    area.left = left;
    area.top = top;
    area.right = right;
    area.bottom = bottom;
    InvalidateRect(hwnd, lh_addr_of(area), LH_OS_SYSTEM_WIN_FALSE);
}

lh_bool_t
lh_os_system_window_get_client_size(lh_os_system_window_handle_t handle, int *width, int *height)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t client;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(width) || lh_null_eq(height), lh_bool_false);
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    lh_return_if(GetClientRect(hwnd, lh_addr_of(client)) == 0, lh_bool_false);
    *width = client.right - client.left;
    *height = client.bottom - client.top;
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_position(lh_os_system_window_handle_t handle, int *x, int *y)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t window;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(x) || lh_null_eq(y), lh_bool_false);
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    lh_return_if(GetWindowRect(hwnd, lh_addr_of(window)) == 0, lh_bool_false);
    *x = window.left;
    *y = window.top;
    return lh_bool_true;
}
