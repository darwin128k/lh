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
 *
 * The window opens with the system caption. A caller can switch it to a
 * client caption: the system caption style comes off, and lh paints the
 * bar. That mode stays on through resize, minimize, and maximize. The
 * rounded shape is a GDI region, so it works where dwmapi is absent.
 * Dark caption and caption colors still ask dwmapi for the system frame,
 * opened by name, and do nothing when that DLL is not there.
 */

#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/shared.h>
#include <lh/os/system/window.h>
#include <lh/os/system/window/emit.h>
#include <lh/os/system/win/dwmapi.h>
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

/* The window itself stores the corner radius (plus one, so "unset" and
   "square" differ) and whether a dark caption was asked for. */
#define LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY L"lh.os.corner"
#define LH_OS_SYSTEM_WIN_WINDOW_DARK_PROPERTY L"lh.os.dark"
/* Set while the corner region is being applied, so the size message that
   follows does not apply it again. */
#define LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD L"lh.os.corner.guard"
/* Set while the client caption is on. Absent means the system caption. */
#define LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME L"lh.os.frame"
#define LH_OS_SYSTEM_WIN_WINDOW_CAPTION_COLOR L"lh.os.caption"
#define LH_OS_SYSTEM_WIN_WINDOW_TEXT_COLOR L"lh.os.text"
#define LH_OS_SYSTEM_WIN_WINDOW_CAPTION_HEIGHT 32
#define LH_OS_SYSTEM_WIN_WINDOW_BORDER 8
#define LH_OS_SYSTEM_WIN_WINDOW_BUTTON_WIDTH 46

void
lh_os_system_win_window_paint_caption(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_hdc_t dc);

lh_os_system_win_lresult_t
lh_os_system_win_window_hit_test(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_lparam_t lparam);

void
lh_os_system_win_window_use_style(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t style,
                                  lh_bool_t app_window);

void
lh_os_system_win_window_apply_region(lh_os_system_win_hwnd_t hwnd);

void
lh_os_system_win_window_limit_maximized(lh_os_system_win_hwnd_t hwnd,
                                        lh_os_system_win_lparam_t lparam);

lh_int_t
lh_os_system_win_window_content_top(lh_os_system_win_hwnd_t hwnd);

lh_os_system_win_dword_t
lh_os_system_win_window_color_prop(lh_os_system_win_hwnd_t hwnd, lh_wstr_cptr name,
                                   lh_os_system_win_dword_t fallback);

void
lh_os_system_win_window_paint_mark(lh_os_system_win_hdc_t dc, lh_int_t kind, lh_int_t left,
                                   lh_int_t top);

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

    /* `width` and `height` are the client. The system frame sits outside them. */
    lh_os_system_win_rect_t bounds;
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = width;
    bounds.bottom = height;
    (void)AdjustWindowRect(lh_addr_of(bounds), LH_OS_SYSTEM_WIN_WS_FRAME_SYSTEM, 0);

    /* `CreateWindowExW` returns `NULL` (== `0` as int) on failure. */
    lh_os_system_win_hwnd_t hwnd = CreateWindowExW(
        lh_cast_static(lh_os_system_win_dword_t, 0), lh_os_system_win_window_class_name,
        (lh_wstr_cptr)title, LH_OS_SYSTEM_WIN_WS_FRAME_SYSTEM, LH_OS_SYSTEM_WIN_CW_USEDEFAULT,
        LH_OS_SYSTEM_WIN_CW_USEDEFAULT, bounds.right - bounds.left, bounds.bottom - bounds.top,
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
    lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_DARK_PROPERTY);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CAPTION_COLOR);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_TEXT_COLOR);
    (void)DestroyWindow(hwnd);
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

