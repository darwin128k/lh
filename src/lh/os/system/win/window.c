/**
 * @file window.c
 * @brief Win32 backend for `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/memory.h>
#include <lh/os/input/contacts.h>
#include <lh/os/input/contacts/initializer.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/os/system/win/input.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/os/system/win/key.h>
#include <lh/os/system/win/user32.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/os/window.h>
#include <lh/util/return.h>

/** The window class every window of ours is registered under. */
#define LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME "lh_os_window"

/** The one timer id this backend uses, and the only one a window of ours may own. */
#define LH_OS_SYSTEM_WIN_TIMER_ID 1U

/** The shortest period a Windows timer can be relied on to keep: USER_TIMER_MINIMUM. */
#define LH_OS_SYSTEM_WIN_TIMER_FLOOR_MS 10U

/** The window procedure of every window of ours: Win32 messages to the window's callbacks. */
static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam);

/** What ::lh_os_window_zone_t means to Windows. The one table, so a zone added to the
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

/* -- Input ----------------------------------------------------------------- */

/** The client point packed in a mouse message's @p lparam, as an event coordinate. */
static lh_os_input_coord_t
lh_os_system_win_window_coord_x(lh_os_system_win_lparam_t lparam)
{
    return lh_os_input_coord_from_int(
        lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_LOWORD(lparam))));
}

/** The y of ::lh_os_system_win_window_coord_x. */
static lh_os_input_coord_t
lh_os_system_win_window_coord_y(lh_os_system_win_lparam_t lparam)
{
    return lh_os_input_coord_from_int(
        lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_HIWORD(lparam))));
}

/** One mouse event of @p kind about @p button to @p window, at the client point in
   @p lparam, with the buttons in @p wparam held after it. Pointer `0` whatever made it:
   a mouse message the system made out of a touch contact or a pen (Windows 7, or no
   pointer API) says so in its device kind. */
static lh_void
lh_os_system_win_window_send_pointer(lh_os_window_t *window, lh_os_input_kind_t kind,
                                     lh_os_input_button_t button, lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(
        lh_addr_of(event), kind, lh_os_system_win_input_device_kind_of_mouse(GetMessageExtraInfo()),
        0U, button, lh_os_system_win_window_coord_x(lparam),
        lh_os_system_win_window_coord_y(lparam), lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(event), lh_os_system_win_input_buttons_of(wparam));
    lh_os_input_event_set_mods(lh_addr_of(event), lh_os_system_win_input_get_mods());
    lh_os_window_on_native_input(window, lh_addr_of(event));
}

/** A button message: the capture follows the buttons, so a drag that leaves the
   window still ends here -- taken on the first press, let go once none is held. */
static lh_void
lh_os_system_win_window_send_button(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                    lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                    lh_os_system_win_lparam_t lparam)
{
    const lh_bool_t down = lh_os_system_win_input_is_button_down(msg);

    if (down)
    {
        SetCapture(hwnd);
    }
    lh_os_system_win_window_send_pointer(
        window, down ? lh_os_input_kind_pointer_down : lh_os_input_kind_pointer_up,
        lh_os_system_win_input_button_of(msg, wparam), wparam, lparam);
    if (!down && lh_os_system_win_input_buttons_of(wparam) == 0U && GetCapture() == hwnd)
    {
        ReleaseCapture();
    }
}

/** A wheel message: its point is on the screen, the event's in the client. */
static lh_void
lh_os_system_win_window_send_wheel(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                   lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                   lh_os_system_win_lparam_t lparam)
{
    lh_os_input_event_t event;
    lh_os_system_win_point_t cursor;
    lh_s32_t dx;
    lh_s32_t dy;

    cursor.x = lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_LOWORD(lparam)));
    cursor.y = lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_HIWORD(lparam)));
    ScreenToClient(hwnd, lh_addr_of(cursor));
    lh_os_system_win_input_wheel_of(msg, wparam, lh_addr_of(dx), lh_addr_of(dy));
    lh_os_input_event_init_wheel(lh_addr_of(event), lh_os_input_coord_from_int(cursor.x),
                                 lh_os_input_coord_from_int(cursor.y), dx, dy, lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(event), lh_os_system_win_input_buttons_of(wparam));
    lh_os_input_event_set_mods(lh_addr_of(event), lh_os_system_win_input_get_mods());
    lh_os_window_on_native_input(window, lh_addr_of(event));
}

