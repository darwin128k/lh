/**
 * @file sink.c
 * @brief Implementation of `lh/gl/view/sink.h`.
 */

#include <lh/gl/view/sink.h>

#include <lh/core/attribute/unused.h>
#include <lh/gl/view.h>

lh_void
lh_gl_view_sink(lh_self_ptr self, lh_input_event_cptr event)
{
    LH_ATTRIBUTE_UNUSED(lh_gl_view_input(lh_ptr_rcast(lh_gl_view_t, self), event));
}