lh_int_t
lh_os_system_win_window_content_top(lh_os_system_win_hwnd_t hwnd)
{
    if (lh_null_eq(hwnd) || lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
    {
        return 0;
    }
    return LH_OS_SYSTEM_WIN_WINDOW_CAPTION_HEIGHT;
}

lh_int_t
lh_os_system_window_get_width(lh_os_system_window_handle_t self)
{
    lh_os_system_win_rect_t bounds;

    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    if (GetWindowRect(lh_os_system_win_window_native(self), lh_addr_of(bounds)) == 0)
    {
        return 0;
    }
    return bounds.right - bounds.left;
}

lh_int_t
lh_os_system_window_get_height(lh_os_system_window_handle_t self)
{
    lh_os_system_win_rect_t bounds;

    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    if (GetWindowRect(lh_os_system_win_window_native(self), lh_addr_of(bounds)) == 0)
    {
        return 0;
    }
    return bounds.bottom - bounds.top;
}

lh_int_t
lh_os_system_window_get_client_width(lh_os_system_window_handle_t self)
{
    lh_os_system_win_rect_t client;

    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    if (GetClientRect(lh_os_system_win_window_native(self), lh_addr_of(client)) == 0)
    {
        return 0;
    }
    return client.right - client.left;
}

lh_int_t
lh_os_system_window_get_client_height(lh_os_system_window_handle_t self)
{
    lh_os_system_win_rect_t client;
    lh_int_t height;

    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    if (GetClientRect(hwnd, lh_addr_of(client)) == 0)
    {
        return 0;
    }
    height = client.bottom - client.top - lh_os_system_win_window_content_top(hwnd);
    if (height < 0)
    {
        return 0;
    }
    return height;
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
        shown = SetDIBitsToDevice(dc, x, y + lh_os_system_win_window_content_top(hwnd),
                                  lh_cast_static(lh_os_system_win_dword_t, width),
                                  lh_cast_static(lh_os_system_win_dword_t, height), 0, 0, 0,
                                  lh_cast_static(lh_os_system_win_uint_t, height), bgrx,
                                  lh_addr_of(info), LH_OS_SYSTEM_WIN_DIB_RGB_COLORS) != 0;
        lh_os_system_win_window_paint_caption(hwnd, dc);
        (void)ReleaseDC(hwnd, dc);
    }
    lh_runtime_allocator_free(bgrx);
    return shown;
}

lh_os_system_win_dword_t
lh_os_system_win_colorref(lh_uint_t rgb)
{
    return lh_cast_static(lh_os_system_win_dword_t, ((rgb & 0xFFU) << 16) | (rgb & 0xFF00U) |
                                                        ((rgb >> 16) & 0xFFU));
}

lh_bool_t
lh_os_system_win_dwm_set_attribute(lh_os_system_win_hwnd_t window,
                                   lh_os_system_win_dword_t attribute, const lh_ptr value,
                                   lh_os_system_win_dword_t size)
{
    const lh_os_system_shared_handle_t image = lh_os_system_shared_open("dwmapi.dll");
    lh_bool_t accepted = lh_bool_false;
    if (lh_null_eq(image))
    {
        return lh_bool_false;
    }

    const lh_ptr sym = lh_os_system_shared_get_sym(image, "DwmSetWindowAttribute");
    if (lh_ptr_is_set(sym))
    {
        const lh_os_system_win_dwm_set_window_attribute_fn fn =
            lh_cast_reinterpret(lh_os_system_win_dwm_set_window_attribute_fn, sym);
        const lh_long_t result = fn(window, attribute, value, size);
        accepted = result >= 0 ? lh_bool_true : lh_bool_false;
    }
    (void)lh_os_system_shared_close(image);
    return accepted;
}

void
lh_os_system_win_window_use_style(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t style,
                                  lh_bool_t app_window)
{
    (void)SetWindowLongW(hwnd, LH_OS_SYSTEM_WIN_GWL_STYLE, lh_cast_static(lh_long_t, style));
    lh_long_t extra = GetWindowLongW(hwnd, LH_OS_SYSTEM_WIN_GWL_EXSTYLE);
    const lh_long_t app = lh_cast_static(lh_long_t, LH_OS_SYSTEM_WIN_WS_EX_APPWINDOW);
    if (app_window != lh_bool_false)
    {
        extra |= app;
    }
    else
    {
        extra &= ~app;
    }
    (void)SetWindowLongW(hwnd, LH_OS_SYSTEM_WIN_GWL_EXSTYLE, extra);
    (void)SetWindowPos(hwnd, lh_null, 0, 0, 0, 0,
                       LH_OS_SYSTEM_WIN_SWP_NOMOVE | LH_OS_SYSTEM_WIN_SWP_NOSIZE |
                           LH_OS_SYSTEM_WIN_SWP_NOZORDER | LH_OS_SYSTEM_WIN_SWP_NOACTIVATE |
                           LH_OS_SYSTEM_WIN_SWP_FRAMECHANGED);
}

