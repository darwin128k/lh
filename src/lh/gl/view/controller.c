/**
 * @file controller.c
 * @brief Implementation of `lh/gl/view/controller.h`.
 */

#include <lh/gl/view.h>
#include <lh/gl/view/controller.h>

lh_bool_t
lh_gl_view_controller_handle_event_is_handled(lh_self_ptr self, lh_input_event_cptr event)
{
    return lh_gl_view_input(lh_ptr_rcast(lh_gl_view_t, self), event);
}

lh_void
lh_gl_view_controller_cancel(lh_self_ptr self)
{
    lh_gl_view_cancel(lh_ptr_rcast(lh_gl_view_t, self));
}
