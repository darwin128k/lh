/**
 * @file controller.c
 * @brief Implementation of `lh/ui/view/controller.h`.
 */

#include <lh/ui/view.h>
#include <lh/ui/view/controller.h>

lh_bool_t
lh_ui_view_controller_handle(lh_self_ptr self, lh_os_input_event_cptr event)
{
    return lh_ui_view_input(lh_ptr_rcast(lh_ui_view_t, self), event);
}

lh_void
lh_ui_view_controller_cancel(lh_self_ptr self)
{
    lh_ui_view_cancel(lh_ptr_rcast(lh_ui_view_t, self));
}