void
lh_os_system_win_window_apply_region(lh_os_system_win_hwnd_t hwnd)
{
    if (lh_null_eq(hwnd) || !lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD)) ||
        lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
    {
        return;
    }

    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD,
                   lh_cast_reinterpret(lh_os_system_win_handle_t, lh_cast_static(lh_usize_t, 1)));

    const lh_os_system_win_handle_t stored =
        GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY);
    lh_int_t radius = 0;
    if (!lh_null_eq(stored))
    {
        radius = lh_cast_static(lh_int_t, lh_cast_reinterpret(lh_usize_t, stored) - 1U);
    }
    const lh_bool_t fitted = IsZoomed(hwnd) != 0 || IsIconic(hwnd) != 0;
    if (radius <= 0 || fitted)
    {
        (void)SetWindowRgn(hwnd, lh_null, lh_bool_true);
    }
    else
    {
        lh_os_system_win_rect_t bounds;
        if (GetWindowRect(hwnd, lh_addr_of(bounds)) != 0)
        {
            const lh_int_t width = bounds.right - bounds.left;
            const lh_int_t height = bounds.bottom - bounds.top;
            if (width > 0 && height > 0)
            {
                const lh_os_system_win_handle_t region =
                    CreateRoundRectRgn(0, 0, width, height, radius * 2, radius * 2);
                if (!lh_null_eq(region) && SetWindowRgn(hwnd, region, lh_bool_true) == 0)
                {
                    (void)DeleteObject(region);
                }
            }
        }
    }
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD);
}

void
lh_os_system_win_window_limit_maximized(lh_os_system_win_hwnd_t hwnd,
                                        lh_os_system_win_lparam_t lparam)
{
    lh_os_system_win_monitorinfo_t info;
    lh_os_system_win_minmaxinfo_t *limits;

    info.size = lh_cast_static(lh_os_system_win_dword_t, sizeof info);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd, LH_OS_SYSTEM_WIN_MONITOR_DEFAULTTONEAREST),
                        lh_addr_of(info)) == 0)
    {
        return;
    }
    limits = lh_cast_reinterpret(lh_os_system_win_minmaxinfo_t *, lparam);
    /* The client frame has no system border, so maximize fills the work
       area and leaves the taskbar visible. */
    limits->max_position.x = info.work.left;
    limits->max_position.y = info.work.top;
    limits->max_size.x = info.work.right - info.work.left;
    limits->max_size.y = info.work.bottom - info.work.top;
}

lh_os_system_win_dword_t
lh_os_system_win_window_color_prop(lh_os_system_win_hwnd_t hwnd, lh_wstr_cptr name,
                                   lh_os_system_win_dword_t fallback)
{
    const lh_os_system_win_handle_t stored = GetPropW(hwnd, name);
    if (lh_null_eq(stored))
    {
        return fallback;
    }
    return lh_cast_static(lh_os_system_win_dword_t, lh_cast_reinterpret(lh_usize_t, stored) - 1U);
}

void
lh_os_system_win_window_paint_mark(lh_os_system_win_hdc_t dc, lh_int_t kind, lh_int_t left,
                                   lh_int_t top)
{
    const lh_int_t x0 = left + 16;
    const lh_int_t x1 = left + 30;
    const lh_int_t y0 = top + 10;
    const lh_int_t y1 = top + 22;
    const lh_int_t mid = top + (LH_OS_SYSTEM_WIN_WINDOW_CAPTION_HEIGHT / 2);
    if (kind == 0)
    {
        (void)MoveToEx(dc, x0, mid, lh_null);
        (void)LineTo(dc, x1, mid);
        return;
    }
    if (kind == 1)
    {
        (void)MoveToEx(dc, x0, y0, lh_null);
        (void)LineTo(dc, x1, y0);
        (void)LineTo(dc, x1, y1);
        (void)LineTo(dc, x0, y1);
        (void)LineTo(dc, x0, y0);
        return;
    }
    (void)MoveToEx(dc, x0, y0, lh_null);
    (void)LineTo(dc, x1, y1);
    (void)MoveToEx(dc, x1, y0, lh_null);
    (void)LineTo(dc, x0, y1);
}

