/**
 * @file window.c
 * @brief Win32 backend for `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/window.h>
#include <lh/os/window.h>
#include <lh/timer/tick.h>
#include <lh/util/ptr.h>

#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME "lh_os_window"

static LRESULT CALLBACK
lh_os_system_win_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

static ATOM
lh_os_system_win_window_register_class(HINSTANCE instance)
{
    WNDCLASSEXA wc;
    static ATOM atom;

    if (atom != 0)
    {
        return atom;
    }

    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = lh_os_system_win_window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorA(lh_null, IDC_ARROW);
    wc.hbrBackground = lh_cast_reinterpret(HBRUSH, (lh_uaddr_t)(COLOR_WINDOW + 1));
    wc.lpszClassName = LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME;
    atom = RegisterClassExA(&wc);
    return atom;
}

static LRESULT CALLBACK
lh_os_system_win_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    lh_os_window_t *window;

    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTA *cs = lh_ptr_rcast(const CREATESTRUCTA, (lh_ptr)lparam);
        window = lh_ptr_rcast(lh_os_window_t, cs->lpCreateParams);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        if (lh_null_ne(window))
        {
            window->handle = lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
        }
    }

    window = lh_cast_reinterpret(lh_os_window_t *, (lh_ptr)GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
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
                         lh_os_system_window_handle_t owner)
{
    HINSTANCE instance;
    HWND hwnd;
    HWND owner_hwnd;
    RECT rect;
    DWORD style;

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

    owner_hwnd = lh_null_eq(owner) ? lh_null : lh_cast_reinterpret(HWND, owner);
    style = WS_OVERLAPPEDWINDOW;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect, style, FALSE);

    hwnd = CreateWindowExA(0, LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME, title, style, CW_USEDEFAULT,
                           CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, owner_hwnd,
                           lh_null, instance, user);
    if (hwnd == lh_null)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    return lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t handle)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(handle));
}

lh_void
lh_os_system_window_set_enabled(lh_os_system_window_handle_t handle, lh_bool_t enabled)
{
    HWND hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(HWND, handle);
    EnableWindow(hwnd, enabled ? TRUE : FALSE);
}

lh_void
lh_os_system_window_close(lh_os_system_window_handle_t handle)
{
    HWND hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(HWND, handle);
    DestroyWindow(hwnd);
}

lh_os_system_window_pump_result_t
lh_os_system_window_pump_wait(lh_tick_t timeout_ms)
{
    MSG msg;
    DWORD wait;
    DWORD result;
    lh_bool_t saw_message;

    wait = (timeout_ms == LH_TICK_T_MAX) ? INFINITE : lh_cast_static(DWORD, timeout_ms);
    result = MsgWaitForMultipleObjects(0, lh_null, FALSE, wait, QS_ALLINPUT);
    if (result == WAIT_TIMEOUT)
    {
        return lh_os_system_window_pump_timeout;
    }

    saw_message = lh_bool_false;
    while (PeekMessageA(&msg, lh_null, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return lh_os_system_window_pump_quit;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
        saw_message = lh_bool_true;
    }
    return saw_message ? lh_os_system_window_pump_message : lh_os_system_window_pump_timeout;
}

lh_void
lh_os_system_window_post_quit(void)
{
    PostQuitMessage(0);
}
