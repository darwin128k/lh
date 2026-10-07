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

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner, int title_height, int corner)
{
    lh_os_system_win_hinstance_t instance;
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_hwnd_t owner_hwnd;
    lh_os_system_win_rect_t rect;
    lh_os_system_win_dword_t style;
    lh_os_system_win_dword_t ex_style;
    lh_bool_t own_chrome;

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
    /* A title bar of our own means no OS frame at all. WS_POPUP draws nothing, so
       the client is the whole window and width x height stays the client size with
       no AdjustWindowRect to undo; WS_EX_APPWINDOW keeps it in the taskbar, which
       WS_POPUP alone would drop from the taskbar entirely. */
    own_chrome = lh_cast_static(lh_bool_t, title_height > 0);
    style = own_chrome ? LH_OS_SYSTEM_WIN_WS_POPUP : LH_OS_SYSTEM_WIN_WS_OVERLAPPEDWINDOW;
    ex_style = own_chrome ? LH_OS_SYSTEM_WIN_WS_EX_APPWINDOW : 0;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(lh_addr_of(rect), style, LH_OS_SYSTEM_WIN_FALSE);

    hwnd = CreateWindowExA(ex_style, LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME, title, style,
                           LH_OS_SYSTEM_WIN_CW_USEDEFAULT, LH_OS_SYSTEM_WIN_CW_USEDEFAULT,
                           rect.right - rect.left, rect.bottom - rect.top, owner_hwnd, lh_null,
                           instance, user);
    if (hwnd == lh_null)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    if (corner > 0)
    {
        /* The region is in window coordinates, which for WS_POPUP are the client's:
           the cut lands exactly on the corner the app did not paint. right and
           bottom are exclusive here, hence +1, and the corner ellipses are asked
           for by their full width, hence corner * 2. The window keeps the region,
           so there is nothing to delete on this side. */
        lh_os_system_win_handle_t region =
            CreateRoundRectRgn(0, 0, width + 1, height + 1, corner * 2, corner * 2);

        if (lh_null_ne(region))
        {
            SetWindowRgn(hwnd, region, LH_OS_SYSTEM_WIN_TRUE);
        }
    }

    ShowWindow(hwnd, LH_OS_SYSTEM_WIN_SW_SHOW);
    UpdateWindow(hwnd);
    return lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
}

lh_void
lh_os_system_window_drag(lh_os_system_window_handle_t handle)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    /* WM_LBUTTONDOWN captured the window before the app heard about it; the move
       loop runs its own capture and does not expect ours. */
    if (GetCapture() == hwnd)
    {
        ReleaseCapture();
    }
    /* WM_NCLBUTTONDOWN over the caption is the OS's move loop: it blocks here until
       the mouse comes up, then returns and the app's pump carries on. Nothing of it
       has to be reimplemented to be correct at the screen edges and on the menu key. */
    SendMessageA(hwnd, LH_OS_SYSTEM_WIN_WM_NCLBUTTONDOWN,
                 (lh_os_system_win_wparam_t)LH_OS_SYSTEM_WIN_HTCAPTION, 0);
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