/* -- Touch and pen --------------------------------------------------------- */

/** Whether the pointer API has been looked up in user32 yet: it is there from
   Windows 8 on, and importing it would keep the program from starting on Windows 7
   and XP. */
static lh_bool_t lh_os_system_win_window_pointer_api_looked_up;

/** `GetPointerType`, or null where the system has no pointer API. */
static lh_os_system_win_get_pointer_type_proc_t lh_os_system_win_window_get_pointer_type;

/** `GetPointerInfo`, or null where the system has no pointer API. */
static lh_os_system_win_get_pointer_info_proc_t lh_os_system_win_window_get_pointer_info;

/** The pointer id each live touch contact and pen goes by. One table for the process:
   the system's pointer ids are unique across its windows, and a contact belongs to the
   window it went down on until it goes up. */
static lh_os_input_contacts_t lh_os_system_win_window_contacts =
    lh_os_input_contacts_empty_initializer();

/** True when this system sends pointer messages (Windows 8 and later); the API is
   looked up on the first call. */
static lh_bool_t
lh_os_system_win_window_has_pointer_api(lh_void)
{
    if (!lh_os_system_win_window_pointer_api_looked_up)
    {
        const lh_os_system_win_handle_t user32 = GetModuleHandleA("user32.dll");

        lh_os_system_win_window_pointer_api_looked_up = lh_bool_true;
        if (lh_null_ne(user32))
        {
            lh_os_system_win_window_get_pointer_type = lh_cast_reinterpret(
                lh_os_system_win_get_pointer_type_proc_t, GetProcAddress(user32, "GetPointerType"));
            lh_os_system_win_window_get_pointer_info = lh_cast_reinterpret(
                lh_os_system_win_get_pointer_info_proc_t, GetProcAddress(user32, "GetPointerInfo"));
        }
    }
    return lh_null_ne(lh_ptr_rcast(lh_void, lh_os_system_win_window_get_pointer_type));
}

/** True when the mouse message being handled is one the system made out of a pen or a
   touch contact that the pointer messages already delivered as itself. Where there is
   no pointer API (Windows 7, XP) such a message is all there is of the contact, so it
   stays, as pointer `0` with the device kind it came from. */
static lh_bool_t
lh_os_system_win_window_is_contact_echo(lh_void)
{
    return lh_type_cast(lh_bool_t,
                        lh_os_system_win_window_has_pointer_api() &&
                            lh_os_system_win_input_is_promoted_mouse(GetMessageExtraInfo()));
}

/** What the system says of pointer @p pointer_id at the current message: its device
   kind, its flags and its point on the screen. `GetPointerInfo` when it answers, else
   `GetPointerType` with the flags of @p wparam and the point of @p lparam. False, the
   outs untouched, when the system no longer knows the pointer. */
static lh_bool_t
lh_os_system_win_window_read_pointer(lh_os_system_win_pointer_id_t pointer_id,
                                     lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam,
                                     lh_os_input_device_kind_t *device_kind,
                                     lh_os_system_win_pointer_flags_t *flags,
                                     lh_os_system_win_point_t *at)
{
    lh_os_system_win_pointer_info_t info;
    lh_os_system_win_pointer_input_type_t type = 0UL;

    if (lh_null_ne(lh_ptr_rcast(lh_void, lh_os_system_win_window_get_pointer_info)) &&
        lh_os_system_win_window_get_pointer_info(pointer_id, lh_addr_of(info)))
    {
        lh_ptr_deref(device_kind) =
            lh_os_system_win_input_device_kind_of_pointer_type(info.pointer_type);
        lh_ptr_deref(flags) = info.pointer_flags;
        lh_ptr_deref(at) = info.pixel_location;
        return lh_bool_true;
    }
    lh_return_if(!lh_os_system_win_window_get_pointer_type(pointer_id, lh_addr_of(type)),
                 lh_bool_false);
    lh_ptr_deref(device_kind) = lh_os_system_win_input_device_kind_of_pointer_type(type);
    lh_ptr_deref(flags) = lh_os_system_win_input_pointer_flags_of(wparam);
    at->x = lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_LOWORD(lparam)));
    at->y = lh_type_cast(int, lh_type_cast(lh_sshort_t, LH_OS_SYSTEM_WIN_HIWORD(lparam)));
    return lh_bool_true;
}

