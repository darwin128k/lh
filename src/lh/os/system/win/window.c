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
 * client frame: the system caption comes off and the caller paints that
 * area. That mode stays on through resize, minimize, and maximize. The
 * rounded shape is a GDI region, so it works where dwmapi is absent.
 * A shadow replaces that region with per-pixel alpha: the region would
 * clip the shadow off. Dark caption and caption colors still ask dwmapi
 * for the system frame, opened by name, and do nothing when that DLL is
 * not there.
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
/* Requested shadow spread, and the padding currently added to the window. */
#define LH_OS_SYSTEM_WIN_WINDOW_SHADOW L"lh.os.shadow"
#define LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD L"lh.os.shadow.pad"
/* Set while the layered bitmap is being shown, so that call cannot re-enter. */
#define LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD L"lh.os.shadow.guard"
/* Peak alpha of the shadow. The falloff is quadratic out to the padding. */
#define LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PEAK 80
/* Set while the corner region is being applied, so the size message that
   follows does not apply it again. */
#define LH_OS_SYSTEM_WIN_WINDOW_CORNER_GUARD L"lh.os.corner.guard"
/* Set while the client caption is on. Absent means the system caption. */
#define LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME L"lh.os.frame"
#define LH_OS_SYSTEM_WIN_WINDOW_BORDER 8

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
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD);
    (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD);
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

/* A property stored as value + 1, so a missing property reads as zero. */
lh_int_t
lh_os_system_win_window_stored(lh_os_system_win_hwnd_t hwnd, lh_wstr_cptr name)
{
    const lh_os_system_win_handle_t stored = GetPropW(hwnd, name);
    if (lh_null_eq(stored))
    {
        return 0;
    }
    return lh_cast_static(lh_int_t, lh_cast_reinterpret(lh_usize_t, stored) - 1U);
}

void
lh_os_system_win_window_store(lh_os_system_win_hwnd_t hwnd, lh_wstr_cptr name, lh_int_t value)
{
    if (value <= 0)
    {
        (void)RemovePropW(hwnd, name);
        return;
    }
    (void)SetPropW(hwnd, name,
                   lh_cast_reinterpret(lh_os_system_win_handle_t,
                                       lh_cast_static(lh_usize_t, value + 1)));
}

/* Spread the caller asked for, including while the window is maximized. */
lh_int_t
lh_os_system_win_window_shadow_spread(lh_os_system_win_hwnd_t hwnd)
{
    if (lh_null_eq(hwnd))
    {
        return 0;
    }
    return lh_os_system_win_window_stored(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW);
}

/* Padding currently inside the window rect. Zero while maximized, because
   that rect is the work area and does not include the shadow. */
lh_int_t
lh_os_system_win_window_shadow_margin(lh_os_system_win_hwnd_t hwnd)
{
    if (lh_null_eq(hwnd) || IsZoomed(hwnd) != 0 || IsIconic(hwnd) != 0)
    {
        return 0;
    }
    return lh_os_system_win_window_stored(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD);
}

lh_int_t
lh_os_system_win_window_isqrt(lh_int_t value)
{
    lh_int_t root;
    lh_int_t next;
    if (value <= 0)
    {
        return 0;
    }
    root = value;
    next = (root + 1) / 2;
    while (next < root)
    {
        root = next;
        next = (root + value / root) / 2;
    }
    return root;
}

/* Signed distance to a rounded rectangle. Negative is inside. Right and
   bottom are exclusive. */
lh_int_t
lh_os_system_win_window_round_distance(lh_int_t px, lh_int_t py, lh_int_t left, lh_int_t top,
                                       lh_int_t right, lh_int_t bottom, lh_int_t radius)
{
    const lh_int_t cx = left + (right - left) / 2;
    const lh_int_t cy = top + (bottom - top) / 2;
    lh_int_t hx = (right - left) / 2 - radius;
    lh_int_t hy = (bottom - top) / 2 - radius;
    lh_int_t dx;
    lh_int_t dy;
    lh_int_t ox;
    lh_int_t oy;
    lh_int_t inside;
    if (hx < 0)
    {
        hx = 0;
    }
    if (hy < 0)
    {
        hy = 0;
    }
    dx = px >= cx ? px - cx : cx - px;
    dy = py >= cy ? py - cy : cy - py;
    dx -= hx;
    dy -= hy;
    ox = dx > 0 ? dx : 0;
    oy = dy > 0 ? dy : 0;
    inside = dx > dy ? dx : dy;
    if (inside > 0)
    {
        inside = 0;
    }
    return lh_os_system_win_window_isqrt(ox * ox + oy * oy) + inside - radius;
}

