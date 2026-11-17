/**
 * @file system.c
 * @brief Implementation of `lh/os/window/backend/system.h`.
 */

#include <lh/os/window/backend/system.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>

lh_os_system_window_handle_t
lh_os_window_backend_system_get_handle(const lh_os_window_t *window)
{
    return lh_os_window_get_native(window);
}

lh_bool_t
lh_os_window_backend_system_open(lh_ptr context, lh_os_window_t *window, const lh_char_t *title,
                                 int width, int height, lh_os_window_t *owner)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_handle_t owner_handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;

    if (lh_null_ne(owner))
    {
        owner_handle = lh_os_window_backend_system_get_handle(owner);
    }
    /* Win32 and X11 store the handle on the window while it is being created, because
       their first messages already need it; AppKit stores it here. Either way it is the
       window's native state once this returns. */
    lh_os_system_window_handle_t handle = lh_os_system_window_open(
        title, width, height, window, owner_handle, lh_os_window_get_frame(window),
        lh_os_window_get_corner(window), lh_os_window_get_placement(window));

    lh_return_if(!lh_os_system_window_is_valid(handle), lh_bool_false);
    lh_os_window_set_native(window, handle);
    return lh_bool_true;
}

lh_void
lh_os_window_backend_system_close(lh_ptr context, lh_os_window_t *window)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_close(lh_os_window_backend_system_get_handle(window));
}

lh_void
lh_os_window_backend_system_invalidate(lh_ptr context, lh_os_window_t *window)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_invalidate(lh_os_window_backend_system_get_handle(window));
}

lh_void
lh_os_window_backend_system_invalidate_rect(lh_ptr context, lh_os_window_t *window, int left,
                                            int top, int right, int bottom)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_invalidate_rect(lh_os_window_backend_system_get_handle(window), left, top,
                                        right, bottom);
}

lh_tick_t
lh_os_window_backend_system_now(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_tick_ms();
}

lh_void
lh_os_window_backend_system_set_timer(lh_ptr context, lh_os_window_t *window, lh_u32_t ms)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_set_timer(lh_os_window_backend_system_get_handle(window), ms);
}

lh_void
lh_os_window_backend_system_set_enabled(lh_ptr context, lh_os_window_t *window, lh_bool_t enabled)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_set_enabled(lh_os_window_backend_system_get_handle(window), enabled);
}

lh_void
lh_os_window_backend_system_minimize(lh_ptr context, lh_os_window_t *window)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_minimize(lh_os_window_backend_system_get_handle(window));
}

lh_void
lh_os_window_backend_system_set_maximized(lh_ptr context, lh_os_window_t *window,
                                          lh_bool_t maximized)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_set_maximized(lh_os_window_backend_system_get_handle(window), maximized);
}

lh_void
lh_os_window_backend_system_set_corner_radius(lh_ptr context, lh_os_window_t *window, int radius)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_set_corner_radius(lh_os_window_backend_system_get_handle(window), radius);
}

lh_bool_t
lh_os_window_backend_system_get_client_size(lh_ptr context, const lh_os_window_t *window,
                                            int *width, int *height)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_system_window_get_client_size(lh_os_window_backend_system_get_handle(window),
                                               width, height);
}

lh_bool_t
lh_os_window_backend_system_get_position(lh_ptr context, const lh_os_window_t *window, int *x,
                                         int *y)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_system_window_get_position(lh_os_window_backend_system_get_handle(window), x, y);
}

lh_bool_t
lh_os_window_backend_system_get_paint_region(lh_ptr context, lh_ptr target, lh_u64_t *area,
                                             lh_u32_t *count)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_system_window_get_paint_region(target, area, count);
}

lh_u64_t
lh_os_window_backend_system_now_us(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_tick_us();
}

lh_void
lh_os_window_backend_system_init(lh_os_window_backend_t *table)
{
    lh_os_window_backend_init(table);
    lh_os_window_backend_set_open(table, lh_os_window_backend_system_open);
    lh_os_window_backend_set_close(table, lh_os_window_backend_system_close);
    lh_os_window_backend_set_invalidate(table, lh_os_window_backend_system_invalidate);
    lh_os_window_backend_set_invalidate_rect(table, lh_os_window_backend_system_invalidate_rect);
    lh_os_window_backend_set_now(table, lh_os_window_backend_system_now);
    lh_os_window_backend_set_set_timer(table, lh_os_window_backend_system_set_timer);
    lh_os_window_backend_set_set_enabled(table, lh_os_window_backend_system_set_enabled);
    lh_os_window_backend_set_minimize(table, lh_os_window_backend_system_minimize);
    lh_os_window_backend_set_set_maximized(table, lh_os_window_backend_system_set_maximized);
    lh_os_window_backend_set_set_corner_radius(table,
                                               lh_os_window_backend_system_set_corner_radius);
    lh_os_window_backend_set_get_client_size(table, lh_os_window_backend_system_get_client_size);
    lh_os_window_backend_set_get_position(table, lh_os_window_backend_system_get_position);
    lh_os_window_backend_set_get_paint_region(table, lh_os_window_backend_system_get_paint_region);
    lh_os_window_backend_set_now_us(table, lh_os_window_backend_system_now_us);
}
