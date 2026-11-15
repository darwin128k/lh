/**
 * @file event.c
 * @brief Apply one pointer event to the held entity.
 */
#include <lh/gl/input/event.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

const lh_gl_entity_t *
lh_gl_input_apply(lh_gl_input_t *self, const lh_gl_layer_t *layer,
                  const lh_os_input_event_ptr event)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(layer);
    lh_assert_runtime_ref(event);
    lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);
    lh_u8_t pointer = lh_os_input_event_get_pointer(event);

    if (kind == lh_os_input_kind_pointer_down)
    {
        if (lh_gl_input_get_tracking(self) == lh_bool_true &&
            lh_gl_input_get_pointer(self) != pointer)
        {
            return lh_null;
        }
        lh_gl_point_t point;

        lh_gl_point_init(lh_addr_of(point),
                         lh_type_cast(lh_gl_scalar_t,
                                      lh_os_input_coord_to_int(lh_os_input_event_get_x(event))),
                         lh_type_cast(lh_gl_scalar_t,
                                      lh_os_input_coord_to_int(lh_os_input_event_get_y(event))));
        const lh_gl_entity_t *found = lh_gl_layer_find_entity(layer, point);

        lh_gl_input_set_captured(self, found);
        lh_gl_input_set_pointer(self, pointer);
        lh_gl_input_set_tracking(self, lh_bool_true);
        return found;
    }
    if (kind == lh_os_input_kind_pointer_move)
    {
        if (lh_gl_input_get_tracking(self) == lh_bool_true &&
            lh_gl_input_get_pointer(self) == pointer)
        {
            return lh_gl_input_move(self);
        }
        return lh_null;
    }
    if (kind == lh_os_input_kind_pointer_up || kind == lh_os_input_kind_pointer_cancel)
    {
        if (lh_gl_input_get_tracking(self) == lh_bool_true &&
            lh_gl_input_get_pointer(self) == pointer)
        {
            return lh_gl_input_release(self);
        }
        return lh_null;
    }
    return lh_null;
}