lh_int_t
lh_os_system_win_window_shadow_alpha(lh_int_t dist, lh_int_t falloff)
{
    lh_int_t remain;
    if (dist >= falloff)
    {
        return 0;
    }
    if (dist <= 0)
    {
        return LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PEAK;
    }
    remain = falloff - dist;
    return LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PEAK * remain * remain / (falloff * falloff);
}

/* Writes a premultiplied top-down bitmap: the card, then a black shadow
   shifted down by a fifth of @p margin. @p src is the card, origin at its
   top left, @p stride pixels per row. */
void
lh_os_system_win_window_compose_shadow(lh_byte_t *dst, lh_int_t full_w, lh_int_t full_h,
                                       const lh_byte_t *src, lh_int_t stride, lh_int_t margin,
                                       lh_int_t radius)
{
    const lh_int_t card_w = full_w - margin * 2;
    const lh_int_t card_h = full_h - margin * 2;
    lh_int_t offset;
    lh_int_t falloff;
    lh_int_t y;
    if (card_w <= 0 || card_h <= 0 || stride <= 0 || lh_ptr_is_null(src))
    {
        return;
    }
    if (radius > card_w / 2)
    {
        radius = card_w / 2;
    }
    if (radius > card_h / 2)
    {
        radius = card_h / 2;
    }
    offset = margin / 5;
    falloff = margin - offset;
    if (falloff < 1)
    {
        falloff = 1;
    }
    for (y = 0; y < full_h; ++y)
    {
        lh_byte_t *const row = dst + (lh_usize_t)y * (lh_usize_t)full_w * 4U;
        lh_int_t x;
        for (x = 0; x < full_w; ++x)
        {
            const lh_int_t sx = x - margin;
            const lh_int_t sy = y - margin;
            lh_int_t card_a = 0;
            lh_int_t shadow_a = 0;
            lh_int_t sr = 0;
            lh_int_t sg = 0;
            lh_int_t sb = 0;
            lh_bool_t covered = lh_bool_false;
            if (sx >= 0 && sy >= 0 && sx < card_w && sy < card_h && sx < stride &&
                (margin <= 0 || radius <= 0 || (sx >= radius && sx < card_w - radius) ||
                 (sy >= radius && sy < card_h - radius)))
            {
                covered = lh_bool_true;
            }
            if (covered)
            {
                const lh_byte_t *pixel =
                    src + ((lh_usize_t)sy * (lh_usize_t)stride + (lh_usize_t)sx) * 4U;
                card_a = pixel[3];
                sr = pixel[0];
                sg = pixel[1];
                sb = pixel[2];
            }
            else if (margin > 0)
            {
                const lh_int_t card_dist = lh_os_system_win_window_round_distance(
                    x, y, margin, margin, margin + card_w, margin + card_h, radius);
                if (card_dist <= 0 && sx >= 0 && sy >= 0 && sx < card_w && sy < card_h &&
                    sx < stride)
                {
                    const lh_byte_t *pixel =
                        src + ((lh_usize_t)sy * (lh_usize_t)stride + (lh_usize_t)sx) * 4U;
                    card_a = pixel[3];
                    if (card_dist == 0 && card_a > 0)
                    {
                        card_a /= 2;
                    }
                    sr = pixel[0];
                    sg = pixel[1];
                    sb = pixel[2];
                }
                if (card_a < 255)
                {
                    const lh_int_t shadow_dist = lh_os_system_win_window_round_distance(
                        x, y, margin, margin + offset, margin + card_w, margin + card_h + offset,
                        radius);
                    shadow_a = lh_os_system_win_window_shadow_alpha(shadow_dist, falloff);
                }
            }
            if (card_a > 0 || shadow_a > 0)
            {
                const lh_int_t inv = 255 - card_a;
                lh_byte_t *const pixel = row + (lh_usize_t)x * 4U;
                pixel[0] = lh_cast_static(lh_byte_t, sb * card_a / 255);
                pixel[1] = lh_cast_static(lh_byte_t, sg * card_a / 255);
                pixel[2] = lh_cast_static(lh_byte_t, sr * card_a / 255);
                pixel[3] = lh_cast_static(lh_byte_t, card_a + shadow_a * inv / 255);
            }
        }
    }
}