void
lh_os_system_win_window_paint_caption(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_hdc_t dc)
{
    if (lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)) || lh_null_eq(dc))
    {
        return;
    }

    lh_os_system_win_rect_t client;
    if (GetClientRect(hwnd, lh_addr_of(client)) == 0)
    {
        return;
    }
    const lh_os_system_win_dword_t caption = lh_os_system_win_window_color_prop(
        hwnd, LH_OS_SYSTEM_WIN_WINDOW_CAPTION_COLOR, 0x001F1B1CU);
    const lh_os_system_win_dword_t text =
        lh_os_system_win_window_color_prop(hwnd, LH_OS_SYSTEM_WIN_WINDOW_TEXT_COLOR, 0x00E5E1E6U);
    lh_os_system_win_rect_t bar = client;
    bar.bottom = LH_OS_SYSTEM_WIN_WINDOW_CAPTION_HEIGHT;
    const lh_os_system_win_handle_t brush = CreateSolidBrush(caption);
    if (!lh_null_eq(brush))
    {
        (void)FillRect(dc, lh_addr_of(bar), brush);
        (void)DeleteObject(brush);
    }

    const lh_os_system_win_handle_t font = GetStockObject(LH_OS_SYSTEM_WIN_DEFAULT_GUI_FONT);
    const lh_os_system_win_handle_t old_font = SelectObject(dc, font);
    (void)SetBkMode(dc, LH_OS_SYSTEM_WIN_TRANSPARENT);
    (void)SetTextColor(dc, text);
    lh_wchar_t title[128];
    const lh_int_t count = GetWindowTextW(hwnd, title, 128);
    if (count > 0)
    {
        (void)TextOutW(dc, 12, 8, title, count);
    }
    (void)SelectObject(dc, old_font);

    const lh_os_system_win_handle_t pen =
        CreatePen(LH_OS_SYSTEM_WIN_PS_SOLID, 1, text);
    if (lh_null_eq(pen))
    {
        return;
    }
    const lh_os_system_win_handle_t old_pen = SelectObject(dc, pen);
    const lh_int_t width = client.right - client.left;
    lh_int_t kind;
    for (kind = 0; kind < 3; ++kind)
    {
        const lh_int_t left = width - (3 - kind) * LH_OS_SYSTEM_WIN_WINDOW_BUTTON_WIDTH;
        lh_os_system_win_window_paint_mark(dc, kind, left, 0);
    }
    (void)SelectObject(dc, old_pen);
    (void)DeleteObject(pen);
}

lh_os_system_win_lresult_t
lh_os_system_win_window_hit_test(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_lparam_t lparam)
{
    lh_os_system_win_point_t point;
    lh_os_system_win_rect_t client;
    point.x = lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, lparam & 0xFFFF));
    point.y = lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, (lparam >> 16) & 0xFFFF));
    if (ScreenToClient(hwnd, lh_addr_of(point)) == 0 || GetClientRect(hwnd, lh_addr_of(client)) == 0)
    {
        return LH_OS_SYSTEM_WIN_HTCLIENT;
    }

    const lh_int_t width = client.right - client.left;
    const lh_int_t height = client.bottom - client.top;
    const lh_int_t border = LH_OS_SYSTEM_WIN_WINDOW_BORDER;
    const lh_bool_t left = point.x < border;
    const lh_bool_t right = point.x >= width - border;
    const lh_bool_t top = point.y < border;
    const lh_bool_t bottom = point.y >= height - border;
    if (top && left)
    {
        return LH_OS_SYSTEM_WIN_HTTOPLEFT;
    }
    if (top && right)
    {
        return LH_OS_SYSTEM_WIN_HTTOPRIGHT;
    }
    if (bottom && left)
    {
        return LH_OS_SYSTEM_WIN_HTBOTTOMLEFT;
    }
    if (bottom && right)
    {
        return LH_OS_SYSTEM_WIN_HTBOTTOMRIGHT;
    }
    if (top)
    {
        return LH_OS_SYSTEM_WIN_HTTOP;
    }
    if (bottom)
    {
        return LH_OS_SYSTEM_WIN_HTBOTTOM;
    }
    /* Caption buttons sit on the right edge. They win over the resize border. */
    if (point.y < LH_OS_SYSTEM_WIN_WINDOW_CAPTION_HEIGHT)
    {
        const lh_int_t button = LH_OS_SYSTEM_WIN_WINDOW_BUTTON_WIDTH;
        if (point.x >= width - button)
        {
            return LH_OS_SYSTEM_WIN_HTCLOSE;
        }
        if (point.x >= width - button * 2)
        {
            return LH_OS_SYSTEM_WIN_HTMAXBUTTON;
        }
        if (point.x >= width - button * 3)
        {
            return LH_OS_SYSTEM_WIN_HTMINBUTTON;
        }
        return LH_OS_SYSTEM_WIN_HTCAPTION;
    }
    if (left)
    {
        return LH_OS_SYSTEM_WIN_HTLEFT;
    }
    if (right)
    {
        return LH_OS_SYSTEM_WIN_HTRIGHT;
    }
    return LH_OS_SYSTEM_WIN_HTCLIENT;
}