/** The pointer id the contact of @p kind goes by, into @p pointer: a press takes the
   lowest free one, anything else finds the one its press took. False when there is
   none -- every id taken, or a contact never seen going down (a pen hovering). */
static lh_bool_t
lh_os_system_win_window_contact_pointer(lh_os_system_win_pointer_id_t pointer_id,
                                        lh_os_input_kind_t kind,
                                        lh_os_input_device_kind_t device_kind,
                                        lh_os_input_pointer_id_t *pointer)
{
    lh_return_if(kind == lh_os_input_kind_pointer_down,
                 lh_os_input_contacts_take(lh_addr_of(lh_os_system_win_window_contacts), pointer_id,
                                           device_kind, pointer));
    return lh_os_input_contacts_find(lh_addr_of(lh_os_system_win_window_contacts), pointer_id,
                                     pointer);
}

/** A pointer message about a touch contact or a pen, to @p window as an event of the
   pointer id its contact goes by (`1` and up, `0` stays the mouse's), at its client
   point. True when the message is taken. False for one that is left to
   `DefWindowProc`: no pointer API, a mouse or a touchpad (their mouse messages
   follow), a pointer the system and the table both no longer know. */
static lh_bool_t
lh_os_system_win_window_send_contact(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                     lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam)
{
    const lh_os_system_win_pointer_id_t pointer_id = LH_OS_SYSTEM_WIN_GET_POINTERID_WPARAM(wparam);
    lh_os_input_device_kind_t device_kind = lh_os_input_device_kind_none;
    lh_os_system_win_pointer_flags_t flags = 0U;
    lh_os_system_win_point_t at = {0, 0};
    lh_os_input_pointer_id_t pointer = 0U;
    lh_os_input_kind_t kind;
    lh_os_input_event_t event;

    lh_return_if(!lh_os_system_win_window_has_pointer_api(), lh_bool_false);
    if (!lh_os_system_win_window_read_pointer(pointer_id, wparam, lparam, lh_addr_of(device_kind),
                                              lh_addr_of(flags), lh_addr_of(at)))
    {
        /* Gone from the system before its message came (a capture change after the
           contact ended): only a contact still in the table is ours to cancel. */
        lh_return_if(!lh_os_input_contacts_find(lh_addr_of(lh_os_system_win_window_contacts),
                                                pointer_id, lh_addr_of(pointer)),
                     lh_bool_false);
        device_kind = lh_os_input_contacts_get_device_kind(
            lh_addr_of(lh_os_system_win_window_contacts), pointer);
        flags = LH_OS_SYSTEM_WIN_POINTER_FLAG_CANCELED;
    }
    lh_return_if(device_kind != lh_os_input_device_kind_touch &&
                     device_kind != lh_os_input_device_kind_pen,
                 lh_bool_false);
    kind = lh_os_system_win_input_pointer_kind_of(lh_type_cast(lh_u32_t, msg), flags);
    /* A contact with no id is not delivered at all, and not handed on either: its
       mouse echo would only be skipped. */
    lh_return_if(!lh_os_system_win_window_contact_pointer(pointer_id, kind, device_kind,
                                                          lh_addr_of(pointer)),
                 lh_type_cast(lh_bool_t, kind == lh_os_input_kind_pointer_down));
    if (kind != lh_os_input_kind_pointer_cancel)
    {
        ScreenToClient(hwnd, lh_addr_of(at));
    }
    lh_os_input_event_init_pointer(
        lh_addr_of(event), kind, device_kind, pointer,
        kind == lh_os_input_kind_pointer_down || kind == lh_os_input_kind_pointer_up
            ? lh_os_input_button_primary
            : lh_os_input_button_none,
        lh_os_input_coord_from_int(kind == lh_os_input_kind_pointer_cancel ? 0 : at.x),
        lh_os_input_coord_from_int(kind == lh_os_input_kind_pointer_cancel ? 0 : at.y),
        lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(event),
                                  (flags & LH_OS_SYSTEM_WIN_POINTER_FLAG_INCONTACT) != 0U &&
                                          kind != lh_os_input_kind_pointer_up &&
                                          kind != lh_os_input_kind_pointer_cancel
                                      ? lh_os_input_button_to_mask(lh_os_input_button_primary)
                                      : 0U);
    lh_os_input_event_set_mods(lh_addr_of(event), lh_os_system_win_input_get_mods());
    lh_os_window_on_native_input(window, lh_addr_of(event));
    /* After the event, not before it. The table is one for the process, and the handler
       may pump messages: a down that arrives while this up is still in its handler must
       not be given the pointer id this event already carries. The slot is freed once the
       handler returns, including when that handler closed the window. */
    if (kind == lh_os_input_kind_pointer_up || kind == lh_os_input_kind_pointer_cancel)
    {
        lh_os_input_contacts_release(lh_addr_of(lh_os_system_win_window_contacts), pointer);
    }
    return lh_bool_true;
}