lh_bool_t
lh_os_system_win_window_present_shadow(lh_os_system_win_hwnd_t hwnd, const lh_ptr pixels,
                                       lh_int_t stride)
{
    lh_os_system_win_rect_t client;
    lh_os_system_win_bitmapinfoheader_t info;
    lh_os_system_win_size_t size;
    lh_os_system_win_point_t origin;
    lh_os_system_win_blend_t blend;
    lh_os_system_win_hdc_t mem;
    lh_os_system_win_handle_t dib;
    lh_os_system_win_handle_t previous;
    lh_ptr bits;
    lh_int_t full_w;
    lh_int_t full_h;
    lh_int_t margin;
    lh_int_t radius;
    lh_bool_t shown;
    if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD)))
    {
        return lh_bool_true;
    }
    if (GetClientRect(hwnd, lh_addr_of(client)) == 0)
    {
        return lh_bool_false;
    }
    full_w = client.right - client.left;
    full_h = client.bottom - client.top;
    if (full_w <= 0 || full_h <= 0)
    {
        return lh_bool_false;
    }
    margin = lh_os_system_win_window_shadow_margin(hwnd);
    radius = 0;
    if (margin > 0)
    {
        radius = lh_os_system_win_window_stored(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CORNER_PROPERTY);
    }
    mem = CreateCompatibleDC(lh_null);
    if (lh_ptr_is_null(mem))
    {
        return lh_bool_false;
    }
    lh_memory_std_set(lh_addr_of(info), 0, sizeof info);
    info.biSize = lh_cast_static(lh_os_system_win_dword_t, sizeof info);
    info.biWidth = full_w;
    info.biHeight = -full_h;
    info.biPlanes = 1;
    info.biBitCount = 32;
    info.biCompression = LH_OS_SYSTEM_WIN_BI_RGB;
    bits = lh_null;
    dib = CreateDIBSection(mem, lh_addr_of(info), LH_OS_SYSTEM_WIN_DIB_RGB_COLORS, lh_addr_of(bits),
                           lh_null, 0);
    shown = lh_bool_false;
    if (!lh_null_eq(dib) && lh_ptr_is_set(bits))
    {
        lh_memory_std_set(bits, 0, (lh_usize_t)full_w * (lh_usize_t)full_h * 4U);
        lh_os_system_win_window_compose_shadow(lh_ptr_rcast(lh_byte_t, bits), full_w, full_h,
                                               lh_ptr_rcast(const lh_byte_t, pixels), stride,
                                               margin, radius);
        previous = SelectObject(mem, dib);
        size.cx = full_w;
        size.cy = full_h;
        origin.x = 0;
        origin.y = 0;
        blend.BlendOp = lh_cast_static(lh_byte_t, LH_OS_SYSTEM_WIN_AC_SRC_OVER);
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = lh_cast_static(lh_byte_t, 255);
        blend.AlphaFormat = lh_cast_static(lh_byte_t, LH_OS_SYSTEM_WIN_AC_SRC_ALPHA);
        (void)SetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD,
                       lh_cast_reinterpret(lh_os_system_win_handle_t,
                                           lh_cast_static(lh_usize_t, 1)));
        shown = UpdateLayeredWindow(hwnd, lh_null, lh_null, lh_addr_of(size), mem,
                                    lh_addr_of(origin), 0, lh_addr_of(blend),
                                    LH_OS_SYSTEM_WIN_ULW_ALPHA) != 0;
        (void)RemovePropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD);
        if (!lh_null_eq(previous))
        {
            (void)SelectObject(mem, previous);
        }
    }
    if (!lh_null_eq(dib))
    {
        (void)DeleteObject(dib);
    }
    (void)DeleteDC(mem);
    return shown;
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
    {
        const lh_int_t span = client.right - client.left;
        const lh_int_t margin =
            lh_os_system_win_window_shadow_margin(lh_os_system_win_window_native(self));
        if (margin > 0 && span > margin * 2)
        {
            return span - margin * 2;
        }
        return span;
    }
}

