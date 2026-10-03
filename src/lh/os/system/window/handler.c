/**
 * @file handler.c
 * @brief The process-wide window event handler, shared by every backend.
 *
 * Single-threaded like the backends themselves: windows are pumped from one
 * thread.
 */

#include <lh/null.h>
#include <lh/os/system/window/emit.h>
#include <lh/util/ptr.h>

static lh_os_system_window_handler_cb lh_os_system_window_handler;
static lh_self_ptr lh_os_system_window_handler_self;

void
lh_os_system_window_set_handler(lh_os_system_window_handler_cb handler, lh_self_ptr self)
{
    lh_os_system_window_handler = handler;
    lh_os_system_window_handler_self = self;
}

void
lh_os_system_window_emit(lh_os_system_window_handle_t window, lh_uint_t type, lh_int_t x,
                         lh_int_t y, lh_int_t width, lh_int_t height, lh_int_t button)
{
    if (lh_ptr_is_null(lh_os_system_window_handler))
    {
        return;
    }

    lh_os_system_window_event_t event;
    event.type = type;
    event.x = x;
    event.y = y;
    event.width = width;
    event.height = height;
    event.button = button;
    lh_os_system_window_handler(lh_os_system_window_handler_self, window, &event);
}
