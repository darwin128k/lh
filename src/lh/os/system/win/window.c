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
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/window.h>
#include <lh/os/system/window/emit.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/runtime/allocator.h>
#include <lh/util/ptr.h>
#include <lh/wchar.h>
#include <lh/wstr.h>
#include <lh/wstr/ptr.h>

#include <stddef.h>

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
    wc.lpszClassName = lh_ptr_rcast(lh_byte_t, lh_addr_of(lh_os_system_win_window_class_name));
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
        lh_cast_static(lh_os_system_win_dword_t, 0), lh_os_system_win_window_class_name,
        (lh_wstr_cptr)title,
        LH_OS_SYSTEM_WIN_WS_OVERLAPPED | LH_OS_SYSTEM_WIN_WS_CAPTION | LH_OS_SYSTEM_WIN_WS_SYSMENU |
            LH_OS_SYSTEM_WIN_WS_THICKFRAME | LH_OS_SYSTEM_WIN_WS_MINIMIZEBOX |
            LH_OS_SYSTEM_WIN_WS_MAXIMIZEBOX,
        LH_OS_SYSTEM_WIN_CW_USEDEFAULT, LH_OS_SYSTEM_WIN_CW_USEDEFAULT, width, height,
        LH_OS_SYSTEM_WIN_HWND_NULL, lh_null, LH_OS_SYSTEM_WIN_HINSTANCE_NULL, lh_null);

    if (lh_null_eq(hwnd))
    {
        lh_os_system_error_capture();
        lh_os_system_win_window_unregister_class();
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* Bit-pattern round-trip: store the Win32 pointer as `lh_ssize_t`. */
    return lh_cast_static(lh_os_system_window_handle_t, lh_cast_static(lh_ssize_t, (lh_ptr)hwnd));
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

void
lh_os_system_window_wait_messages(void)
{
    (void)WaitMessage();
}

lh_bool_t
lh_os_system_window_present(lh_os_system_window_handle_t self, const lh_ptr pixels, lh_int_t stride,
                            lh_int_t x, lh_int_t y, lh_int_t width, lh_int_t height)
{
    if (!lh_os_system_window_is_valid(self) || lh_ptr_is_null(pixels) || width <= 0 ||
        height <= 0 || x < 0 || y < 0 || stride < x + width)
    {
        return lh_bool_false;
    }

    /* A DIB's 32-bit pixels are b, g, r, unused: copy the area over with red
       and blue swapped, top row first (hence the negative height below). */
    const lh_usize_t row_bytes = lh_cast_static(lh_usize_t, width) * 4U;
    const lh_usize_t stride_bytes = lh_cast_static(lh_usize_t, stride) * 4U;
    lh_byte_t *const bgrx = lh_ptr_rcast(
        lh_byte_t, lh_runtime_allocator_alloc(row_bytes * lh_cast_static(lh_usize_t, height)));
    const lh_byte_t *src_row = lh_ptr_rcast(const lh_byte_t, pixels) +
                                lh_cast_static(lh_usize_t, y) * stride_bytes +
                                lh_cast_static(lh_usize_t, x) * 4U;
    lh_byte_t *dst = bgrx;
    for (lh_int_t row = 0; row < height; ++row, src_row += stride_bytes)
    {
        const lh_byte_t *src = src_row;
        for (lh_int_t column = 0; column < width; ++column, src += 4, dst += 4)
        {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = 0;
        }
    }

    lh_os_system_win_bitmapinfoheader_t info;
    lh_memory_std_set(lh_addr_of(info), 0, sizeof info);
    info.biSize = lh_cast_static(lh_os_system_win_dword_t, sizeof info);
    info.biWidth = width;
    info.biHeight = -height;
    info.biPlanes = 1;
    info.biBitCount = 32;
    info.biCompression = LH_OS_SYSTEM_WIN_BI_RGB;

    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    const lh_os_system_win_hdc_t dc = GetDC(hwnd);
    lh_bool_t shown = lh_bool_false;
    if (lh_ptr_is_set(dc))
    {
        shown = SetDIBitsToDevice(dc, x, y, lh_cast_static(lh_os_system_win_dword_t, width),
                                  lh_cast_static(lh_os_system_win_dword_t, height), 0, 0, 0,
                                  lh_cast_static(lh_os_system_win_uint_t, height), bgrx,
                                  lh_addr_of(info), LH_OS_SYSTEM_WIN_DIB_RGB_COLORS) != 0;
        (void)ReleaseDC(hwnd, dc);
    }
    lh_runtime_allocator_free(bgrx);
    return shown;
}

/* The window handle as the public API stores it. */
static lh_os_system_window_handle_t
lh_os_system_win_window_handle_of(lh_os_system_win_hwnd_t hwnd)
{
    return lh_cast_static(lh_os_system_window_handle_t, lh_cast_static(lh_ssize_t, (lh_ptr)hwnd));
}

/* Report a pointer message: the client coordinates are the signed low and
   high words of @p lparam (negative left of or above the client area). */
static void
lh_os_system_win_window_emit_pointer(lh_os_system_win_hwnd_t hwnd, lh_uint_t type,
                                     lh_os_system_win_lparam_t lparam, lh_int_t button)
{
    const lh_int_t x = lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, lparam & 0xFFFF));
    const lh_int_t y =
        lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, (lparam >> 16) & 0xFFFF));
    lh_os_system_window_emit(lh_os_system_win_window_handle_of(hwnd), type, x, y, 0, 0, button);
}

static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam)
{
    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_PAINT:
    {
        /* The handler shows the pixels (present works between BeginPaint and
           EndPaint too); EndPaint then marks the area valid. */
        lh_os_system_win_paintstruct_t ps;
        (void)BeginPaint(hwnd, lh_addr_of(ps));
        lh_os_system_window_emit(lh_os_system_win_window_handle_of(hwnd),
                                 lh_os_system_window_event_paint, ps.rcPaint.left, ps.rcPaint.top,
                                 ps.rcPaint.right - ps.rcPaint.left,
                                 ps.rcPaint.bottom - ps.rcPaint.top, 0);
        (void)EndPaint(hwnd, lh_addr_of(ps));
        return 0;
    }
    case LH_OS_SYSTEM_WIN_WM_SIZE:
        lh_os_system_window_emit(lh_os_system_win_window_handle_of(hwnd),
                                 lh_os_system_window_event_resize, 0, 0,
                                 lh_cast_static(lh_int_t, lparam & 0xFFFF),
                                 lh_cast_static(lh_int_t, (lparam >> 16) & 0xFFFF), 0);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_MOUSEMOVE:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_move, lparam,
                                             0);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_down, lparam,
                                             0);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_LBUTTONUP:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_up, lparam, 0);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_down, lparam,
                                             1);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_RBUTTONUP:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_up, lparam, 1);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_down, lparam,
                                             2);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_MBUTTONUP:
        lh_os_system_win_window_emit_pointer(hwnd, lh_os_system_window_event_pointer_up, lparam, 2);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_CLOSE:
        lh_os_system_window_emit(lh_os_system_win_window_handle_of(hwnd),
                                 lh_os_system_window_event_close, 0, 0, 0, 0, 0);
        PostQuitMessage(0);
        return 0;
    case LH_OS_SYSTEM_WIN_WM_DESTROY:
        /* Closing one window should not exit the process; only `WM_CLOSE`
           followed by `DestroyWindow` calls `PostQuitMessage`. */
        return lh_cast_static(lh_os_system_win_lresult_t, 0);
    default:
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}