lh_int_t
lh_os_system_window_get_client_height(lh_os_system_window_handle_t self)
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
    {
        const lh_int_t span = client.bottom - client.top;
        const lh_int_t margin =
            lh_os_system_win_window_shadow_margin(lh_os_system_win_window_native(self));
        if (margin > 0 && span > margin * 2)
        {
            return span - margin * 2;
        }
        return span;
    }
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

    {
        const lh_os_system_win_hwnd_t hwnd = lh_os_system_win_window_native(self);
        if (lh_os_system_win_window_shadow_spread(hwnd) > 0 &&
            !lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME)))
        {
            return lh_os_system_win_window_present_shadow(hwnd, pixels, stride);
        }
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
    /* A region clips the shadow off, so the alpha bitmap is the outline. */
    if (radius <= 0 || fitted || lh_os_system_win_window_shadow_spread(hwnd) > 0)
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

/* Grows the window by the shadow padding and marks it layered, or puts both
   back when the shadow is off. The padding is stored before the resize so
   the size message reports the card, not the padding. */
void
lh_os_system_win_window_place_shadow(lh_os_system_win_hwnd_t hwnd)
{
    const lh_bool_t client = !lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_CLIENT_FRAME));
    const lh_bool_t fitted = IsZoomed(hwnd) != 0 || IsIconic(hwnd) != 0;
    const lh_int_t spread = lh_os_system_win_window_shadow_spread(hwnd);
    const lh_int_t have = lh_os_system_win_window_stored(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD);
    const lh_int_t want = client && !fitted ? spread : 0;
    const lh_long_t layered = lh_cast_static(lh_long_t, LH_OS_SYSTEM_WIN_WS_EX_LAYERED);
    lh_long_t extra = GetWindowLongW(hwnd, LH_OS_SYSTEM_WIN_GWL_EXSTYLE);
    const lh_long_t previous = extra;
    if (spread <= 0 && have <= 0 && (extra & layered) == 0)
    {
        if (client)
        {
            lh_os_system_win_window_apply_region(hwnd);
        }
        else
        {
            (void)SetWindowRgn(hwnd, lh_null, lh_bool_true);
        }
        return;
    }
    if (client && spread > 0)
    {
        extra |= layered;
    }
    else
    {
        extra &= ~layered;
    }
    (void)SetWindowLongW(hwnd, LH_OS_SYSTEM_WIN_GWL_EXSTYLE, extra);
    if (want != have && !fitted)
    {
        lh_os_system_win_rect_t bounds;
        if (GetWindowRect(hwnd, lh_addr_of(bounds)) != 0)
        {
            const lh_int_t delta = want - have;
            lh_os_system_win_window_store(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD, want);
            if (SetWindowPos(hwnd, lh_null, bounds.left - delta, bounds.top - delta,
                             (bounds.right - bounds.left) + delta * 2,
                             (bounds.bottom - bounds.top) + delta * 2,
                             LH_OS_SYSTEM_WIN_SWP_NOZORDER | LH_OS_SYSTEM_WIN_SWP_NOACTIVATE |
                                 LH_OS_SYSTEM_WIN_SWP_FRAMECHANGED) == 0)
            {
                lh_os_system_win_window_store(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_PAD, have);
            }
        }
    }
    else if (extra != previous)
    {
        (void)SetWindowPos(hwnd, lh_null, 0, 0, 0, 0,
                           LH_OS_SYSTEM_WIN_SWP_NOMOVE | LH_OS_SYSTEM_WIN_SWP_NOSIZE |
                               LH_OS_SYSTEM_WIN_SWP_NOZORDER | LH_OS_SYSTEM_WIN_SWP_NOACTIVATE |
                               LH_OS_SYSTEM_WIN_SWP_FRAMECHANGED);
    }
    if (client)
    {
        lh_os_system_win_window_apply_region(hwnd);
    }
    else
    {
        (void)SetWindowRgn(hwnd, lh_null, lh_bool_true);
    }
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

    lh_int_t width = client.right - client.left;
    lh_int_t height = client.bottom - client.top;
    const lh_int_t margin = lh_os_system_win_window_shadow_margin(hwnd);
    const lh_int_t border = LH_OS_SYSTEM_WIN_WINDOW_BORDER;
    lh_bool_t left;
    lh_bool_t right;
    lh_bool_t top;
    lh_bool_t bottom;
    if (margin > 0)
    {
        if (point.x < margin || point.y < margin || point.x >= width - margin ||
            point.y >= height - margin)
        {
            return LH_OS_SYSTEM_WIN_HTTRANSPARENT;
        }
        point.x -= margin;
        point.y -= margin;
        width -= margin * 2;
        height -= margin * 2;
    }
    left = point.x < border;
    right = point.x >= width - border;
    top = point.y < border;
    bottom = point.y >= height - border;
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
    lh_os_system_win_window_place_shadow(hwnd);
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
lh_os_system_window_set_shadow(lh_os_system_window_handle_t self, lh_int_t spread)
{
    lh_os_system_win_hwnd_t hwnd;
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    if (spread < 0)
    {
        spread = 0;
    }
    hwnd = lh_os_system_win_window_native(self);
    if (lh_os_system_win_window_shadow_spread(hwnd) == spread)
    {
        return;
    }
    lh_os_system_win_window_store(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW, spread);
    lh_os_system_win_window_place_shadow(hwnd);
}