/** A key message, plain or with Alt (`WM_SYS*`); the virtual key goes along as the
   scancode, so a key ::lh_os_key_t has no name for can still be told apart. */
static lh_void
lh_os_system_win_window_send_key(lh_os_window_t *window, lh_os_system_win_uint_t msg,
                                 lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam)
{
    lh_os_input_event_t event;
    const lh_u32_t vk = lh_type_cast(lh_u32_t, wparam);
    const lh_bool_t down = lh_type_cast(lh_bool_t, msg == LH_OS_SYSTEM_WIN_WM_KEYDOWN ||
                                                       msg == LH_OS_SYSTEM_WIN_WM_SYSKEYDOWN);

    lh_os_input_event_init_key(lh_addr_of(event), lh_os_system_win_key_from_vk(vk), down,
                               down ? lh_os_system_win_input_is_repeat(lparam) : lh_bool_false, vk,
                               lh_os_tick_ms());
    lh_os_input_event_set_mods(lh_addr_of(event), lh_os_system_win_input_get_mods());
    lh_os_window_on_native_input(window, lh_addr_of(event));
}

/** Where a window opens. Win32 has exactly one answer for this — `CW_USEDEFAULT`,
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
        lh_ptr_deref(x) = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;
        lh_ptr_deref(y) = LH_OS_SYSTEM_WIN_CW_USEDEFAULT;
        return;
    }

    centered_x = monitor_info.work.left + (monitor_info.work.right - monitor_info.work.left -
                                           (window_rect->right - window_rect->left)) /
                                              2;
    centered_y = monitor_info.work.top + (monitor_info.work.bottom - monitor_info.work.top -
                                          (window_rect->bottom - window_rect->top)) /
                                             2;
    lh_ptr_deref(x) = centered_x > monitor_info.work.left ? centered_x : monitor_info.work.left;
    lh_ptr_deref(y) = centered_y > monitor_info.work.top ? centered_y : monitor_info.work.top;
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
    region = CreateRoundRectRgn(0, 0, window.right - window.left + 1,
                                window.bottom - window.top + 1, radius * 2, radius * 2);
    if (lh_null_eq(region))
    {
        return;
    }
    SetWindowRgn(hwnd, region, LH_OS_SYSTEM_WIN_TRUE);
}

static lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam)
{
    lh_os_window_t *window;

    if (msg == LH_OS_SYSTEM_WIN_WM_NCCREATE)
    {
        const lh_os_system_win_createstructa_t *cs = lh_ptr_rcast(
            const lh_os_system_win_createstructa_t, lh_cast_reinterpret(lh_ptr, lparam));
        window = lh_ptr_rcast(lh_os_window_t, cs->lpCreateParams);
        SetWindowLongPtrA(hwnd, LH_OS_SYSTEM_WIN_GWLP_USERDATA,
                          lh_cast_reinterpret(lh_os_system_win_lparam_t, window));
        if (lh_null_ne(window))
        {
            window->handle = lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
        }
    }

    window = lh_cast_reinterpret(
        lh_os_window_t *,
        lh_cast_reinterpret(lh_ptr, GetWindowLongPtrA(hwnd, LH_OS_SYSTEM_WIN_GWLP_USERDATA)));

    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_NCHITTEST:
        if (lh_null_ne(window))
        {
            /* The point arrives in screen coordinates; the app thinks in client ones,
               and it is the app that knows what its own chrome is. */
            lh_os_system_win_point_t at;

            at.x =
                lh_cast_static(int, lh_cast_static(lh_sshort_t, LH_OS_SYSTEM_WIN_LOWORD(lparam)));
            at.y =
                lh_cast_static(int, lh_cast_static(lh_sshort_t, LH_OS_SYSTEM_WIN_HIWORD(lparam)));
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
            const int reason = lh_cast_static(int, LH_OS_SYSTEM_WIN_LOWORD(wparam));

            if (GetClientRect(hwnd, lh_addr_of(client)))
            {
                lh_os_window_on_native_resize(
                    window, lh_cast_static(int, client.right), lh_cast_static(int, client.bottom),
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
    case LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_XBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_LBUTTONUP:
    case LH_OS_SYSTEM_WIN_WM_RBUTTONUP:
    case LH_OS_SYSTEM_WIN_WM_MBUTTONUP:
    case LH_OS_SYSTEM_WIN_WM_XBUTTONUP:
        /* A press the system made out of a contact the pointer messages delivered
           is not a second press. */
        if (lh_null_ne(window) && !lh_os_system_win_window_is_contact_echo())
        {
            lh_os_system_win_window_send_button(window, hwnd, msg, wparam, lparam);
        }
        /* An X button message says it was handled by returning TRUE, every other by 0. */
        return msg == LH_OS_SYSTEM_WIN_WM_XBUTTONDOWN || msg == LH_OS_SYSTEM_WIN_WM_XBUTTONUP
                   ? LH_OS_SYSTEM_WIN_TRUE
                   : 0;
    case LH_OS_SYSTEM_WIN_WM_MOUSEMOVE:
        if (lh_null_ne(window) && !lh_os_system_win_window_is_contact_echo())
        {
            lh_os_system_win_window_send_pointer(window, lh_os_input_kind_pointer_move,
                                                 lh_os_input_button_none, wparam, lparam);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_CAPTURECHANGED:
        /* Another window took the mouse while a button was held here: that press is
           never going to be released to us, so it is cancelled rather than left down.
           Our own ReleaseCapture comes after the last button went up, with none held. */
        if (lh_null_ne(window) && lh_cast_reinterpret(lh_os_system_win_hwnd_t, lparam) != hwnd &&
            lh_os_window_get_pressed_buttons(window) != 0U)
        {
            lh_os_system_win_window_send_pointer(window, lh_os_input_kind_pointer_cancel,
                                                 lh_os_input_button_none, 0U, 0);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_POINTERDOWN:
    case LH_OS_SYSTEM_WIN_WM_POINTERUPDATE:
    case LH_OS_SYSTEM_WIN_WM_POINTERUP:
    case LH_OS_SYSTEM_WIN_WM_POINTERCAPTURECHANGED:
        if (lh_null_ne(window) &&
            lh_os_system_win_window_send_contact(window, hwnd, msg, wparam, lparam))
        {
            return 0;
        }
        break;
    case LH_OS_SYSTEM_WIN_WM_MOUSEWHEEL:
    case LH_OS_SYSTEM_WIN_WM_MOUSEHWHEEL:
        if (lh_null_ne(window))
        {
            lh_os_system_win_window_send_wheel(window, hwnd, msg, wparam, lparam);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_KEYDOWN:
    case LH_OS_SYSTEM_WIN_WM_KEYUP:
        if (lh_null_ne(window))
        {
            lh_os_system_win_window_send_key(window, msg, wparam, lparam);
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_SYSKEYDOWN:
    case LH_OS_SYSTEM_WIN_WM_SYSKEYUP:
        /* Heard, and then still handed on: Alt+F4 and the system menu live there. */
        if (lh_null_ne(window))
        {
            lh_os_system_win_window_send_key(window, msg, wparam, lparam);
        }
        break;
    case LH_OS_SYSTEM_WIN_WM_CHAR:
        if (lh_null_ne(window))
        {
            const lh_u32_t code = lh_os_system_win_text_from_char(lh_type_cast(lh_u32_t, wparam));

            if (lh_os_system_win_is_text(code))
            {
                lh_os_input_event_t event;

                lh_os_input_event_init_text(lh_addr_of(event), code, lh_os_tick_ms());
                lh_os_input_event_set_mods(lh_addr_of(event), lh_os_system_win_input_get_mods());
                lh_os_window_on_native_input(window, lh_addr_of(event));
            }
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_SETFOCUS:
    case LH_OS_SYSTEM_WIN_WM_KILLFOCUS:
        if (lh_null_ne(window))
        {
            lh_os_input_event_t event;

            lh_os_input_event_init(lh_addr_of(event),
                                   msg == LH_OS_SYSTEM_WIN_WM_SETFOCUS ? lh_os_input_kind_focus_in
                                                                       : lh_os_input_kind_focus_out,
                                   lh_os_input_device_kind_keyboard, lh_os_tick_ms());
            lh_os_window_on_native_input(window, lh_addr_of(event));
        }
        return 0;
    case LH_OS_SYSTEM_WIN_WM_TIMER:
        if (lh_null_ne(window))
        {
            /* Every timer id is delivered to the window, and only one is ours. A timer
               another piece of code put on this window must not be read as the app's
               tick -- the callback would then run at that one's period, which is the one
               number the app chose not to. */
            if (lh_cast_static(lh_os_system_win_uintptr_t, wparam) ==
                lh_cast_static(lh_os_system_win_uintptr_t, LH_OS_SYSTEM_WIN_TIMER_ID))
            {
                lh_os_window_on_native_tick(window);
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

/** Register the window class once per process; its atom, or 0 on failure. */
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
        lh_os_system_win_window_centered_origin(lh_addr_of(rect), owner_hwnd, lh_addr_of(x),
                                                lh_addr_of(y));
    }

    hwnd = CreateWindowExA(ex_style, LH_OS_SYSTEM_WIN_WINDOW_CLASS_NAME, title, style, x, y,
                           rect.right - rect.left, rect.bottom - rect.top, owner_hwnd, lh_null,
                           instance, user);
    if (hwnd == lh_null)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    lh_os_system_window_set_corner_radius(lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd),
                                          corner);

    ShowWindow(hwnd, LH_OS_SYSTEM_WIN_SW_SHOW);
    UpdateWindow(hwnd);
    return lh_cast_reinterpret(lh_os_system_window_handle_t, hwnd);
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
lh_os_system_window_invalidate_rect(lh_os_system_window_handle_t handle, int left, int top,
                                    int right, int bottom)
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

/** The one timer id this backend uses, and the only one a window of ours may own. */
#define LH_OS_SYSTEM_WIN_TIMER_ID 1U

/** The shortest period a Windows timer can be relied on to keep: USER_TIMER_MINIMUM. */
#define LH_OS_SYSTEM_WIN_TIMER_FLOOR_MS 10U

lh_void
lh_os_system_window_set_timer(lh_os_system_window_handle_t handle, lh_u32_t ms)
{
    lh_os_system_win_hwnd_t hwnd;

    if (lh_null_eq(handle))
    {
        return;
    }
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    if (ms == 0U)
    {
        KillTimer(hwnd, LH_OS_SYSTEM_WIN_TIMER_ID);
        return;
    }
    /* Not `SetTimer`'s return value that matters: `SetTimer` returns an id, and **0**
       means the timer could not be made -- not the id it was given, so a window that
       silently stopped ticking looks exactly like a window whose timer never fired.
       Retried at the system's floor rather than left dead: a caller asking for 1 ms on
       a system whose resolution is 15.6 ms asked for as fast as it can, and refusing
       turns a too-fast request into no timer at all. */
    if (SetTimer(hwnd, LH_OS_SYSTEM_WIN_TIMER_ID, lh_cast_static(lh_os_system_win_uint_t, ms),
                 lh_null) == 0)
    {
        SetTimer(hwnd, LH_OS_SYSTEM_WIN_TIMER_ID,
                 lh_cast_static(lh_os_system_win_uint_t, LH_OS_SYSTEM_WIN_TIMER_FLOOR_MS), lh_null);
    }
}

lh_bool_t
lh_os_system_window_get_client_size(lh_os_system_window_handle_t handle, int *width, int *height)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t client;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(width) || lh_null_eq(height), lh_bool_false);
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    lh_return_if(GetClientRect(hwnd, lh_addr_of(client)) == 0, lh_bool_false);
    lh_ptr_deref(width) = client.right - client.left;
    lh_ptr_deref(height) = client.bottom - client.top;
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_paint_region(lh_ptr paint_dc, lh_u64_t *area, lh_u32_t *count)
{
    /* On the stack, in rects so it is aligned for them: a header and about 250
       rectangles, which is far more than an invalidation of widgets makes. A region
       that does not fit is reported as unknown rather than allocated for inside a
       paint. */
    lh_os_system_win_rect_t buffer[256];
    const lh_os_system_win_rgndataheader_t *header =
        lh_ptr_rcast(const lh_os_system_win_rgndataheader_t, buffer);
    const lh_os_system_win_rect_t *rects;
    lh_os_system_win_handle_t rgn;
    lh_os_system_win_dword_t bytes;
    lh_os_system_win_dword_t i;
    lh_int_t got;
    lh_bool_t ok = lh_bool_false;

    lh_return_if(lh_null_eq(area) || lh_null_eq(count), lh_bool_false);
    lh_ptr_deref(area) = 0U;
    lh_ptr_deref(count) = 0U;
    lh_return_if(lh_null_eq(paint_dc), lh_bool_false);
    rgn = CreateRectRgn(0, 0, 0, 0);
    lh_return_if(lh_null_eq(rgn), lh_bool_false);
    got = GetRandomRgn(lh_cast_reinterpret(lh_os_system_win_hdc_t, paint_dc), rgn,
                       LH_OS_SYSTEM_WIN_SYSRGN);
    if (got == 0)
    {
        /* No system region: nothing was asked for. */
        ok = lh_bool_true;
    }
    else if (got == 1)
    {
        bytes = GetRegionData(rgn, 0U, lh_null);
        if (bytes != 0U && bytes <= sizeof(buffer) && GetRegionData(rgn, bytes, buffer) == bytes)
        {
            /* The rectangles never overlap, so their areas add up to the region's. They
               are in screen coordinates on NT and client ones on 9x, which moves them
               and does not resize them -- and only the size is wanted. */
            rects = lh_ptr_rcast(const lh_os_system_win_rect_t,
                                 lh_ptr_rcast(const lh_byte_t, buffer) + header->dwSize);
            for (i = 0U; i < header->nCount; ++i)
            {
                lh_ptr_deref(area) += lh_cast_static(lh_u64_t, rects[i].right - rects[i].left) *
                                      lh_cast_static(lh_u64_t, rects[i].bottom - rects[i].top);
            }
            lh_ptr_deref(count) = lh_cast_static(lh_u32_t, header->nCount);
            ok = lh_bool_true;
        }
    }
    (void)DeleteObject(rgn);
    return ok;
}

lh_bool_t
lh_os_system_window_get_position(lh_os_system_window_handle_t handle, int *x, int *y)
{
    lh_os_system_win_hwnd_t hwnd;
    lh_os_system_win_rect_t window;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(x) || lh_null_eq(y), lh_bool_false);
    hwnd = lh_cast_reinterpret(lh_os_system_win_hwnd_t, handle);
    lh_return_if(GetWindowRect(hwnd, lh_addr_of(window)) == 0, lh_bool_false);
    lh_ptr_deref(x) = window.left;
    lh_ptr_deref(y) = window.top;
    return lh_bool_true;
}
