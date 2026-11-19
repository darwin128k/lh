/**
 * @file input.c
 * @brief Implementation of `lh/gx/input.h`.
 */

#include <lh/gx/input.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_void
lh_gx_input_take(lh_gx_pointer_t *pointer, lh_gx_focus_t *focus, lh_gx_entity_t *top,
                 const lh_gx_event_t *event)
{
    const lh_gx_event_kind_t kind = lh_gx_event_get_kind(event);

    if (lh_gx_event_is_pointer(event))
    {
        lh_gx_pointer_take(pointer, top, event);
        lh_return_if(kind != lh_gx_event_kind_press);
        lh_gx_focus_press(focus, lh_gx_pointer_get_captured(pointer));
        return;
    }
    lh_return_if(kind != lh_gx_event_kind_key && kind != lh_gx_event_kind_text);
    LH_ATTRIBUTE_UNUSED(lh_gx_focus_take(focus, event));
}