lh_int_t
lh_os_system_window_get_shadow(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return 0;
    }
    return lh_os_system_win_window_shadow_spread(lh_os_system_win_window_native(self));
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
}

void
lh_os_system_window_close_frame(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    (void)PostMessageW(lh_os_system_win_window_native(self), LH_OS_SYSTEM_WIN_WM_CLOSE, 0, 0);
}

void
lh_os_system_window_minimize(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    (void)ShowWindow(lh_os_system_win_window_native(self), LH_OS_SYSTEM_WIN_SW_MINIMIZE);
}

void
lh_os_system_window_zoom(lh_os_system_window_handle_t self)
{
    lh_os_system_win_hwnd_t hwnd;

    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    hwnd = lh_os_system_win_window_native(self);
    (void)ShowWindow(hwnd, IsZoomed(hwnd) != 0 ? LH_OS_SYSTEM_WIN_SW_RESTORE
                                               : LH_OS_SYSTEM_WIN_SW_MAXIMIZE);
}

void
lh_os_system_window_begin_move(lh_os_system_window_handle_t self)
{
    lh_os_system_win_hwnd_t hwnd;

    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    hwnd = lh_os_system_win_window_native(self);
    (void)ReleaseCapture();
    (void)SendMessageW(hwnd, LH_OS_SYSTEM_WIN_WM_NCLBUTTONDOWN, LH_OS_SYSTEM_WIN_HTCAPTION, 0);
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
    lh_int_t x = lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, lparam & 0xFFFF));
    lh_int_t y = lh_cast_static(lh_sshort_t, lh_cast_static(lh_ushort_t, (lparam >> 16) & 0xFFFF));
    lh_os_system_win_rect_t client;
    lh_int_t margin;
    lh_int_t width;
    lh_int_t height;
    if (GetClientRect(hwnd, lh_addr_of(client)) == 0)
    {
        return;
    }
    margin = lh_os_system_win_window_shadow_margin(hwnd);
    width = client.right - client.left;
    height = client.bottom - client.top;
    if (x < margin || y < margin || x >= width - margin || y >= height - margin)
    {
        return;
    }
    x -= margin;
    y -= margin;
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
        if (!lh_null_eq(GetPropW(hwnd, LH_OS_SYSTEM_WIN_WINDOW_SHADOW_GUARD)))
        {
            return 0;
        }
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