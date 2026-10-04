#include <lh/entity/button.h>
#include <lh/assert.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_button_show_pressed(lh_entity_button_t *self, lh_bool_t down)
{
    lh_entity_2d_t *const box = lh_ptr_rcast(lh_entity_2d_t, self);
    if (down)
    {
        if (lh_ptr_is_set(self->pressed_style) && lh_ptr_is_null(self->rest_style))
        {
            self->rest_style = lh_entity_2d_get_style(box);
            lh_entity_2d_set_style(box, self->pressed_style);
        }
        return;
    }
    if (lh_ptr_is_set(self->rest_style))
    {
        const lh_ui_style_t *const rest = self->rest_style;
        self->rest_style = lh_null;
        lh_entity_2d_set_style(box, rest);
    }
}

lh_void
lh_entity_button_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_button_t *const button = lh_ptr_rcast(lh_entity_button_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    lh_entity_screen_t *screen;

    if (code == LH_ENTITY_EVENT_DELETE)
    {
        screen = lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
        if (lh_ptr_is_set(screen))
        {
            lh_entity_screen_clear_pressed(screen, self);
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        button->pressed = lh_bool_true;
        lh_entity_button_show_pressed(button, lh_bool_true);
        if (button->repeat)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP && button->pressed)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        const lh_bool_t inside =
            lh_ptr_is_set(point) &&
            lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point);
        button->pressed = lh_bool_false;
        lh_entity_button_show_pressed(button, lh_bool_false);
        if (inside && !button->repeat)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        }
    }
}

const lh_entity_class_t lh_entity_button_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_button_t), lh_null, lh_null,
                                lh_entity_button_on_event);

lh_bool_t
lh_entity_button_is_pressed(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

const lh_ui_style_t *
lh_entity_button_get_pressed_style(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed_style;
}

lh_void
lh_entity_button_set_pressed_style(lh_entity_button_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->pressed_style = style;
}

lh_bool_t
lh_entity_button_get_repeat(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->repeat;
}

lh_void
lh_entity_button_set_repeat(lh_entity_button_t *self, lh_bool_t repeat)
{
    lh_assert_runtime_ref(self);
    self->repeat = repeat;
}