void
lh_os_system_window_set_frame(lh_os_system_window_handle_t self, lh_int_t frame)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }

    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    const lh_bool_t client = frame == LH_OS_SYSTEM_WINDOW_FRAME_CLIENT;
    const lh_bool_t current = !lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME));
    if (current == client)
    {
        if (client)
        {
            lh_os_system_win_window_apply_region(hwnd);
        }
        return;
    }
    if (client)
    {
        (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME,
                       lh_cast_reinterpret(lh_os_system_win_handle_t, lh_cast_static(lh_usize_t, 1)));
    }
    else
    {
        (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME);
    }

    /* The style change posts a size message. The guard keeps that message
       from applying the region before the style is in place. */
    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD,
                   lh_cast_reinterpret(lh_os_system_win_handle_t, lh_cast_static(lh_usize_t, 1)));
    if (client)
    {
        lh_os_system_win_window_use_style(hwnd, LH_OS_SYSTEM_WIN_WS_FRAME_CLIENT, lh_bool_true);
    }
    else
    {
        lh_os_system_win_window_use_style(hwnd, LH_OS_SYSTEM_WIN_WS_FRAME_SYSTEM, lh_bool_false);
    }
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD);
    if (client)
    {
        lh_os_system_win_window_apply_region(hwnd);
    }
    else
    {
        (void)SetWindowRgn(hwnd, lh_null, lh_bool_true);
    }
}

lh_int_t
lh_os_system_window_get_frame(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return LH_OS_SYSTEM_WINDOW_FRAME_SYSTEM;
    }
    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
    {
        return LH_OS_SYSTEM_WINDOW_FRAME_CLIENT;
    }
    return LH_OS_SYSTEM_WINDOW_FRAME_SYSTEM;
}

void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t self, lh_int_t radius)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    if (radius < 0)
    {
        radius = 0;
    }

    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY,
                   lh_cast_reinterpret(lh_os_system_win_handle_t,
                                       lh_cast_static(lh_usize_t, radius + 1)));
    lh_os_system_win_window_apply_region(hwnd);
}

lh_int_t
lh_os_system_window_get_corner_radius(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    const lh_os_system_win_handle_t stored =
        GetPropW(lh_os_system_win_window_native(self), LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY);
    if (lh_null_eq(stored))
    {
        return 0;
    }
    return lh_cast_static(lh_int_t, lh_cast_reinterpret(lh_usize_t, stored) - 1U);
}

void
lh_os_system_window_set_dark(lh_os_system_window_handle_t self, lh_bool_t dark)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }

    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    lh_os_system_win_bool_t enabled = dark != lh_bool_false ? 1 : 0;
    const lh_os_system_win_dword_t size = lh_cast_static(lh_os_system_win_dword_t, sizeof enabled);
    (void)lh_os_system_win_dwm_set_attribute(
        hwnd, LH_OS_SYSTEM_WIN_DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, lh_addr_of(enabled), size);
    (void)lh_os_system_win_dwm_set_attribute(hwnd, LH_OS_SYSTEM_WIN_DWMWA_USE_IMMERSIVE_DARK_MODE,
                                             lh_addr_of(enabled), size);
    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_DARK_PROPERTY,
                   lh_cast_reinterpret(lh_os_system_win_handle_t,
                                       lh_cast_static(lh_usize_t, dark != lh_bool_false ? 2 : 1)));
    /* The caption keeps its old color until the frame is built again. */
    if (lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)) &&
        lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD)))
    {
        (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD,
                       lh_cast_reinterpret(lh_os_system_win_handle_t,
                                           lh_cast_static(lh_usize_t, 1)));
        (void)SetWindowPos(hwnd, lh_null, 0, 0, 0, 0,
                           LH_OS_SYSTEM_WIN_SWP_NOMOVE | LH_OS_SYSTEM_WIN_SWP_NOSIZE |
                               LH_OS_SYSTEM_WIN_SWP_NOZORDER | LH_OS_SYSTEM_WIN_SWP_NOACTIVATE |
                               LH_OS_SYSTEM_WIN_SWP_FRAMECHANGED);
        (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD);
    }
}

lh_bool_t
lh_os_system_window_get_dark(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return lh_bool_false;
    }
    const lh_os_system_win_handle_t stored =
        GetPropW(lh_os_system_win_window_native(self), LH_OS_SYSTEM_WIN_WINDOW_DARK_PROPERTY);
    return lh_cast_reinterpret(lh_usize_t, stored) == 2U ? lh_bool_true : lh_bool_false;
}

void
lh_os_system_window_set_chrome(lh_os_system_window_handle_t self, lh_uint_t caption, lh_uint_t text,
                               lh_uint_t border)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }

    const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
    const lh_os_system_win_dword_t caption_bgr = lh_os_system_win_colorref(caption);
    const lh_os_system_win_dword_t text_bgr = lh_os_system_win_colorref(text);
    const lh_os_system_win_dword_t border_bgr = lh_os_system_win_colorref(border);
    const lh_os_system_win_dword_t size =
        lh_cast_static(lh_os_system_win_dword_t, sizeof caption_bgr);
    (void)lh_os_system_win_dwm_set_attribute(hwnd, LH_OS_SYSTEM_WIN_DWMWA_BORDER_COLOR,
                                             lh_addr_of(border_bgr), size);
    (void)lh_os_system_win_dwm_set_attribute(hwnd, LH_OS_SYSTEM_WIN_DWMWA_CAPTION_COLOR,
                                             lh_addr_of(caption_bgr), size);
    (void)lh_os_system_win_dwm_set_attribute(hwnd, LH_OS_SYSTEM_WIN_DWMWA_TEXT_COLOR,
                                             lh_addr_of(text_bgr), size);
    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CAPTION_COLOR,
                   lh_cast_reinterpret(lh_os_system_win_handle_t,
                                       lh_cast_static(lh_usize_t, caption_bgr) + 1U));
    (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_TEXT_COLOR,
                   lh_cast_reinterpret(lh_os_system_win_handle_t,
                                       lh_cast_static(lh_usize_t, text_bgr) + 1U));
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
    const lh_int_t top = lh_os_system_win_window_content_top(hwnd);
    const lh_int_t y =
        lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, (lparam >> 16) & 0xFFFF)) - top;
    if (y < 0)
    {
        return;
    }
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
    case LH_OS_SYSTEM_WIN_WM_ERASEBKGND:
        /* The caller paints the client. Skipping the system fill keeps the
           first frame from flashing the default brush. */
        return 1;
    case LH_OS_SYSTEM_WIN_WM_GETMINMAXINFO:
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            lh_os_system_win_window_limit_maximized(hwnd, lparam);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCCALCSIZE:
        if (wparam != 0 && !lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCHITTEST:
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return lh_os_system_win_window_hit_test(hwnd, lparam);
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCPAINT:
        /* The system caption is not ours to repaint while the client frame
           is up; the default paint puts the light caption back. */
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCACTIVATE:
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return DefWindowProcW(hwnd, msg, wparam, -1);
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCLBUTTONDOWN:
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)) &&
            (wparam == LH_OS_SYSTEM_WIN_HTCLOSE || wparam == LH_OS_SYSTEM_WIN_HTMINBUTTON ||
             wparam == LH_OS_SYSTEM_WIN_HTMAXBUTTON))
        {
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_NCLBUTTONUP:
        if (lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
        if (wparam == LH_OS_SYSTEM_WIN_HTCLOSE)
        {
            (void)PostMessageW(hwnd, LH_OS_SYSTEM_WIN_WM_CLOSE, 0, 0);
            return 0;
        }
        if (wparam == LH_OS_SYSTEM_WIN_HTMINBUTTON)
        {
            (void)ShowWindow(hwnd, LH_OS_SYSTEM_WIN_SW_MINIMIZE);
            return 0;
        }
        if (wparam == LH_OS_SYSTEM_WIN_HTMAXBUTTON)
        {
            (void)ShowWindow(hwnd, IsZoomed(hwnd) != 0 ? LH_OS_SYSTEM_WIN_SW_RESTORE
                                                       : LH_OS_SYSTEM_WIN_SW_MAXIMIZE);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    case LH_OS_SYSTEM_WIN_WM_SIZE:
    {
        lh_os_system_window_emit(
            lh_os_system_win_window_handle_of(hwnd), lh_os_system_window_event_resize, 0, 0,
            lh_os_system_window_get_client_width(lh_os_system_win_window_handle_of(hwnd)),
            lh_os_system_window_get_client_height(lh_os_system_win_window_handle_of(hwnd)), 0);
        if (lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD)))
        {
            lh_os_system_win_window_apply_region(hwnd);
        }
        return 0;
    }
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